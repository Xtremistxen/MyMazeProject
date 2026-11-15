
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MazeAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;

UCLASS()
class MYMAZEPROJECT_API AMazeAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMazeAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    // Called when the enemy sees or loses a target
    UFUNCTION()
    void OnTargetPerceptionUpdated(
        AActor* Actor,
        FAIStimulus Stimulus);

    // Behavior Tree assigned in the editor
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // Detects nearby actors using AI senses
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UAIPerceptionComponent* EnemyPerception;

    // Configures the enemy's vision
    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;
};

