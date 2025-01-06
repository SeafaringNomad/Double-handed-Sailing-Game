#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_GameModeBase.h"
#include "CPP_SailSetting.generated.h"

// 前向声明
class ACPP_Boat;
class ACPP_Field;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SAILING_API UCPP_SailSetting : public UActorComponent
{
    GENERATED_BODY()

public:
    // Constructor
    UCPP_SailSetting();

    // Wind Sail Angles
    void UpdateWindSailAngle(float NavigationAngle);

    // Wind Sail Angles Boundary
    void UpdateWindSailBoundaries();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // Boat reference
    UPROPERTY()
    ACPP_Boat* Boat;

    // Game Mode Reference
    TWeakObjectPtr<ACPP_GameModeBase> GameMode;

    // Air Density (constant)
    const float AirDensity = 0.0001f;

    // Sail Angles
    float WindMainSailAngle;
    float WindJibAngle;
    float WindSpinnakerAngle;

    // Upper and Lower Boundary
    float OutboardWindSailBoundary;
    float InboardWindSailBoundary; 

    // Sail Forces
    FVector2D CalcSailForce(FName SailName) const;

    // Sail Properties
    float BoatMainSailAngle;
    float BoatJibAngle;
    float BoatSpinnakerAngle;

    FVector2D MainSailForce;
    FVector2D JibForce;
    FVector2D SpinnakerForce;
    FVector2D TotalForce;

    float MainSailArea;
    float JibArea;
    float SpinnakerArea;

    bool bIsSpinnakerOpen;
    
    float ForwardMaxSpeed;

public:
    // Functions to update sail force and toggle spinnaker
    void UpdateSailForce();
    void ToggleSpinnaker();

    // Getter functions returning values
    float GetBoatMainSailAngle() const {return BoatMainSailAngle;}
    
    float GetBoatJibAngle() const {return BoatJibAngle;}
    
    float GetBoatSpinnakerAngle() const {return BoatSpinnakerAngle;}
    
    bool IsSpinnakerOpen() const {return bIsSpinnakerOpen;}

    float GetRollSailTorque() const {return TotalForce.X;}

    float GetForwardSailForce() const {return TotalForce.Y;}

    FVector2D GetMainSailForce() const {return MainSailForce;}

    FVector2D GetJibForce() const {return JibForce;}

    FVector2D GetSpinnakerForce() const {return SpinnakerForce;}

    void SetBoatMainSailAngle(float InBoatMainSailAngle) {this->BoatMainSailAngle = InBoatMainSailAngle;}

    void SetBoatJibAngle(float InBoatJibAngle){ this->BoatJibAngle = InBoatJibAngle; }

    void SetBoatSpinnakerAngle(float InBoatSpinnakerAngle) {this->BoatSpinnakerAngle = InBoatSpinnakerAngle;}

    void SetSpinnakerOpen(bool bInIsSpinnakerOpen) {this->bIsSpinnakerOpen = bInIsSpinnakerOpen;}
};
