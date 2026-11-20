
#include "MazeHealthPickup.h"
#include "MazeCharacter.h"

AMazeHealthPickup::AMazeHealthPickup()
{
    // Our meat pickup uses the collision and mesh
    // components already created by MazePickup
}

void AMazeHealthPickup::ApplyPickupEffect(AActor* OtherActor)
{
    // Make sure the actor collecting the meat is our player
    AMazeCharacter* Player = Cast<AMazeCharacter>(OtherActor);

    if (!Player)
    {
        return;
    }

    // Try to restore the player's health
    // Heal() makes sure we never go over max health
    float HealthRestored = Player->Heal(HealAmount);

    // If the player is already at full health,
    // leave the meat on the ground for later
    if (HealthRestored <= 0.0f)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Health pickup skipped - player doesn't need healing!"));

        return;
    }

    // Let us know the player collected the meat
    UE_LOG(LogTemp, Warning,
        TEXT("Meat collected! Restored %.0f HP"),
        HealthRestored);

    // Remove the meat after successfully healing
    Destroy();
}
