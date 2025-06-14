// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UStaticMeshComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UPoolSubsystem;

UCLASS(Abstract) // Abstract class, can't be instantiated
class RUHUIK_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

protected:

	// Properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bHasExploded = false;

	// Components

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	UNiagaraComponent* ProjectileVFX;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* ImpactVFX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	// Methods
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();
	
};