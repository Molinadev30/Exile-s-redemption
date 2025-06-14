// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolSubsystem.h"


void UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, APawn* Instigator, AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, Location, Rotation, Instigator); // Call the templated version to handle the actual spawning
}

void UPoolSubsystem::ReturnToPool(AActor* Poolable)
{
	const UClass* PoolableClass = Poolable->GetClass(); // Get the class of the actor being returned to the pool

	// Check if the class implements the poolable interface
	
	if (PoolableClass->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		IPoolableInterface::Execute_OnReturnToPool(Poolable); // Call the OnReturnToPool function on the actor
		
		FPoolArray* ObjectPool = ObjectPools.Find(PoolableClass); // Find the pool for this class
		
		ObjectPool->Add(Poolable); // Add the actor back to the pool
	}
	
	else
	{
		Poolable->Destroy(); // If the class does not implement the interface, destroy the actor
	}
}
