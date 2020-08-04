// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "FPSAIGuard.h"

AFPSGameMode::AFPSGameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;

    // use our custom HUD class
    HUDClass = AFPSHUD::StaticClass();

    GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool Success)
{
    if (InstigatorPawn != nullptr)
    {
        if (SpectatingViewpointClass)
        {
            TArray<AActor*> ReturnedActors;
            UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

            if (ReturnedActors.Num())
            {
                AActor* NewViewTarget = ReturnedActors[0];

                for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
                {
                    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get()))
                    {
                        PC->SetViewTargetWithBlend(NewViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
                    }
                }

                for (TActorIterator<AFPSAIGuard> It(GetWorld()); It; ++It)
                {
                    UE_LOG(LogTemp, Warning, TEXT("ITERATING GUARD"));
                    It->GetController()->StopMovement();
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spectating Class Not Set"));
        }
    }

    AFPSGameState* GS = GetGameState<AFPSGameState>();

    if (GS != nullptr)
    {
        GS->MultiCastOnMissionComplete(InstigatorPawn, Success);
    }

    OnMissionCompleted(InstigatorPawn, Success);
}
