#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_PlayerController.generated.h"

class ACPP_GameModeBase;
class ACPP_Boat;

UCLASS()
class ACPP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACPP_PlayerController();
	ACPP_Boat* GetControlledBoat() const;
	bool bIsControllingJib;

protected:
	virtual void SetupInputComponent() override;

private:
	void Turn(float Value);
	void AdjustMainSail(float Value);
	void AdjustJibOrSpinnaker(float Value);
	void HikingOutTrimmer(float Value);
	void HikingOutHelmsman(float Value);
	void ToggleSpinnaker();
	void SwitchControl();

	UPROPERTY()
	ACPP_GameModeBase* CachedGameMode;
};
