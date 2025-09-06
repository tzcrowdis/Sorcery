// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SorceryCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

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

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Shoot Default Projectile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootDefaultAction;

	/** Shoot Default Projectile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelLeft;

	/** Shoot Default Projectile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelRight;
	
public:
	ASorceryCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Default Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> ProjectileClass;

	/** Spell offset from the characters location */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(MakeEditWidget = true))
	//FVector SpellOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	USceneComponent* ElementWheel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* FireElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* ShockElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* IceElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	UStaticMeshComponent* AcidElement;

protected:
	TQueue<int32> EWRotationQueue;
	int32 EWCurrentRotation;
	FRotator EWStartRotation;
	FRotator EWPreviousRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	int32 EWLeftRotationValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Wheel")
	int32 EWRightRotationValue;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Cast spell to shoot the Default Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ShootDefaultSpell();

	/* Element Wheel Input Queue Functions */
	void QueueElementWheelLeft();
	void QueueElementWheelRight();

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	bool ProcessElementWheelQueue();

	/** Element Wheel Timeline Functions */
	UFUNCTION(BlueprintImplementableEvent, Category = "Element Wheel")
	void RotateElementWheel();

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	void UpdateElementWheelRotation(float Rotation);
};

