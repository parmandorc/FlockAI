// Fill out your copyright notice in the Description page of Project Settings.

#include "FlockAI.h"
#include "Agent.h"


AAgent::AAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    
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
}

void AAgent::BeginPlay()
{
    // Initialize move vector
    NewMoveVector = GetActorRotation().Vector().GetSafeNormal();
}

void AAgent::Tick(float DeltaSeconds)
{
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