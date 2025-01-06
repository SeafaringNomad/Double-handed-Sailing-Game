#pragma once
#include "CoreMinimal.h"
#include "CPP_Field.h"
#include "GameFramework/Pawn.h"
#include "CPP_SailSetting.h"
#include "CPP_GameModeBase.h"
#include "CPP_Boat.generated.h"

UCLASS()
class SAILING_API ACPP_Boat : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACPP_Boat();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* BoatMesh;

    ACPP_GameModeBase* GameMode;

    float AngleStandardizating(float Angle);

    float PresentSpeed;
    
    float BoatWindAngle;
    
    float PresentWindMagnitude;
    
    float RollHikingTorque;

    float NavigationAngle;

    float DeltaNavigationAngle;

    float YawAngularSpeed;


public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCPP_SailSetting* SailSetting;
    
    void AxisTurn(float Value);

    void AxisAdjustMainSail(float Value);
    
    void AxisAdjustJib(float Value);
    
    void AxisAdjustSpinnaker(float Value);
    
    void AxisHikingOutTrimmer(float Value);
    
    void AxisHikingOutHelmsman(float Value);
    
    void ActionToggleSpinnaker();
    
    void ApplySailAndPlayerForce(float DeltaTime);

    // Getters for boat's state
    float GetNavigationAngle() const {return NavigationAngle;}
    
    float GetPresentSpeed() const {return PresentSpeed;}
    
    float GetBoatWindAngle() const {return BoatWindAngle;}
    
    FVector2D GetPawnLocation() const;

    UFUNCTION(BlueprintCallable)
    float GetYawAngularSpeed() const {return YawAngularSpeed;}

    UFUNCTION(BlueprintCallable)
    FVector2D GetMainSailForce() const {return SailSetting->GetMainSailForce();}

    UFUNCTION(BlueprintCallable)
    FVector2D GetJibForce() const {return SailSetting->GetJibForce();}

    UFUNCTION(BlueprintCallable)
    FVector2D GetSpinnakerForce() const {return SailSetting->GetSpinnakerForce();}
    
    UFUNCTION(BlueprintCallable)
    float GetBoatMainSailAngle() const {return SailSetting->GetBoatMainSailAngle();}

    UFUNCTION(BlueprintCallable)
    float GetBoatJibAngle() const {return SailSetting->GetBoatJibAngle();}

    UFUNCTION(BlueprintCallable)
    float GetBoatSpinnakerAngle() const {return SailSetting->GetBoatSpinnakerAngle();}
    
    UFUNCTION(BlueprintCallable)
    bool IsSpinnakerOpen() const {return SailSetting->IsSpinnakerOpen();}

    UFUNCTION(BlueprintCallable)
    float GetRollHikingForce() const {return RollHikingTorque;}

    UFUNCTION(BlueprintCallable)
    float GetPresentWindMagnitude() const {return GameMode->Field->GetPresentWindMagnitude(GetPawnLocation());}
};
