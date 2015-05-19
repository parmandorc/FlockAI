// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Stimulus.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FLOCKAI_API AStimulus : public AActor
{
	GENERATED_BODY()
	
public:
    AStimulus();
    
    // The value of the stimulus for the Agents.
    UPROPERTY(Category = AI, EditAnywhere, BlueprintReadWrite)
    float Value;
};
