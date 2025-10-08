// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SorceryPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class SORCERY_API ASorceryPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for gameplay and UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* GameInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* SkillsMenuInputMappingContext;

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;

	// End Actor interface

public:

	// UI functions
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ToggleSkillsMenu();
};
