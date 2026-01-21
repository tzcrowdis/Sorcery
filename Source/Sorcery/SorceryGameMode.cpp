// Copyright Epic Games, Inc. All Rights Reserved.

#include "SorceryGameMode.h"
#include "SorceryCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASorceryGameMode::ASorceryGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Sorcery/Blueprints/BP_FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

}
