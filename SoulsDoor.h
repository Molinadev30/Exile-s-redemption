// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SoulsDoor.generated.h"

class UStaticMeshComponent;

UCLASS()
class RUHUIK_API ASoulsDoor : public AActor, public IGameplayInterface
{
	GENERATED_BODY()

public:
	
	ASoulsDoor(); //Constructor
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override; // Implementation of the Interact function from IGameplayInterface

};
