
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MazeEnemyCharacter.generated.h"


class UAnimMontage;

UCLASS()
class MYMAZEPROJECT_API AMazeEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMazeEnemyCharacter();

    // Distance at which the enemy can attack the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 150.0f;

    // Amount of damage dealt by a melee attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage = 20.0f;

    // Minimum time between attacks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown = 1.5f;

    // Animation played when the enemy attacks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage = nullptr;

    // Returns true when the enemy is allowed to attack
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool CanAttack() const;

    // Starts a melee attack animation
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformAttack();

    // Called by the attack animation's hit notification
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyMeleeDamage();

    // ===== Enemy Stun System =====

// Tracks whether this enemy is currently stunned
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stun")
    bool bIsStunned = false;

    // Animation played while the enemy is stunned
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
    UAnimMontage* StunMontage = nullptr;

    // Stun this enemy for a set amount of time
    UFUNCTION(BlueprintCallable, Category = "Stun")
    void ApplyStun(float Duration);

    // Check if the enemy is stunned
    UFUNCTION(BlueprintCallable, Category = "Stun")
    bool IsStunned() const;


protected:
    virtual void BeginPlay() override;

    // Called when the stun duration ends
    void EndStun();

    // Timer that controls how long the enemy stays stunned
    FTimerHandle StunTimerHandle;

private:
    // Game time when the enemy last attacked
    float LastAttackTime = -1000.0f;

    // Prevent multiple hits during one attack animation
    bool bHasHitThisAttack = false;

};

