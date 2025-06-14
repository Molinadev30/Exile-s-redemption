// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "GameplayInterface.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UInteractionComponent::PrimaryInteract()
{
	// Parameters for the object query
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner(); // Get the owner of this component
	
	FVector EyeLocation; // The location of the owner's eyes
	FRotator EyeRotation; // The rotation of the owner's eyes
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); // Get the eye location and rotation of the owner

	FVector End = EyeLocation + (EyeRotation.Vector() * 1000); // Calculate the end point of the line trace

	TArray<FHitResult> Hits; // Array to store hit results

	float Radius = 30.0f; // Radius of the sphere for the sweep

	// Create a collision shape for the sphere sweep
	
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	// Perform a sphere sweep to check for objects in the specified range
	
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red; // Color of the line based on whether a blocking hit occurred

	// Iterate through the hit results and check if the hit actor implements the gameplay interface
	
	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor(); // Get the actor that was hit by the sweep
		if(HitActor)
		{
			// Check if the hit actor implements the gameplay interface
			if (HitActor->Implements<UGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner); // Cast the owner to a pawn
				IGameplayInterface::Execute_Interact(HitActor, MyPawn); // Call the Interact function on the hit actor, passing the pawn as a parameter
				break; // Exit the loop after the first valid interaction
			}
		}
	}
}
