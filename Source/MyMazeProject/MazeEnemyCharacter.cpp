
#include "MazeEnemyCharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "TimerManager.h"
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
    // Stunned enemies can't attack
    if (bIsStunned || !GetWorld())
    {
        return false;
    }

    // Check whether the attack cooldown has expired
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    return CurrentTime - LastAttackTime >= AttackCooldown;
}

void AMazeEnemyCharacter::PerformAttack()
{
    // Don't allow attacks while the enemy is stunned
    if (bIsStunned)
    {
        return;
    }

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

    // Debug whether the enemy is stunned when the hit happens
    UE_LOG(LogTemp, Warning,
        TEXT("MELEE CHECK | Enemy: %s | Stunned: %s"),
        *GetName(),
        bIsStunned ? TEXT("YES") : TEXT("NO"));


    // Don't deal damage while stunned
    if (bIsStunned)
    {
        return;
    }

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

// Check whether the enemy is stunned
bool AMazeEnemyCharacter::IsStunned() const
{
    return bIsStunned;
}

// Stun the enemy for a set amount of time
void AMazeEnemyCharacter::ApplyStun(float Duration)
{
    if (!GetWorld() || Duration <= 0.0f)
    {
        return;
    }

    // Refresh the timer if the enemy is already stunned
    GetWorldTimerManager().ClearTimer(StunTimerHandle);

    if (!bIsStunned)
    {
        bIsStunned = true;

        // Stop the current AI behavior
        if (AAIController* AIController =
            Cast<AAIController>(GetController()))
        {
            AIController->StopMovement();

            if (UBrainComponent* Brain =
                AIController->GetBrainComponent())
            {
                Brain->PauseLogic(TEXT("Enemy stunned"));
            }
        }

        // Stop the enemy from moving
        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->DisableMovement();

        // Stop any attack animation already playing
        if (UAnimInstance* AnimInstance =
            GetMesh()->GetAnimInstance())
        {
            if (AttackMontage)
            {
                // Immediately interrupt the attack animation
                AnimInstance->Montage_Stop(0.0f, AttackMontage);
            }

            // Play the stun reaction animation
            if (StunMontage)
            {
                AnimInstance->Montage_Play(StunMontage);

                // Keep repeating the stun loop until the timer expires
                AnimInstance->Montage_SetNextSection(
                    FName(TEXT("StunLoop")),
                    FName(TEXT("StunLoop")),
                    StunMontage
                );
            }
        }
    }

    // Start the timer that ends the stun
    GetWorldTimerManager().SetTimer(
        StunTimerHandle,
        this,
        &AMazeEnemyCharacter::EndStun,
        Duration,
        false);

    UE_LOG(LogTemp, Warning, TEXT("Enemy stunned for %.1f seconds"), Duration);
}

// Return the enemy to its normal AI behavior
void AMazeEnemyCharacter::EndStun()
{
    bIsStunned = false;

    // Play the recovery animation when the stun ends
    if (StunMontage && GetMesh())
    {
        if (UAnimInstance* AnimInstance =
            GetMesh()->GetAnimInstance())
        {
            if (AnimInstance->Montage_IsPlaying(StunMontage))
            {
                // Stop looping and transition into recovery
                AnimInstance->Montage_SetNextSection(
                    FName(TEXT("StunLoop")),
                    FName(TEXT("StunEnd")),
                    StunMontage
                );

                // Jump directly to the recovery animation
                AnimInstance->Montage_JumpToSection(
                    FName(TEXT("StunEnd")),
                    StunMontage
                );
            }
        }
    }

    // Allow movement again
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // Resume our Behavior Tree
    if (AAIController* AIController =
        Cast<AAIController>(GetController()))
    {
        if (UBrainComponent* Brain =
            AIController->GetBrainComponent())
        {
            Brain->ResumeLogic(TEXT("Stun ended"));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Enemy recovered from stun"));
}



