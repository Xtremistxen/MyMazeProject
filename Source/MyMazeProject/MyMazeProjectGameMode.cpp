// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyMazeProjectGameMode.h"
#include "MyMazeProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyMazeProjectGameMode::AMyMazeProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
