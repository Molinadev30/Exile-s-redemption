// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "MagicProjectile.generated.h"

class UPoolSubsystem;

UCLASS()
class RUHUIK_API AMagicProjectile : public AProjectileBase
{
	GENERATED_BODY()

protected:

	//Properties
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

public:
	
	AMagicProjectile(); //Constructor

	//Inherited methods
	
	virtual void PostInitializeComponents() override;
};