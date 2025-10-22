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

#include "SorceryPlayerController.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Sorcery.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "NiagaraFunctionLibrary.h"

#include "ElementalBallSpell.h"
#include "LaserSpell.h"
#include "LobSpell.h"

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

	ActiveElementScale = 1.25f;

	// skills / stats
	MaxHealth = 100.f;
	Health = MaxHealth;
	DamagePercent = 1.f;
	AttackSpeedPercent = 1.f;

	// dash variables
	DashVelocity = 500.f;
	DashMaxCount = 1;
	DashCount = 0;
	bDashCooldownActive = false;
	bDashResetTimerActive = false;

	// origin point for spells
	SpellAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpellAttachPoint"));
	SpellAttachPoint->SetupAttachment(GetMesh1P());

	// souls
	SoulsHeld = 0;
	SoulsUpgradeCost = 5;
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
	SelectedElementScale = ActiveElementScale * DefaultElementScale;

	ActiveElement = EElementalType::Fire;
	UpdateActiveElementalType();

	ActiveSpell = ESpellEquipped::ElementalBall; // NOTE default should be elemental ball
	EquipSpell(ActiveSpell);
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

		// Shooting
		EnhancedInputComponent->BindAction(ShootPressedAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ShootSpell);
		EnhancedInputComponent->BindAction(ShootReleasedAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ReleaseSpell);

		// Rotate Element Wheel
		EnhancedInputComponent->BindAction(ElementWheelLeft, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelLeft);
		EnhancedInputComponent->BindAction(ElementWheelRight, ETriggerEvent::Triggered, this, &ASorceryCharacter::QueueElementWheelRight);
		
		// Equip spells
		EnhancedInputComponent->BindAction(EquipElementalBall, ETriggerEvent::Triggered, this, &ASorceryCharacter::EquipSpell, ESpellEquipped::ElementalBall);
		EnhancedInputComponent->BindAction(EquipLaser, ETriggerEvent::Triggered, this, &ASorceryCharacter::EquipSpell, ESpellEquipped::Laser);
		EnhancedInputComponent->BindAction(EquipLob, ETriggerEvent::Triggered, this, &ASorceryCharacter::EquipSpell, ESpellEquipped::Lob);
	
		// Open Skills Menu
		EnhancedInputComponent->BindAction(SkillsMenuAction, ETriggerEvent::Triggered, this, &ASorceryCharacter::ToggleSkillsMenu);
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

void ASorceryCharacter::Dash()
{
	if (bDashCooldownActive)
		return;

	// dash horizontally according to movement input
	FVector DashVector = GetPendingMovementInputVector();
	DashVector.Z = 0.f;
	DashVector.Normalize();
	LaunchCharacter(DashVector * DashVelocity, false, false);
	DashCount++;

	// soft timer to reset dash count when player doesnt hit max dash count
	if (DashCount > 0 && DashCount < DashMaxCount)
	{
		if (bDashResetTimerActive)
			GetWorldTimerManager().ClearTimer(DashResetTimer);

		GetWorldTimerManager().SetTimer(DashResetTimer, this, &ASorceryCharacter::ResetDashCount, DashResetTime);
	}

	// cooldown if you've reached the max consecutive dashes
	if (DashCount == DashMaxCount)
	{
		GetWorldTimerManager().ClearTimer(DashResetTimer);
		bDashResetTimerActive = false;

		GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &ASorceryCharacter::ClearDashCooldown, DashCooldownTime);
		bDashCooldownActive = true;
		print("dash cooldown active");
	}
}

void ASorceryCharacter::ResetDashCount()
{
	GetWorldTimerManager().ClearTimer(DashResetTimer);
	bDashResetTimerActive = false;
	DashCount = 0;
}

void ASorceryCharacter::ClearDashCooldown()
{
	GetWorldTimerManager().ClearTimer(DashCooldownTimer);
	DashCount = 0;
	bDashCooldownActive = false;
	print("dash cooldown cleared");
}

void ASorceryCharacter::ToggleSkillsMenu()
{
	// TODO consider getting player controller reference from the start
	AController* controller = GetController();
	ASorceryPlayerController* SorceryController = Cast<ASorceryPlayerController>(controller);
	SorceryController->ToggleSkillsMenu();
	
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			ElementalBallSpell->UpdateDamage(DamagePercent);
			ElementalBallSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;

		case ESpellEquipped::Laser:
			LaserSpell->UpdateDamage(DamagePercent);
			LaserSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;

		case ESpellEquipped::Lob:
			LobSpell->UpdateDamage(DamagePercent);
			LobSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;
	}
}

/*
	HEALTH & PROGRESSION
*/

float ASorceryCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		// TODO die
	}
	else
	{
		Health -= DamageAmount;
	}

	UpdateHealthBar();

	return DamageAmount;
}

void ASorceryCharacter::GatherSouls(int Amount)
{
	SoulsHeld += Amount;
}

bool ASorceryCharacter::SpendSoulsForUpgrade()
{
	if (SoulsHeld - SoulsUpgradeCost < 0)
		return false;
	else
	{
		SoulsHeld -= SoulsUpgradeCost;
		return true;
	}
}

bool ASorceryCharacter::CheckSoulsForUpgrade()
{
	if (SoulsHeld - SoulsUpgradeCost < 0)
		return false;
	else
		return true;
}

/*
	SPELL FUNCTIONS
*/
void ASorceryCharacter::EquipSpell(ESpellEquipped NewSpell)
{
	// remove old spell
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			if (ElementalBallSpell) ElementalBallSpell->Destroy();
			break;

		case ESpellEquipped::Laser:
			if (LaserSpell) LaserSpell->Destroy();
			break;

		case ESpellEquipped::Lob:
			if (LobSpell) LobSpell->Destroy();
			break;
	}
	
	// update active spell
	ActiveSpell = NewSpell;

	// equip new spell
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			ElementalBallSpell = GetWorld()->SpawnActor<AElementalBallSpell>(ElementalBallSpellClass);
			ElementalBallSpell->AttachToComponent(SpellAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ElementalBallSpell->ChangeElementalType(ActiveElement);
			ElementalBallSpell->UpdateReticle();
			ElementalBallSpell->UpdateDamage(DamagePercent);
			ElementalBallSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;

		case ESpellEquipped::Laser:
			LaserSpell = GetWorld()->SpawnActor<ALaserSpell>(LaserSpellClass);
			LaserSpell->AttachToComponent(SpellAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			LaserSpell->ChangeElementalType(ActiveElement);
			LaserSpell->UpdateReticle();
			LaserSpell->UpdateDamage(DamagePercent);
			LaserSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;

		case ESpellEquipped::Lob:
			LobSpell = GetWorld()->SpawnActor<ALobSpell>(LobSpellClass);
			LobSpell->AttachToComponent(SpellAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			LobSpell->ChangeElementalType(ActiveElement);
			LobSpell->UpdateReticle();
			LobSpell->UpdateDamage(DamagePercent);
			LobSpell->UpdateAttackSpeed(AttackSpeedPercent);
			break;
	}
}

void ASorceryCharacter::ShootSpell()
{
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			ElementalBallSpell->ShootElementalBall(this);
			break;

		case ESpellEquipped::Laser:
			LaserSpell->ShootLaser(FirstPersonCameraComponent);
			break;

		case ESpellEquipped::Lob:
			LobSpell->ShootLobSpell(this);
			break;
	}

	UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
	if (AnimInstance && SpellsAnimMontage)
	{
		AnimInstance->Montage_Play(SpellsAnimMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("ShootDefaultSpell"), SpellsAnimMontage);
	}
}

void ASorceryCharacter::ReleaseSpell()
{
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			// no action necessary
			break;

		case ESpellEquipped::Laser:
			LaserSpell->DeactivateLaser();
			break;

		case ESpellEquipped::Lob:
			// no action necessary
			break;
	}
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
	switch (ActiveSpell)
	{
		case ESpellEquipped::ElementalBall:
			if (ElementalBallSpell) ElementalBallSpell->ChangeElementalType(ActiveElement);
			break;

		case ESpellEquipped::Laser:
			if (LaserSpell) LaserSpell->ChangeElementalType(ActiveElement);
			break;

		case ESpellEquipped::Lob:
			if (LobSpell) LobSpell->ChangeElementalType(ActiveElement);
			break;
	}
}

