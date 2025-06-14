// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolableInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"


// Struct to hold the pool of actors

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool; //Array to hold pooled actors

	// Check if the pool is empty
	
	bool IsEmpty() const
	{
		return ObjectPool.IsEmpty();
	}
	
	// Pop an actor from the pool, returning nullptr if the pool is empty
	
	AActor* Pop()
	{
		return ObjectPool.Pop();
	}

	// Add an object to the pool
	
	void Add(AActor* ActorToAdd)
	{
		ObjectPool.Add(ActorToAdd);
	}
};

// Subsystem to manage object pooling

UCLASS()
class RUHUIK_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// Function to spawn an actor from the pool
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem", meta = (DeterminesOutputType = "PoolClass", DynamicOutputParam = "SpawnedActor")) //Set output param to be the same as the input param
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, APawn* Instigator, AActor*& SpawnedActor);

	// Template function to spawn an actor from the pool, allowing for type-specific spawning
	
	template <typename T>
	T* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, APawn* Instigator);

	// Function to return an actor to the pool
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(AActor* Poolable);

	// Map to hold pools of actors, keyed by their class type
	
	UPROPERTY()
	TMap<UClass*, FPoolArray> ObjectPools;
};

// Template function implementation to spawn an actor from the pool

template <typename T>
T* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, APawn* Instigator)
{
	T* PooledActor = nullptr; // Initialize the pooled actor to nullptr

	//Verify if the class implements the poolable interface
	
	if (PoolClass.Get()->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolClass); // Get or create the pool for the specified class

		// Check if the pool is empty, if so, spawn a new actor; otherwise, pop an actor from the pool
		
		if (ObjectPool.IsEmpty())
		{
			FActorSpawnParameters SpawnParams; // Parameters for spawning the actor
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handle collision when spawning
			SpawnParams.Instigator = Instigator; // Set the instigator for the spawned actor
			
			PooledActor = GetWorld()->SpawnActor<T>(PoolClass, Location, Rotation, SpawnParams); // Spawn a new actor of type T
		}
		else
		{
			PooledActor = CastChecked<T>(ObjectPool.Pop()); // Pop an actor from the pool and cast it to type T
			PooledActor->SetInstigator(Instigator);	// Set the instigator for the pooled actor
			PooledActor->SetActorLocationAndRotation(Location, Rotation); // Set the location and rotation of the pooled actor
		}

		IPoolableInterface::Execute_OnSpawnFromPool(PooledActor); // Call the interface method to notify the actor it has been spawned from the pool
	}
	
	return PooledActor; // Return the pooled actor, which may be nullptr if spawning failed
}
