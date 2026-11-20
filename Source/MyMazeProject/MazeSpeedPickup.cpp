
#include "MazeSpeedPickup.h"
#include "MazeCharacter.h"

// Set up our bread pickup
AMazeSpeedPickup::AMazeSpeedPickup()
{
    // The collision and mesh are already handled by MazePickup
}

// Give the player a speed boost when they collect bread
void AMazeSpeedPickup::ApplyPickupEffect(AActor* OtherActor)
{
    // Make sure the actor collecting bread is our player
    AMazeCharacter* Player = Cast<AMazeCharacter>(OtherActor);

    if (!Player)
    {
        return;
    }

    // Give the player a temporary movement speed boost
    Player->ApplySpeedBoost(SpeedMultiplier, BoostDuration);

    UE_LOG(LogTemp, Warning,
        TEXT("Bread collected! Speed boosted for %.1f seconds"),
        BoostDuration);

    // Remove the bread after collecting it
    Destroy();
}

