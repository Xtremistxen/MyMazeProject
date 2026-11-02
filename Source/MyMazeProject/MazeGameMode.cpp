
#include "MazeGameMode.h"
#include "MazeCharacter.h"

AMazeGameMode::AMazeGameMode()
{
    // Spawn our custom MazeCharacter when the game starts
    DefaultPawnClass = AMazeCharacter::StaticClass();
}

