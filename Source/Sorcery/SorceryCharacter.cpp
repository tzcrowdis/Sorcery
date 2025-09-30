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
#include "GameFramework/CharacterMovementComponent.h"

#include "NiagaraFunctionLibrary.h"

#include "LaserSpell.h"

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

	// dash variables
	DashVelocity = 500.f;
	DashMaxCount = 1;
	DashCount = 0;
	bDashCooldownActive = false;

	// aim variables
	MaxAimDistance = 5000.f;

	// muzzle flash
	MuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlashEffect"));
	MuzzleFlashComp->SetupAttachment(GetMesh1P());
	if (FireSpellMuzzleFlash)
		MuzzleFlashComp->SetAsset(FireSpellMuzzleFlash);

	// laser spell
	//LaserSpellComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserSpell"));
	//LaserSpellComp->SetupAttachment(GetMesh1P());
	//LaserSpellComp->SetAsset(LaserSpellSystem);
	//LaserSpell = CreateDefaultSubobject<ALaserSpell>(TEXT("LaserSpell"));
	//LaserSpell->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::KeepRelativeTransform);
	//LaserSpell->SetActorRelativeLocation(MuzzleFlashComp->GetRelativeLocation());
}

void ASorceryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// element select collider functions
	ElementSelectCollider->OnComponentBeginOverlap.AddDynamic(this, &ASorceryCharacter::ElementSelectOverlapBegin);
	ElementSelectCollider->OnComponentEndOverlap.AddDynamic(this, &ASorceryCharacter::ElementSelectOverlapEnd);

	// element wheel selection of element
	DefaultElementScale = SMFireElement->GetRelativeScale3D();
	SelectedElementScale = 1.1f * DefaultElementScale;

	ActiveElement = EElementalType::Fire;
	UpdateActiveElementalType();
}

void ASorceryCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::Dash);

		// Default Shooting
		//EnhancedInputComponent->BindAction(ShootDefaultAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ShootDefaultSpell);

		EnhancedInputComponent->BindAction(ShootDefaultAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ShootLaserSpell);
		EnhancedInputComponent->BindAction(DestroyLaserAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::DestroyLaserSpell);

		// Rotate Element Wheel
		EnhancedInputComponent->BindAction(ElementWheelLeft, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelLeft);
		EnhancedInputComponent->BindAction(ElementWheelRight, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelRight);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
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

FVector ASorceryCharacter::Aim()
{
	FHitResult HitResult;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * MaxAimDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);

	FVector AimPoint = End;
	if (HitResult.bBlockingHit)
		AimPoint = HitResult.Location;

	return AimPoint;
}

void ASorceryCharacter::Dash()
{
	if (bDashCooldownActive)
		return;

	if (DashCount == DashMaxCount)
	{
		GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &ASorceryCharacter::ClearDashCooldown, DashCooldownTime);
		bDashCooldownActive = true;
		print("dash cooldown active");
		return;
	}

	// NOTE i wanted to dash in camera direction but struggled with vertical friction, might be better without due to backwards dash
	//FVector DashVector = FirstPersonCameraComponent->GetForwardVector(); 

	// only dash forward
	//FVector DashVector = GetActorForwardVector();

	// dash horizontally according to movement input
	FVector DashVector = GetPendingMovementInputVector();
	DashVector.Z = 0.f;
	DashVector.Normalize();
	
	LaunchCharacter(DashVector * DashVelocity, false, false);
	DashCount++;
}

void ASorceryCharacter::ClearDashCooldown()
{
	GetWorldTimerManager().ClearTimer(DashCooldownTimer);
	DashCount = 0;
	bDashCooldownActive = false;
	print("dash cooldown cleared");
}

/*
	SPELL FUNCTIONS
*/
void ASorceryCharacter::ShootDefaultSpell()
{
	if (GetController() == nullptr)
		return;

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// Spawn location
			/*
			const USkeletalMeshSocket* SpellOffsetSocket = GetMesh1P()->GetSocketByName("SpellRSocket");
			if (!SpellOffsetSocket)
				return;
			const FVector SpawnLocation = SpellOffsetSocket->GetSocketLocation(GetMesh1P());
			*/
			const FVector SpawnLocation = MuzzleFlashComp->GetComponentLocation();

			// Spawn rotation
			FVector AimPoint = Aim();
			FVector Direction = AimPoint - SpawnLocation;
			Direction.Normalize();
			const FRotator SpawnRotation = Direction.Rotation();
			
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ASorceryProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			// muzzle flash
			if (MuzzleFlashComp)
			{
				if (MuzzleFlashComp->IsActive())
					MuzzleFlashComp->Deactivate();

				MuzzleFlashComp->SetWorldRotation(SpawnRotation);
				MuzzleFlashComp->Activate();
			}
			
			// animation
			UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
			if (AnimInstance && SpellsAnimMontage)
			{
				AnimInstance->Montage_Play(SpellsAnimMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("ShootDefaultSpell"), SpellsAnimMontage);
			}
		}
	}

	/*
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	*/
}

void ASorceryCharacter::ShootLaserSpell()
{
	// spawn laser (TODO do when equipping)
	LaserSpell = GetWorld()->SpawnActor<ALaserSpell>(LaserSpellClass);
	LaserSpell->AttachToComponent(MuzzleFlashComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//FAttachmentTransformRules::ScaleRule
	LaserSpell->ChangeElementalType(ActiveElement); // TESTING

	LaserSpell->ShootLaser(FirstPersonCameraComponent);
}

void ASorceryCharacter::DestroyLaserSpell()
{
	LaserSpell->DestroyLaser();

	// destroy laser (TODO do when unequipping)
	LaserSpell->Destroy();
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

	UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
	if (AnimInstance && ElementWheelAnimMontage)
	{
		if (EWCurrentRotation <= 0)
		{
			AnimInstance->Montage_Play(ElementWheelAnimMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("ElementWheelLeft"), ElementWheelAnimMontage);
		}
		else
		{
			AnimInstance->Montage_Play(ElementWheelAnimMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("ElementWheelRight"), ElementWheelAnimMontage);
		}
	}

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
		SMFireElement->SetRelativeScale3D(SelectedElementScale);
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMIceElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Ice;
		SMIceElement->SetRelativeScale3D(SelectedElementScale);
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMShockElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Shock;
		SMShockElement->SetRelativeScale3D(SelectedElementScale);
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMAcidElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Acid;
		SMAcidElement->SetRelativeScale3D(SelectedElementScale);
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
		SMFireElement->SetRelativeScale3D(DefaultElementScale);
	}
	else if (StaticMesh == SMIceElement->GetStaticMesh())
	{
		SMIceElement->SetRelativeScale3D(DefaultElementScale);
	}
	else if (StaticMesh == SMShockElement->GetStaticMesh())
	{
		SMShockElement->SetRelativeScale3D(DefaultElementScale);
	}
	else if (StaticMesh == SMAcidElement->GetStaticMesh())
	{
		SMAcidElement->SetRelativeScale3D(DefaultElementScale);
	}
}


void ASorceryCharacter::UpdateActiveElementalType()
{
	ASorceryProjectile* projectile = Cast<ASorceryProjectile>(ProjectileClass->GetDefaultObject());
	if (projectile) projectile->ChangeElementalType(ActiveElement);

	if (LaserSpell) LaserSpell->ChangeElementalType(ActiveElement);

	switch (ActiveElement)
	{
		case EElementalType::Fire:
			MuzzleFlashComp->SetAsset(FireSpellMuzzleFlash);
			break;
		case EElementalType::Ice:
			MuzzleFlashComp->SetAsset(IceSpellMuzzleFlash);
			break;
		case EElementalType::Shock:
			MuzzleFlashComp->SetAsset(ShockSpellMuzzleFlash);
			break;
		case EElementalType::Acid:
			MuzzleFlashComp->SetAsset(AcidSpellMuzzleFlash);
			break;
	}
	MuzzleFlashComp->Deactivate();
}