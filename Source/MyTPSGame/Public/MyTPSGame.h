// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) TEXT("("))

#define PRINT_CALLINFO() UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO)

#define PRINT_LOG(fmt, ...) UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO, *FString::printf(fmt, ##__VA_ARGS__))