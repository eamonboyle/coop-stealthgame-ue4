// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"


#include "FPSCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComp;

    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SphereComp->SetupAttachment(MeshComp);

    SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
    Super::BeginPlay();
}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    PlayEffects();

    if (GetLocalRole() == ROLE_Authority)
    {
        if (AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor))
        {
            Player->bIsCarryingObjective = true;

            Destroy();
        }
    }
}

void AFPSObjectiveActor::PlayEffects()
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupFX, GetActorLocation());
}
