// Copyright Epic Games, Inc. All Rights Reserved.

#include "SorceryCharacter.h"
#include "SorceryProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Sorcery.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASorceryCharacter

ASorceryCharacter::ASorceryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// create the element wheel component
	ElementWheel = CreateDefaultSubobject<USceneComponent>(TEXT("ElementWheel"));
	ElementWheel->SetupAttachment(GetMesh1P());

	SMFireElement = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireElement"));
	SMFireElement->SetupAttachment(ElementWheel);

	SMShockElement = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShockElement"));
	SMShockElement->SetupAttachment(ElementWheel);

	SMIceElement = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IceElement"));
	SMIceElement->SetupAttachment(ElementWheel);

	SMAcidElement = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AcidElement"));
	SMAcidElement->SetupAttachment(ElementWheel);

	// element wheel rotation values
	EWLeftRotationValue = -90;
	EWRightRotationValue = 90;
	EWCurrentRotation = 0;
	EWPreviousRotation = FRotator(0.f, 0.f, 0.f);

	ElementSelectCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ElementSelectCollider"));
	ElementSelectCollider->SetupAttachment(GetMesh1P());
	ElementSelectCollider->InitSphereRadius(1.f);
}

void ASorceryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// element select collider functions
	ElementSelectCollider->OnComponentBeginOverlap.AddDynamic(this, &ASorceryCharacter::ElementSelectOverlapBegin);
	ElementSelectCollider->OnComponentEndOverlap.AddDynamic(this, &ASorceryCharacter::ElementSelectOverlapEnd);

	// element wheel selection of element
	ActiveElement = EElementalType::Fire;
	UpdateActiveElementalType();
}

//////////////////////////////////////////////////////////////////////////// Input

void ASorceryCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::Look);

		// Default Shooting
		EnhancedInputComponent->BindAction(ShootDefaultAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ShootDefaultSpell);

		// Rotate Element Wheel
		EnhancedInputComponent->BindAction(ElementWheelLeft, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelLeft);
		EnhancedInputComponent->BindAction(ElementWheelRight, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelRight);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ASorceryCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASorceryCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

/*
	SPELL FUNCTIONS
*/
void ASorceryCharacter::ShootDefaultSpell()
{
	if (GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			
			const USkeletalMeshSocket* SpellOffsetSocket = GetMesh1P()->GetSocketByName("SpellRSocket");
			if (!SpellOffsetSocket)
				return;
			const FVector SpawnLocation = SpellOffsetSocket->GetSocketLocation(GetMesh1P());
			
			// SpellOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			//const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(SpellOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ASorceryProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	/*
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	*/
}

/*
	ELEMENT WHEEL FUNCTIONS	
*/
void ASorceryCharacter::QueueElementWheelLeft()
{
	int32* peek = EWRotationQueue.Peek();

	if (peek != nullptr && *peek == EWLeftRotationValue)
	{
		EWRotationQueue.Enqueue(EWLeftRotationValue);
	}
	else
	{
		EWRotationQueue.Empty();
		EWRotationQueue.Enqueue(EWLeftRotationValue);
		RotateElementWheel();
	}
}

void ASorceryCharacter::QueueElementWheelRight()
{
	int32* peek = EWRotationQueue.Peek();
	if (peek != nullptr && *peek == EWRightRotationValue)
	{
		EWRotationQueue.Enqueue(EWRightRotationValue);
	}
	else
	{
		EWRotationQueue.Empty();
		EWRotationQueue.Enqueue(EWRightRotationValue);
		RotateElementWheel();
	}
}

bool ASorceryCharacter::ProcessElementWheelQueue()
{
	if (EWRotationQueue.IsEmpty())
		return false;

	EWRotationQueue.Dequeue(EWCurrentRotation);
	EWStartRotation = ElementWheel->GetRelativeRotation();
	EWPreviousRotation = EWStartRotation;
	return true;
}

void ASorceryCharacter::UpdateElementWheelRotation(float NormalizedRotation)
{
	FRotator Rotation = EWStartRotation;
	Rotation.Pitch = FMath::Lerp(EWStartRotation.Pitch, EWStartRotation.Pitch + EWCurrentRotation, NormalizedRotation);
	ElementWheel->AddLocalRotation(Rotation - EWPreviousRotation);
	EWPreviousRotation = Rotation;
}

void ASorceryCharacter::ElementSelectOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// NOTE dirty implementation, dependent on static mesh, which will likely be switched out
	if (!OtherComp->ComponentHasTag(TEXT("ElementOrb")))
		return;

	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(OtherComp);
	if (!StaticMeshComp)
		return;

	UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();
	if (StaticMesh == SMFireElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Fire;
		SMFireElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMIceElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Ice;
		SMIceElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMShockElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Shock;
		SMShockElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMAcidElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Acid;
		SMAcidElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
}

void ASorceryCharacter::ElementSelectOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// NOTE dirty implementation, dependent on static mesh, which will likely be switched out
	if (!OtherComp->ComponentHasTag(TEXT("ElementOrb")))
		return;

	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(OtherComp);
	if (!StaticMeshComp)
		return;

	UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();
	if (StaticMesh == SMFireElement->GetStaticMesh())
	{
		SMFireElement->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));
	}
	else if (StaticMesh == SMIceElement->GetStaticMesh())
	{
		SMIceElement->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));
	}
	else if (StaticMesh == SMShockElement->GetStaticMesh())
	{
		SMShockElement->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));
	}
	else if (StaticMesh == SMAcidElement->GetStaticMesh())
	{
		SMAcidElement->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));
	}
}


void ASorceryCharacter::UpdateActiveElementalType()
{
	ASorceryProjectile* projectile = Cast<ASorceryProjectile>(ProjectileClass->GetDefaultObject());
	if (projectile) projectile->ChangeElementalType(ActiveElement);
}