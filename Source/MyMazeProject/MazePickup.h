

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazePickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS(Abstract)
class MYMAZEPROJECT_API AMazePickup : public AActor
{
    GENERATED_BODY()

public:
    // Set up the basic pickup actor
    AMazePickup();


    // Every pickup needs its own effect
    // Meat heals, bread boosts speed, etc.
    virtual void ApplyPickupEffect(AActor* OtherActor)
        PURE_VIRTUAL(AMazePickup::ApplyPickupEffect, );


protected:
    // Runs when the pickup first appears
    virtual void BeginPlay() override;

    // Detects when something touches our pickup
    UFUNCTION()
    void OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // Sphere that checks if the player is close enough
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    USphereComponent* PickupCollision;

    // Mesh that displays the food item
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* PickupMesh;
};

