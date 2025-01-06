#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_GameModeBase.generated.h"

class ACPP_PlayerController;
class ACPP_Boat;
class ACPP_Field;

UCLASS()
class ACPP_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ACPP_GameModeBase();

protected:
	// GameModeBase overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Input handling functions
	void HandleAxisTurn(ACPP_PlayerController* PlayerController, float Value);
	void HandleAxisBoatMainSailAngle(ACPP_PlayerController* PlayerController, float Value);
	void HandleAxisBoatJibOrSpinnakerAngle(ACPP_PlayerController* PlayerController, float Value, bool bIsControllingJib);
	void HandleAxisHikingOutTrimmer(ACPP_PlayerController* PlayerController, float Value);
	void HandleAxisHikingOutHelmsman(ACPP_PlayerController* PlayerController, float Value);
	void HandleActionToggleSpinnaker(ACPP_PlayerController* PlayerController);
	void HandleActionSwitchControl(ACPP_PlayerController* PlayerController, bool& bIsControllingJib);

	// Pawn possession function
	void PossessPawn(ACPP_Boat* Boat);

	// Field reference, replicated
	UPROPERTY(Replicated)
	ACPP_Field* Field;

private:
	// Player and Boat properties
	int32 NumPlayer;
	TSubclassOf<ACPP_Boat> BoatClass;
	TArray<ACPP_PlayerController*> PlayerControllers;
	FTransform SpawnPosition;

	// Temporary Player Controller pointer
	UPROPERTY()
	ACPP_PlayerController* tmpPlayerController;
};
