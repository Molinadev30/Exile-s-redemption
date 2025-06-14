// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicProjectile.h"
#include "Components/SphereComponent.h"

// Sets default values
AMagicProjectile::AMagicProjectile()
{
	//Initialize properties
	
	SphereComp->SetSphereRadius(20.0f);
	DamageAmount = 20.0f;
}

void AMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Check if the instigator is valid and ignore it during movement
	
	if (APawn* ProjectileInstigator = GetInstigator())
	{
     	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true); // Ignore the instigator actor during movement
		ProjectileInstigator->MoveIgnoreActorAdd(this);	// Add this projectile to the instigator's ignore list
	}
}