#include "CPP_PlayerController.h"
#include "CPP_Boat.h"
#include "CPP_GameModeBase.h"
#include "Kismet/GameplayStatics.h"

ACPP_PlayerController::ACPP_PlayerController()
{
    bIsControllingJib = true;  // Initialize the player to control the jib by default
    CachedGameMode = nullptr;  // Cached reference to the game mode
}

void ACPP_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind axis inputs to corresponding control functions
    InputComponent->BindAxis("Turn", this, &ACPP_PlayerController::Turn);
    InputComponent->BindAxis("AdjustMainSail", this, &ACPP_PlayerController::AdjustMainSail);
    InputComponent->BindAxis("AdjustJibOrSpinnaker", this, &ACPP_PlayerController::AdjustJibOrSpinnaker);
    InputComponent->BindAxis("HikingOutTrimmer", this, &ACPP_PlayerController::HikingOutTrimmer);
    InputComponent->BindAxis("HikingOutHelmsman", this, &ACPP_PlayerController::HikingOutHelmsman);

    // Bind action inputs to corresponding action functions
    InputComponent->BindAction("ToggleSpinnaker", IE_Pressed, this, &ACPP_PlayerController::ToggleSpinnaker);
    InputComponent->BindAction("SwitchControl", IE_Pressed, this, &ACPP_PlayerController::SwitchControl);

    // Cache a reference to the game mode for future use
    CachedGameMode = Cast<ACPP_GameModeBase>(UGameplayStatics::GetGameMode(this));
}

ACPP_Boat* ACPP_PlayerController::GetControlledBoat() const
{
    // Returns the controlled boat by casting the possessed Pawn to ACPP_Boat
    return Cast<ACPP_Boat>(GetPawn());
}

void ACPP_PlayerController::Turn(float Value)
{
    // Send the turn axis value to the game mode to handle the turning logic
    if (CachedGameMode)
    {
        CachedGameMode->HandleAxisTurn(this, Value);
    }
}

void ACPP_PlayerController::AdjustMainSail(float Value)
{
    // Send the main sail adjustment axis value to the game mode
    if (CachedGameMode)
    {
        CachedGameMode->HandleAxisBoatMainSailAngle(this, Value);
    }
}

void ACPP_PlayerController::AdjustJibOrSpinnaker(float Value)
{
    // Send the jib or spinnaker adjustment axis value to the game mode, based on the currently controlled sail
    if (CachedGameMode)
    {
        CachedGameMode->HandleAxisBoatJibOrSpinnakerAngle(this, Value, bIsControllingJib);
    }
}

void ACPP_PlayerController::HikingOutTrimmer(float Value)
{
    // Send the hiking out input for the trimmer role to the game mode
    if (CachedGameMode)
    {
        CachedGameMode->HandleAxisHikingOutTrimmer(this, Value);
    }
}

void ACPP_PlayerController::HikingOutHelmsman(float Value)
{
    // Send the hiking out input for the helmsman role to the game mode
    if (CachedGameMode)
    {
        CachedGameMode->HandleAxisHikingOutHelmsman(this, Value);
    }
}

void ACPP_PlayerController::ToggleSpinnaker()
{
    // Trigger the action to toggle the spinnaker sail in the game mode
    if (CachedGameMode)
    {
        CachedGameMode->HandleActionToggleSpinnaker(this);
    }
}

void ACPP_PlayerController::SwitchControl()
{
    // Trigger the action to switch control between the jib and spinnaker
    if (CachedGameMode)
    {
        CachedGameMode->HandleActionSwitchControl(this, bIsControllingJib);
    }
}