
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MazeGameMode.generated.h"

UCLASS()
class MYMAZEPROJECT_API AMazeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    // Set the default player character
    AMazeGameMode();
};

