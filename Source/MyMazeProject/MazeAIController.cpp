

#include "MazeAIController.h"

#include "MazeCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

AMazeAIController::AMazeAIController()
{
    // Set up perception so the enemy can detect the player
    EnemyPerception =
        CreateDefaultSubobject<UAIPerceptionComponent>(
            TEXT("EnemyPerception"));

    SetPerceptionComponent(*EnemyPerception);

    // Set up how far the enemy can see
    SightConfig =
        CreateDefaultSubobject<UAISenseConfig_Sight>(
            TEXT("SightConfig"));

    // Enemy can spot the player from 1200 units away
    SightConfig->SightRadius = 1200.0f;

    // Enemy loses sight after the player gets far enough away
    SightConfig->LoseSightRadius = 1500.0f;

    // Controls how wide the enemy's vision is
    SightConfig->PeripheralVisionAngleDegrees = 70.0f;

    // Allow the enemy to detect different types of actors
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    // Give our perception component the sight settings
    EnemyPerception->ConfigureSense(*SightConfig);

    // Make sight the main sense the enemy uses
    EnemyPerception->SetDominantSense(
        UAISense_Sight::StaticClass());

    // Call our function whenever the enemy sees or loses something
    EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AMazeAIController::OnTargetPerceptionUpdated);
}

void AMazeAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Start the enemy's behavior tree when it gets controlled
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AMazeAIController::OnTargetPerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus)
{
    // Make sure we are detecting the player and not something else
    if (!Cast<AMazeCharacter>(Actor))
    {
        return;
    }

    // Get the blackboard so we can update the enemy's target
    UBlackboardComponent* BlackboardComp =
        GetBlackboardComponent();

    // Stop here if the blackboard isn't available
    if (!BlackboardComp)
    {
        return;
    }

    // Check if the enemy can currently see the player
    if (Stimulus.WasSuccessfullySensed())
    {
        // Save the player as the target so the enemy can chase
        BlackboardComp->SetValueAsObject(
            TEXT("TargetActor"), Actor);
    }
    else
    {
        // Player is no longer detected, so check our current target
        if (BlackboardComp->GetValueAsObject(
            TEXT("TargetActor")) == Actor)
        {
            // Clear the target so the enemy stops chasing
            // The behavior tree can then return to guarding or patrolling
            BlackboardComp->ClearValue(
                TEXT("TargetActor"));
        }
    }
}


