// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define print(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT(Text), false)

#define print_k(Key, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 5.f, FColor::White, TEXT(Text))

#define printf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)

#define printf_k(Key, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 1.5f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define warn(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT(Text), false)

#define warn_k(Key, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.f, FColor::Yellow, TEXT(Text))

#define warnf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)

#define warnf_k(Key, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.f, FColor::Yellow, FString::Printf(TEXT(Format), ##__VA_ARGS__))

// NOTE this enum should probably be in its own header file
UENUM(BlueprintType)
enum class EElementalType : uint8
{
    Fire UMETA(DisplayName = "Fire"),
    Ice UMETA(DisplayName = "Ice"),
    Shock UMETA(DisplayName = "Shock"),
    Acid UMETA(DisplayName = "Acid")
};