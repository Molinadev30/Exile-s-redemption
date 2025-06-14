// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true; // Enable ticking

	//Components
	
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp"); // Create a sphere component for collision
	RootComponent = SphereComp;
	SphereComp->SetCollisionProfileName("Projectile"); // Set collision profile for the sphere component

	ProjectileVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileVFX")); // Create a Niagara component for VFX
	ProjectileVFX->SetupAttachment(SphereComp);
}

void AProjectileBase::Explode_Implementation()
{
	// This function is called when the projectile explodes
	
	if (ensure(IsValid(this) && ImpactVFX)  && !bHasExploded)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,             
		ImpactVFX,
		GetActorLocation(),
		GetActorRotation(),
		FVector(1.0f),   
		true,            
		true,             
		ENCPoolMethod::AutoRelease,
		true              
		);
		bHasExploded = true;
	}
}