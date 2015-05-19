// Fill out your copyright notice in the Description page of Project Settings.

#include "FlockAI.h"
#include "FlockAIPlayer.h"


// Sets default values
AFlockAIPlayer::AFlockAIPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Default values
    ZoomedOutMoveSpeed = 3500.0f;
    ZoomedInMoveSpeed = 1750.0f;
    ZoomedOutDistance = 3000.0f;
    ZoomedInDistance = 1400.0f;
    bZoomingIn = true;
    bWantToSpawn = false;
    CurrentGamemode = EFlockAIGamemode::EGM_SpawnNewAgents;
    
    // Create the root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    
    // Create a camera boom...
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->AttachTo(RootComponent);
    CameraBoom->TargetArmLength = ZoomedInDistance;
    CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
    CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 3.0f;
    
    // Create a camera...
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    CameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
    
    // Create the spawning preview mesh
    PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
    PreviewMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PreviewMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_Destructible); // Ignored by VisionSphere in Agents
    PreviewMeshComponent->bAbsoluteLocation = true;
    PreviewMeshComponent->bAbsoluteRotation = true;
}

// Called when the game starts or when spawned
void AFlockAIPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlockAIPlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

    // Find movement direction
    const float ForwardValue = GetInputAxisValue("MoveForward");
    const float RightValue = GetInputAxisValue("MoveRight");
    
    // Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
    FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);
    
    // If non-zero size, move this actor
    if(!MoveDirection.IsZero())
    {
        const float MoveSpeed = bZoomingIn ? ZoomedInMoveSpeed : ZoomedOutMoveSpeed;
        
        MoveDirection = MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime;
        
        FVector NewLocation = GetActorLocation();
        NewLocation += GetActorForwardVector() * MoveDirection.X;
        NewLocation += GetActorRightVector() * MoveDirection.Y;
        
        SetActorLocation(NewLocation);
    }
    
    // Handle zoom camera movement
    {
        if (bZoomingIn)
            ZoomFactor -= DeltaTime / 0.5f;
        else
            ZoomFactor += DeltaTime / 0.5f;
        
        ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
        
        CameraBoom->TargetArmLength = FMath::Lerp(ZoomedInDistance, ZoomedOutDistance, ZoomFactor);
    }
    
    // Show oriented preview of Agent when spawning
    if (bWantToSpawn)
    {
        if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnNewAgents)
        {
            MouseLocation = GetCursorPositionInActionLayer();
            
            PreviewMeshComponent->SetWorldRotation( (MouseLocation - SpawningLocation).Rotation() );
        }
    }
}

// Called to bind functionality to input
void AFlockAIPlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

    check(InputComponent);
    
    // set up key bindings
    InputComponent->BindAxis("MoveForward");
    InputComponent->BindAxis("MoveRight");
    InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AFlockAIPlayer::ZoomIn);
    InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AFlockAIPlayer::ZoomOut);
    InputComponent->BindAction("Spawn", IE_Pressed, this, &AFlockAIPlayer::BeginSpawning);
    InputComponent->BindAction("Spawn", IE_Released, this, &AFlockAIPlayer::DoSpawning);
    InputComponent->BindAction("CancelSpawning", IE_Pressed, this, &AFlockAIPlayer::CancelSpawning);
    InputComponent->BindAction("Gamemode1", IE_Pressed, this, &AFlockAIPlayer::ChangeGamemode<EFlockAIGamemode::EGM_SpawnNewAgents>);
    InputComponent->BindAction("Gamemode2", IE_Pressed, this, &AFlockAIPlayer::ChangeGamemode<EFlockAIGamemode::EGM_SpawnPositiveStimuli>);
    InputComponent->BindAction("Gamemode3", IE_Pressed, this, &AFlockAIPlayer::ChangeGamemode<EFlockAIGamemode::EGM_SpawnNegativeStimuli>);
}

void AFlockAIPlayer::ZoomIn()
{
    bZoomingIn = true;
}

void AFlockAIPlayer::ZoomOut()
{
    bZoomingIn = false;
}

void AFlockAIPlayer::BeginSpawning()
{
    bWantToSpawn = true;
    
    SpawningLocation = GetCursorPositionInActionLayer();
    
    PreviewMeshComponent->SetWorldLocation(SpawningLocation);
    
    if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnNewAgents)
    {
        PreviewMeshComponent->SetStaticMesh(PreviewMeshAgent);
    }
    else if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnPositiveStimuli)
    {
        PreviewMeshComponent->SetStaticMesh(PreviewMeshPositiveStimulus);
    }
    else if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnNegativeStimuli)
    {
        PreviewMeshComponent->SetStaticMesh(PreviewMeshNegativeStimulus);
    }
}

void AFlockAIPlayer::DoSpawning()
{
    if (bWantToSpawn)
    {
        if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnNewAgents)
        {
            GetWorld()->SpawnActor<AAgent>(AgentBP, SpawningLocation, (MouseLocation - SpawningLocation).Rotation());
        }
        else if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnPositiveStimuli)
        {
            GetWorld()->SpawnActor<AStimulus>(PositiveStimulusBP, SpawningLocation, FRotator::ZeroRotator);
        }
        else if (CurrentGamemode == EFlockAIGamemode::EGM_SpawnNegativeStimuli)
        {
            GetWorld()->SpawnActor<AStimulus>(NegativeStimulusBP, SpawningLocation, FRotator::ZeroRotator);
        }
        
        bWantToSpawn = false;
        
        PreviewMeshComponent->SetStaticMesh(NULL);
    }
}

void AFlockAIPlayer::CancelSpawning()
{
    bWantToSpawn = false;
    
    PreviewMeshComponent->SetStaticMesh(NULL);
}

FVector AFlockAIPlayer::GetCursorPositionInActionLayer()
{
    FVector MousePosition;
    FVector MouseDirection;
    FVector Result;
    
    GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MousePosition, MouseDirection);
    
    Result.X = (- MousePosition.Z / MouseDirection.Z) * MouseDirection.X + MousePosition.X;
    Result.Y = (- MousePosition.Z / MouseDirection.Z) * MouseDirection.Y + MousePosition.Y;
    Result.Z = 0.0F;
    
    return Result;
}

template<EFlockAIGamemode Gamemode>
void AFlockAIPlayer::ChangeGamemode()
{
    if (CurrentGamemode != Gamemode)
    {
        CancelSpawning();
        
        CurrentGamemode = Gamemode;
        
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Set Gamemode: %d"),(int)Gamemode));
    }
}