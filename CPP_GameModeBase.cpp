#include "CPP_GameModeBase.h"
#include "Engine/World.h"
#include "CPP_PlayerController.h"
#include "CPP_Field.h"
#include "CPP_Boat.h"
#include "Net/UnrealNetwork.h"


ACPP_GameModeBase::ACPP_GameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
    NumPlayer = 0;
    BoatClass = ACPP_Boat::StaticClass();
    tmpPlayerController = nullptr;
    SpawnPosition = FTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, 100.0f), FVector(1.0f));
    Field = nullptr;
}

void ACPP_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Only spawn the Field on the server
    if (HasAuthority())
    {
        Field = GetWorld()->SpawnActor<ACPP_Field>();
    }
}

void ACPP_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACPP_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    
    // Cast the PlayerController to our custom ACPP_PlayerController
    ACPP_PlayerController* PlayerController = Cast<ACPP_PlayerController>(NewPlayer);
    tmpPlayerController = PlayerController;
    if (PlayerController)
    {
        NumPlayer++;  // Increase player count
        PlayerControllers.Add(PlayerController);  // Add the new PlayerController to the array
    }
}

void ACPP_GameModeBase::PossessPawn(ACPP_Boat* Boat)
{
    // Check if the PlayerController and Boat are valid
    if (tmpPlayerController && Boat)
    {
        tmpPlayerController->Possess(Boat);  // Possess the boat
        tmpPlayerController->SetViewTargetWithBlend(Boat);  // Switch the view to the possessed boat
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Boat Possessed"));
        }
    }
}

void ACPP_GameModeBase::HandleAxisTurn(ACPP_PlayerController* PlayerController, float Value)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            Boat->AxisTurn(Value);  // Apply turning axis input to the boat
        }
    }
}

void ACPP_GameModeBase::HandleAxisBoatMainSailAngle(ACPP_PlayerController* PlayerController, float Value)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            Boat->AxisAdjustMainSail(Value);  // Adjust the main sail angle based on input value
        }
    }
}

void ACPP_GameModeBase::HandleAxisBoatJibOrSpinnakerAngle(ACPP_PlayerController* PlayerController, float Value, bool bIsControllingJib)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            // Adjust either the jib or spinnaker sail angle based on which is being controlled
            if (bIsControllingJib)
            {
                Boat->AxisAdjustJib(Value);
            }
            else
            {
                Boat->AxisAdjustSpinnaker(Value);
            }
        }
    }
}

void ACPP_GameModeBase::HandleAxisHikingOutTrimmer(ACPP_PlayerController* PlayerController, float Value)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            Boat->AxisHikingOutTrimmer(Value);  // Handle hiking out action for the trimmer
        }
    }
}

void ACPP_GameModeBase::HandleAxisHikingOutHelmsman(ACPP_PlayerController* PlayerController, float Value)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            Boat->AxisHikingOutHelmsman(Value);  // Handle hiking out action for the helmsman
        }
    }
}

void ACPP_GameModeBase::HandleActionToggleSpinnaker(ACPP_PlayerController* PlayerController)
{
    if (PlayerController)
    {
        ACPP_Boat* Boat = PlayerController->GetControlledBoat();
        if (Boat)
        {
            Boat->ActionToggleSpinnaker();  // Toggle the spinnaker sail
        }
    }
}

void ACPP_GameModeBase::HandleActionSwitchControl(ACPP_PlayerController* PlayerController, bool& bIsControllingJib)
{
    if (PlayerController)
    {
        // Toggle between controlling the jib and the spinnaker
        PlayerController->bIsControllingJib = !PlayerController->bIsControllingJib;
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Now controlling %s"), PlayerController->bIsControllingJib ? TEXT("Jib") : TEXT("Spinnaker")));
        }
    }
}

void ACPP_GameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Ensure that the Field actor is replicated
    DOREPLIFETIME(ACPP_GameModeBase, Field);
}