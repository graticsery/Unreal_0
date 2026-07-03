// Copyright Epic Games, Inc. All Rights Reserved.

#include "PP_1GameMode.h"
#include "PP_1Character.h"
#include "UObject/ConstructorHelpers.h"

APP_1GameMode::APP_1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
