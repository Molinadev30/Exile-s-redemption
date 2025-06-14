// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PoolSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	//Ignore all chain actors while moving
	
	for (TActorIterator<AActor> It(GetWorld(), ChainClass); It; ++It)
	{
		AActor* Actor = *It;
		MoveIgnoreActorAdd(Actor);
	}
}

APlayerCharacter::APlayerCharacter()
{
	//Components
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(GetMesh(), "head");
	
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UInteractionComponent>("InteractionComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = true;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Get a reference to the player controller
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	// Add Input Mapping Context
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	// Set up action bindings
	
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Movement
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		//Primary Attack
		
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::StartPrimaryAttack);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopPrimaryAttack);
		
		//Primary Interact
		
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &APlayerCharacter::PrimaryInteract);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Failed to find an Enhanced Input component!"));
	}
}

void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	//Hide head and neck bones for FPS view
	Super::OnConstruction(Transform);
	if (GetMesh())
	{
		GetMesh()->HideBoneByName("head", EPhysBodyOp::PBO_None);
        GetMesh()->HideBoneByName("neck_01", EPhysBodyOp::PBO_None);
        GetMesh()->HideBoneByName("neck_02", EPhysBodyOp::PBO_None);
	}

	//Get Pool Subsystem
	PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>();
}

void APlayerCharacter::Move(const FInputActionValue& InputActionValue)
 {
	// Get the control rotation and set its pitch and roll to zero to avoid unwanted movements

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	ControlRot.Roll = 0;

	// Get the input value as a 2D vector

	const FVector2D AxisValue = InputActionValue.Get<FVector2D>();

	// Get the forward vector from the control rotation
	
	const FVector ForwardVector = ControlRot.Vector();
	
	AddMovementInput(ForwardVector, AxisValue.Y);	// Move forward and backward
	
	// Get the right vector from the control rotation
	
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
    AddMovementInput(RightVector, AxisValue.X);	//Move right and left
 }

void APlayerCharacter::Look(const FInputActionValue& InputActionValue)
{
	// Get the input value as a 2D vector
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	//Apply the input to the controller's yaw and pitch
	
	AddControllerYawInput(Value.X * LookSensitivity * UserLookSensitivity);
	AddControllerPitchInput(Value.Y * LookSensitivity * UserLookSensitivity);
}

void APlayerCharacter::StartPrimaryAttack()
{
	//If the player is already holding the attack, don't do anything
	
	if (bIsHoldingPrimaryAttack) return;
	
	bIsHoldingPrimaryAttack = true; // Set the flag to indicate that the player is holding the primary attack
	
	// If the player can fire, spawn a projectile immediately
	
	if (CanFire())
	{
		SpawnProjectile(ProjectileClass);
	}

	// Set the timer to handle repeating primary attacks, this does not happen in single shot mode
	
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_PrimaryAttack))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &APlayerCharacter::HandleRepeatingPrimaryAttack, PrimaryAttackRepeatRate, true, 0.0f);
	}
}

void APlayerCharacter::HandleRepeatingPrimaryAttack()
{
	//If the player con fire, spawn a projectile when timer calls this function
	
	if (CanFire())
	{
		SpawnProjectile(ProjectileClass);
	}
}

void APlayerCharacter::StopPrimaryAttack()
{
	// If the player is not holding the primary attack, do nothing
	
	if (!bIsHoldingPrimaryAttack) return;

	// If the player is in automatic mode, we set the variable to false so next time player enters StartPrimaryAttack(), it will be able to fire again.
	
	if (bAutomaticMode)	bIsHoldingPrimaryAttack = false;
	
	GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);	//Clear the timer for repeating primary attacks
}

bool APlayerCharacter::CanFire() const
{
	// Check if the player is holding the primary attack and if the animation blueprint allows firing
	
	return bIsHoldingPrimaryAttack && bCanFireFromABP;
}

void APlayerCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	// Ensure the class to spawn is valid and we have a reference to the pool subsystem
	
	if (!ensureAlways(ClassToSpawn && PoolSubsystem)) return;
	
	FVector HandLocation = GetMesh()->GetBoneLocation("hand_r"); //Get the hand location for the projectile spawn

	// Set up spawn parameters for the projectile
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	
	FHitResult Hit;	//Create a hit result to store the trace result
	
	FVector TraceStart = CameraComp->GetComponentLocation();    // Start point of the trace, camera location
	FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);	//Endpoint of the trace, 5000 units away from the camera
	
	FCollisionQueryParams Params;	// Create a collision query parameters object
	
	Params.AddIgnoredActor(this);	// Ignore the player character
	
	FRotator ProjRotation = FRotator::ZeroRotator;	// Initialize the projectile rotation to zero

	// Perform a line trace to find the desired impact point and adjust the projectile rotation accordingly
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel1, Params))
	{
		// Calculate the direction vector from the hand location to the hit location
		
		FVector VectorToHand = (Hit.ImpactPoint - HandLocation);
		
		// Adjust rotation to face the hit point
		
		ProjRotation = FRotationMatrix::MakeFromX(VectorToHand).Rotator();
	}
	else
	{
		// If no hit was detected, set the projectile rotation to face the trace end point
		
		ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
	}

	//Spawn muzzle VFX
	
	if (ensure(IsValid(this) && MuzzleVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		MuzzleVFX,
		GetMesh()->GetSocketLocation("Muzzle_01"),
		ProjRotation,
		FVector(0.01),
		true,
		true,
		ENCPoolMethod::AutoRelease,
		true
		);
	}
	
	// Spawn the projectile from the pool subsystem
	
	AActor* SpawnedActor = nullptr;
	
	PoolSubsystem->SpawnFromPool(ClassToSpawn, HandLocation, ProjRotation, this, SpawnedActor);
}

void APlayerCharacter::PrimaryInteract()
{
	// Ensure the interaction component is valid before calling the interaction method
	
	if (ensure(InteractionComp))
	{
		InteractionComp->PrimaryInteract();
	}
}
