// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
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

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Shoot Default Projectile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootDefaultAction;

	/** Rotate Element Wheel Left Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelLeft;

	/** Rotate Element Wheel Right Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ElementWheelRight;
	
public:
	ASorceryCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Default Projectile classes to spawn for each element */
	TSubclassOf<class ASorceryProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> DefaultProjectile_Fire;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> DefaultProjectile_Ice;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> DefaultProjectile_Shock;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> DefaultProjectile_Acid;

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

protected:
	/* Element Wheel Rotation Vars */
	TQueue<int32> EWRotationQueue;
	int32 EWCurrentRotation;
	FRotator EWStartRotation;
	FRotator EWPreviousRotation;
	int32 EWLeftRotationValue;
	int32 EWRightRotationValue;

	/* Element Wheel Type */
	EElementalType ActiveElement;

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

	/** Element Wheel Timeline Functions */
	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	bool ProcessElementWheelQueue();

	UFUNCTION(BlueprintImplementableEvent, Category = "Element Wheel")
	void RotateElementWheel();

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	void UpdateElementWheelRotation(float Rotation);

	/* Selecting Element from Wheel */
	UFUNCTION()
	void ElementSelectOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ElementSelectOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Element Wheel")
	void UpdateActiveElementalType();
};

