
#include "MazeEnemyCharacter.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Controller.h"


AMazeEnemyCharacter::AMazeEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Let the AI rotate the enemy toward its movement
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 400.0f, 0.0f);

    // Enemy walking speed
    GetCharacterMovement()->MaxWalkSpeed = 250.0f;
}

void AMazeEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

bool AMazeEnemyCharacter::CanAttack() const
{
    if (!GetWorld())
    {
        return false;
    }

    // Check whether the attack cooldown has expired
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    return CurrentTime - LastAttackTime >= AttackCooldown;
}

void AMazeEnemyCharacter::PerformAttack()
{
    if (!CanAttack() || !AttackMontage)
    {
        return;
    }

    // Get the enemy's animation instance
    UAnimInstance* AnimInstance =
        GetMesh()->GetAnimInstance();

    if (!AnimInstance)
    {
        return;
    }

    // Play the melee attack animation
    const float Duration =
        AnimInstance->Montage_Play(AttackMontage);


    if (Duration > 0.0f)
    {
        // Begin a new attack and allow one successful hit
        bHasHitThisAttack = false;

        // Record when the attack started
        LastAttackTime = GetWorld()->GetTimeSeconds();
    }



}

void AMazeEnemyCharacter::ApplyMeleeDamage()
{
    // Only one successful hit per attack
    if (bHasHitThisAttack)
    {
        return;
    }

    // Find the player character
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (!PlayerPawn)
    {
        return;
    }

    // Check whether the player is within melee range
    const float Distance = FVector::Dist(
        GetActorLocation(),
        PlayerPawn->GetActorLocation());

    if (Distance > AttackRange)
    {
        return;
    }

    // Prevent another hit during this attack
    bHasHitThisAttack = true;

    // Apply damage through Unreal's damage system
    UGameplayStatics::ApplyDamage(
        PlayerPawn,
        AttackDamage,
        GetController(),
        this,
        UDamageType::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("Enemy hit player for %.0f damage"),
        AttackDamage);
}



