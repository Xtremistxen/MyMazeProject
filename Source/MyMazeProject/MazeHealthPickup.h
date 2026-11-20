
#pragma once

#include "CoreMinimal.h"
#include "MazePickup.h"
#include "MazeHealthPickup.generated.h"

UCLASS()
class MYMAZEPROJECT_API AMazeHealthPickup : public AMazePickup
{
    GENERATED_BODY()

public:
    // Set up our meat health pickup
    AMazeHealthPickup();

    // This is the health pickup's version of the
    // abstract function from our parent class
    virtual void ApplyPickupEffect(AActor* OtherActor) override;

protected:
    // How much health the meat restores
    // Can be changed in the Blueprint editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float HealAmount = 25.0f;
};

