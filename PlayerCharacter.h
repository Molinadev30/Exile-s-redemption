// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UPoolSubsystem;
class USkeletalMeshComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UInteractionComponent;
class UAnimMontage;
class UNiagaraSystem;

UCLASS()
class RUHUIK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	
	// Class references
	
	UPROPERTY(EditAnywhere, Category = "Chain")
	TSubclassOf<AActor> ChainClass;
	
	UPROPERTY()
	UPoolSubsystem* PoolSubsystem;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackHoleProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;
	
	//Timer handles
	
	FTimerHandle TimerHandle_PrimaryAttack;
	
	FTimerHandle TimerHandle_RepeatAttack;
	
	FTimerHandle TimerHandle_BlackholeAttack;
	
	FTimerHandle TimerHandle_Dash;

	//Components

	UPROPERTY(EditAnywhere, Category = "Attack")
	UNiagaraSystem* MuzzleVFX;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	UInteractionComponent* InteractionComp;

	//Input Actions and Mapping Contexts
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* PrimaryAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* PrimaryInteractAction;

	//Methods
	
	void Move(const FInputActionValue& InputActionValue);
	
	void Look(const FInputActionValue& InputActionValue);
	
	void StartPrimaryAttack();
	
	void HandleRepeatingPrimaryAttack();
	
	void StopPrimaryAttack();
	
	bool CanFire() const;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);
	
	void PrimaryInteract();

	//Properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float LookSensitivity = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float UserLookSensitivity = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	bool bIsSliding = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bIsHoldingPrimaryAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float PrimaryAttackRepeatRate = 0.2f;
	
	UPROPERTY(BlueprintReadWrite, Category="Attack")
	bool bCanFireFromABP = false;
	
	UPROPERTY(BlueprintReadWrite, Category="Attack")
	bool bAutomaticMode = false;

public:
	//Constructor
	
	APlayerCharacter();

	//Inherited methods
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void PostInitializeComponents() override;
};
