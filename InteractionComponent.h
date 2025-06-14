// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUHUIK_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UInteractionComponent(); //Constructor

	void PrimaryInteract(); // Function to handle primary interaction
	
	

		
};
