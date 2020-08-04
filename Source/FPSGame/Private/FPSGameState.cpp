
// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FPSPlayerController.h"

void AFPSGameState::MultiCastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool Success)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get()))
        {
            if (PC->IsLocalController())
            {
                PC->OnMissionCompleted(InstigatorPawn, Success);

                APawn* MyPawn = PC->GetPawn();

                if (MyPawn != nullptr)
                {
                    MyPawn->DisableInput(PC);
                }
            }
        }
    }
}
