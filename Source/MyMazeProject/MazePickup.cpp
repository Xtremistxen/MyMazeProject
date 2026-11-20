
#include "MazePickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

AMazePickup::AMazePickup()
{
    // Pickups don't need to update every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create a sphere to detect when the player touches food
    PickupCollision = CreateDefaultSubobject<USphereComponent>(
        TEXT("PickupCollision"));

    RootComponent = PickupCollision;

    // Set how close the player needs to be
    PickupCollision->InitSphereRadius(75.0f);

    // Only detect pawn overlaps
    PickupCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    PickupCollision->SetCollisionResponseToAllChannels(
        ECR_Ignore);

    PickupCollision->SetCollisionResponseToChannel(
        ECC_Pawn, ECR_Overlap);

    // Create the mesh that displays our Synty food
    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("PickupMesh"));

    PickupMesh->SetupAttachment(RootComponent);

    // The food mesh is visual only
    // The sphere handles all collision detection
    PickupMesh->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);
}

void AMazePickup::BeginPlay()
{
    Super::BeginPlay();

    // Listen for the player touching our collision sphere
    PickupCollision->OnComponentBeginOverlap.AddDynamic(
        this, &AMazePickup::OnPickupOverlap);
}

void AMazePickup::OnPickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // Ignore invalid actors and ourselves
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    // Only allow the player to collect food
    APawn* OverlappingPawn = Cast<APawn>(OtherActor);

    if (!OverlappingPawn || !OverlappingPawn->IsPlayerControlled())
    {
        return;
    }

    // Let the child pickup decide what happens
    // Meat heals, bread boosts speed
    ApplyPickupEffect(OtherActor);
}

void AMazePickup::ApplyPickupEffect(AActor* OtherActor)
{
    // Child pickups should override this function
    UE_LOG(LogTemp, Warning,
        TEXT("Pickup effect not implemented in child class!"));
}


