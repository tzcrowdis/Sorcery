// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Sorcery.h"
#include "SorceryCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class USphereComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASorceryCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Shooting Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootPressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootReleasedAction;

	/** Skills Menu Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SkillsMenuAction;

	/* Equipping Spell Inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipElementalBall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipLaser;

	/** Rotate Element Wheel Left Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelLeft;

	/** Rotate Element Wheel Right Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelRight;

	/* Animation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* SpellsAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ElementWheelAnimMontage;

	/* Spell Actors */
	class AElementalBallSpell* ElementalBallSpell;
	class ALaserSpell* LaserSpell;

protected:
	/* Dash */
	int DashCount;
	FTimerHandle DashCooldownTimer;
	bool bDashCooldownActive;
	FTimerHandle DashResetTimer;
	bool bDashResetTimerActive;

	/* Element Wheel Rotation Vars */
	TQueue<int32> EWRotationQueue;
	int32 EWCurrentRotation;
	FRotator EWStartRotation;
	FRotator EWPreviousRotation;
	int32 EWLeftRotationValue;
	int32 EWRightRotationValue;
	FVector DefaultElementScale;
	FVector SelectedElementScale;

	/* Element Wheel Type */
	EElementalType ActiveElement;

	/* Equipped Spell */
	ESpellEquipped ActiveSpell;

	/* Souls and Leveling */
	//float LevelCap;

public:
	/* Souls and Leveling */
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progression")
	//int Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int SoulsUpgradeCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int SoulsHeld;

	// health
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	
	/* Modifiable Skills/Stats */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float DamagePercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float AttackSpeedPercent;

	// move speed handled by character movement
	// jump height, count, etc. handled by character movement

	/** Dash Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	int DashMaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashCooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashResetTime;

	
	/** Spell Classes and Variables */
	UPROPERTY(EditAnywhere, Category = "Spells")
	USceneComponent* SpellAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Spells")
	TSubclassOf<class AElementalBallSpell> ElementalBallSpellClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spells")
	TSubclassOf<class ALaserSpell> LaserSpellClass;

	/* Element Wheel */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	USceneComponent* ElementWheel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* SMFireElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* SMShockElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* SMIceElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* SMAcidElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	USphereComponent* ElementSelectCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	float ActiveElementScale;

protected:
	virtual void BeginPlay();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/* Called when switching between spells */
	void EquipSpell(ESpellEquipped NewSpell);

	/* toggles skills menu */
	void ToggleSkillsMenu();

	/** Called for dash input */
	void Dash();
	void ClearDashCooldown();
	void ResetDashCount();

	/* Element Wheel Input Queue Functions */
	void QueueElementWheelLeft();
	void QueueElementWheelRight();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	ASorceryCharacter();

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;
	
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float TakeDamage
	(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void UpdateHealthBar();

	/** Functions to handle shooting spells */
	UFUNCTION(BlueprintCallable, Category = "Spells")
	void ShootSpell();

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void ReleaseSpell();

	/** Element Wheel Timeline Functions */
	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	bool ProcessElementWheelQueue();

	UFUNCTION(BlueprintImplementableEvent, Category = "Element Wheel")
	void RotateElementWheel(); // event in blueprint

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	void UpdateElementWheelRotation(float Rotation);

	/* Selecting Element from Wheel */
	UFUNCTION()
	void ElementSelectOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ElementSelectOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	void UpdateActiveElementalType();

	/* Souls and Leveling */
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void GatherSouls(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool SpendSoulsForUpgrade();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool CheckSoulsForUpgrade();
};

