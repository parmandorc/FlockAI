// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Agent.h"
#include "Stimulus.h"
#include "FlockAIPlayer.generated.h"

UENUM()
enum EFlockAIGamemode
{
    EGM_SpawnNewAgents,
    EGM_SpawnPositiveStimuli,
    EGM_SpawnNegativeStimuli,
};

UCLASS()
class FLOCKAI_API AFlockAIPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFlockAIPlayer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    
    // CAMERA VARIABLES
    
    /* The speed the camera moves around the level, when zoomed out */
    UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
    float ZoomedOutMoveSpeed;
    
    /* The speed the camera moves around the level, when zoomed in */
    UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
    float ZoomedInMoveSpeed;
    
    /* The distance the camera is from the action layer, when zoomed out */
    UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
    float ZoomedOutDistance;
    
    /* The distance the camera is from the action layer, when zoomed in */
    UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
    float ZoomedInDistance;
	
    
    //SPAWN REFERENCES
    
    // The preview mesh of the Agent when its is able to spawn
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    class UStaticMesh* PreviewMeshAgent;
    
    // The preview mesh of the Positive Stimulus
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    class UStaticMesh* PreviewMeshPositiveStimulus;
    
    // The preview mesh of the Negative Stimulus
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    class UStaticMesh* PreviewMeshNegativeStimulus;
    
    /* The mesh component */
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    class UStaticMeshComponent* PreviewMeshComponent;
    
    // The class of the Agent to spawn
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    TSubclassOf<AAgent> AgentBP;
    
    // The class of the Negative Stimulus to spawn
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    TSubclassOf<AStimulus> NegativeStimulusBP;
    
    // The class of the Positive Stimulus to spawn
    UPROPERTY(Category = Spawn, EditDefaultsOnly)
    TSubclassOf<AStimulus> PositiveStimulusBP;

protected:
    // CLASS COMPONENTS
    
    /* The camera */
    UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* CameraComponent;
    
    /* Camera boom positioning the camera above the character */
    UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;
    
    
    // Binding functions for input actions
    void ZoomIn();
    void ZoomOut();
    void BeginSpawning();
    void DoSpawning();
    void CancelSpawning();
    template<EFlockAIGamemode Gamemode> void ChangeGamemode();
    
    // Input variables
    bool bZoomingIn;
    float ZoomFactor;
    bool bWantToSpawn;
    FVector SpawningLocation;
    FVector MouseLocation;
    EFlockAIGamemode CurrentGamemode;
    
    /* Returns the cursor position inside the game action layer */
    FVector GetCursorPositionInActionLayer();
    
public:
    /** Returns CameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
