// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBlackHole.h"

#include "Components/SphereComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComponent;

    InnerSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComponent"));
    InnerSphereComponent->SetSphereRadius(100);
    InnerSphereComponent->SetupAttachment(MeshComponent);

    InnerSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OverlapInnerSphere);
    
    OuterSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    OuterSphereComponent->SetSphereRadius(3000);
    OuterSphereComponent->SetupAttachment(MeshComponent);
}

void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        OtherActor->Destroy();
    }
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TArray<UPrimitiveComponent*> OverlappingActors;
    OuterSphereComponent->GetOverlappingComponents(OverlappingActors);

    if (OverlappingActors.Num())
    {
        for (int32 i = 0; i < OverlappingActors.Num(); i++)
        {
            UPrimitiveComponent* PrimitiveComponent = OverlappingActors[i];

            if (PrimitiveComponent && PrimitiveComponent->IsSimulatingPhysics())
            {
                const float SphereRadius = OuterSphereComponent->GetScaledSphereRadius();
                const float ForceStrength = -2000;

                PrimitiveComponent->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
            }
        }
    }
}
