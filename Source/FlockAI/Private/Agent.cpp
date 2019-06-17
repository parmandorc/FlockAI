// Fill out your copyright notice in the Description page of Project Settings.

#include "FlockAI.h"
#include "Agent.h"
#include "Stimulus.h"


AAgent::AAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Initializing default values
    BaseMovementSpeed = 200.0f;
    MaxMovementSpeed = 300.0f;
    AlignmentWeight = 1.0f;
    CohesionWeight = 0.5f;
    SeparationWeight = 4.0f;
    VisionRadius = 400.0f;

    // Create the mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Create vision sphere
    VisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisionSphere"));
    VisionSphere->AttachTo(RootComponent);
    VisionSphere->SetSphereRadius(VisionRadius);

    InertiaWeigh = 0.0f;
    AgentPhysicalRadius = 45.0f;
    SeparationWeight = 0.5f;
}

void AAgent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize move vector
    NewMoveVector = GetActorRotation().Vector().GetSafeNormal();
}

void AAgent::ResetComponents()
{
    AlignmentComponent = FVector::ZeroVector;
    CohesionComponent = FVector::ZeroVector;
    SeparationComponent = FVector::ZeroVector;
    NegativeStimuliComponent = FVector::ZeroVector;
    NegativeStimuliMaxFactor = 0.0f;
    PositiveStimuliComponent = FVector::ZeroVector;
    PositiveStimuliMaxFactor = 0.0f;
}

void AAgent::DebugComponents()
{
    FVector TempVector;
    TempVector = AlignmentComponent * AlignmentWeight;
    UE_LOG(LogFlockAI, Log, TEXT("Alignment: %s"), *TempVector.ToString());
    TempVector = CohesionComponent * CohesionWeight;
    UE_LOG(LogFlockAI, Log, TEXT("Cohesion: %s"), *TempVector.ToString());
    TempVector = SeparationComponent * SeparationWeight;
    UE_LOG(LogFlockAI, Log, TEXT("Separation: %s"), *TempVector.ToString());
    UE_LOG(LogFlockAI, Log, TEXT("NegativeStimuli: %s"), *NegativeStimuliComponent.ToString());
    UE_LOG(LogFlockAI, Log, TEXT("PositiveStimuli: %s"), *PositiveStimuliComponent.ToString());
}

void AAgent::CalculateNewMoveVector_Implementation()
{
    ResetComponents();

    FVector ActorLocation = GetActorLocation();

    VisionSphere->GetOverlappingActors(Neighbourhood, StaticClass());
    // Compute Alignment Component Vector
    for (AActor* Actor : Neighbourhood)
    {
        AAgent* Agent = (AAgent*)Actor;
        AlignmentComponent += Agent->CurrentMoveVector.GetSafeNormal();
    }
    AlignmentComponent.Normalize();
    Neighbourhood.Remove(this);
    if (Neighbourhood.Num() > 0)
    {
        // Compute Cohesion Component Vector
        for (AActor* Agent : Neighbourhood)
        {
            CohesionComponent += Agent->GetActorLocation() - ActorLocation;
        }
        CohesionComponent /= Neighbourhood.Num() * 100.0f;
        // Compute Separation Component Vector
        for (AActor* Agent : Neighbourhood)
        {
            FVector SeparationVector = ActorLocation - Agent->GetActorLocation();
            SeparationComponent += SeparationVector.GetSafeNormal() / FMath::Abs(SeparationVector.Size() - AgentPhysicalRadius * 2.0f);
        }
        SeparationComponent *= (1.0f + Neighbourhood.Num() / 5.0f) * 100.0f;
    }

    TArray<AActor*> StimulusActors;
    VisionSphere->GetOverlappingActors(StimulusActors, AStimulus::StaticClass());
    for (AActor* Actor : StimulusActors)
    {
        AStimulus* Stimulus = (AStimulus*)Actor;
        FVector StimuliVector = Stimulus->GetActorLocation() - ActorLocation;
        if (Stimulus->Value < 0.0f)
        {
            // Compute Negative Stimuli Component Vector
            FVector NegativeStimuliVector = StimuliVector.GetSafeNormal() / FMath::Abs(StimuliVector.Size() - AgentPhysicalRadius) * 100.0f * Stimulus->Value;
            NegativeStimuliComponent += NegativeStimuliVector;
            NegativeStimuliMaxFactor = FMath::Max(NegativeStimuliMaxFactor, NegativeStimuliVector.Size());
        }
        else
        {
            // Compute Positive Stimuli Component Vector
            float PositiveStimuliFactor = Stimulus->Value / StimuliVector.Size();
            if (PositiveStimuliFactor > PositiveStimuliMaxFactor)
            {
                PositiveStimuliComponent = StimuliVector.GetSafeNormal() * Stimulus->Value;
                PositiveStimuliMaxFactor = PositiveStimuliFactor;
            }
        }
    }
    // Compute Aggregation of Components
    NegativeStimuliComponent = NegativeStimuliComponent.GetSafeNormal() * NegativeStimuliMaxFactor;
    NewMoveVector = NegativeStimuliComponent + PositiveStimuliComponent;
    NewMoveVector += AlignmentComponent * AlignmentWeight;
    NewMoveVector += CohesionComponent * CohesionWeight;
    NewMoveVector += SeparationComponent * SeparationWeight;

    //DebugComponents();
}

void AAgent::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CurrentMoveVector = NewMoveVector;

    CalculateNewMoveVector();

    const FVector NewDirection = (NewMoveVector * BaseMovementSpeed * DeltaSeconds).GetClampedToMaxSize(MaxMovementSpeed * DeltaSeconds);
    const FRotator NewRotation = NewMoveVector.Rotation();

    FHitResult Hit(1.f);
    RootComponent->MoveComponent(NewDirection, NewRotation, true, &Hit);

    if (Hit.IsValidBlockingHit())
    {
        const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
        const FVector Deflection = FVector::VectorPlaneProject(NewDirection, Normal2D) * (1.f - Hit.Time);
        RootComponent->MoveComponent(Deflection, NewRotation, true);
    }
}