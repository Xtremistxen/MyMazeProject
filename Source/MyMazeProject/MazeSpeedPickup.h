
#pragma once

#include "CoreMinimal.h"
#include "MazePickup.h"
#include "MazeSpeedPickup.generated.h"

UCLASS()
class MYMAZEPROJECT_API AMazeSpeedPickup : public AMazePickup
{
    GENERATED_BODY()

public:
    // Set up our bread speed pickup
    AMazeSpeedPickup();

    // Give the player a speed boost when they grab bread
    virtual void ApplyPickupEffect(AActor* OtherActor) override;

protected:
    // How much faster the player moves
    // 1.5 means 50% faster than normal
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float SpeedMultiplier = 1.5f;

    // How long the speed boost lasts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float BoostDuration = 5.0f;
};

