 #include "CPP_SailSetting.h"
#include "CPP_Boat.h"
#include "CPP_Field.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCPP_SailSetting::UCPP_SailSetting()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Initialize angles for the boat's sails
    BoatMainSailAngle = PI/4.0f;
    BoatJibAngle = PI/4.0f;
    BoatSpinnakerAngle = PI/4.0f;

    OutboardWindSailBoundary = PI/4.0f;
    InboardWindSailBoundary = 0.0f;

    // Initialize angles between the wind and the sails
    WindMainSailAngle = 0.0f;
    WindJibAngle = 0.0f;
    WindSpinnakerAngle = 0.0f;

    // Initialize forces for each sail and total force
    MainSailForce = FVector2D::ZeroVector;
    JibForce = FVector2D::ZeroVector;
    SpinnakerForce = FVector2D::ZeroVector;
    TotalForce = FVector2D::ZeroVector;

    // Set sail areas
    MainSailArea = 13.8f;
    JibArea = 5.8f;
    SpinnakerArea = 25.1f;

    // By default, the spinnaker is not open
    bIsSpinnakerOpen = false;

    ForwardMaxSpeed = 0.0f;

    Boat = nullptr;
}

// Called when the game starts
void UCPP_SailSetting::BeginPlay()
{
    Super::BeginPlay();

    // Get reference to the boat actor
    Boat = Cast<ACPP_Boat>(GetOwner());

    // Get reference to the game mode
    GameMode = Cast<ACPP_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void UCPP_SailSetting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (Boat)
    {
        // Get the current navigation angle of the boat
        float NavigationAngle = Boat->GetNavigationAngle();
        
        // Update the angle between the wind and each sail
        UpdateWindSailAngle(NavigationAngle);
        
        UpdateWindSailBoundaries();
        
        // Update the forces acting on each sail
        UpdateSailForce();
    }
}

void UCPP_SailSetting::UpdateWindSailAngle(float NavigationAngle)
{
    // Update the wind angle relative to each sail
    WindMainSailAngle = NavigationAngle + this->BoatMainSailAngle;
    WindJibAngle = NavigationAngle + this->BoatJibAngle;
    if (bIsSpinnakerOpen) WindSpinnakerAngle = NavigationAngle + this->BoatSpinnakerAngle;
}


FVector2D UCPP_SailSetting::CalcSailForce(FName SailName) const
{
    // Determine which sail's force is being calculated and get its respective angle and area
    float BoatSailAngle, SailArea;
    
    if (SailName == FName(TEXT("Main Sail")))
    {
        BoatSailAngle = BoatMainSailAngle;
        SailArea = MainSailArea;
    }
    else if (SailName == FName(TEXT("Jib")))
    {
        BoatSailAngle = BoatJibAngle;
        SailArea = JibArea;
    }
    else if (SailName == FName(TEXT("Spinnaker")))
    {
        if (!bIsSpinnakerOpen) return FVector2D::ZeroVector;  // If spinnaker is closed, no force is generated
        BoatSailAngle = BoatSpinnakerAngle;
        SailArea = SpinnakerArea;
    }
    else
    {
        return FVector2D::ZeroVector;  // If the sail name is invalid, return zero vector
    }

    float BoatWindAngle = Boat->GetBoatWindAngle();  // Get wind angle relative to the boat
    float WindSailAngle = BoatWindAngle - BoatSailAngle;  // Calculate the wind angle relative to the sail
    
    // Initialize lateral and longitudinal lift forces
    float RollSailLift = 0.0f;
    float ForwardSailLift = 0.0f;
    
    // Calculate the normal forces acting on the sail in both the lateral and longitudinal directions
    float RollSailNormal = - AirDensity * Boat->GetPresentSpeed() * Boat->GetPresentSpeed() * FMath::Sin(WindSailAngle) * SailArea;
    float ForwardSailNormal = AirDensity * Boat->GetPresentSpeed() * Boat->GetPresentSpeed() * FMath::Cos(WindSailAngle) * SailArea;

    // Calculate lift based on wind angle, using different conditions for different ranges of angles
    if (BoatWindAngle != 0.0f)
    {
        // Condition 1: π/4 <= x <= 3π/4
        if (BoatWindAngle >= (PI / 4.0f) && BoatWindAngle <= (3.0f * PI / 4.0f))
        {
            ForwardSailLift = -0.1f * (BoatWindAngle - (PI / 4.0f)) * (BoatWindAngle - (7.0f * PI / 4.0f)) + 1.0f;
        }

        // Condition 2: 7π/4 <= x <= 2π
        else if (BoatWindAngle >= (7.0f * PI / 4.0f) && BoatWindAngle <= (2.0f * PI))
        {
            ForwardSailLift = -4.0f / PI * (BoatWindAngle - (2.0f * PI));
        }

        // Condition 3: 3π/4 <= x <= 5π/4
        else if (BoatWindAngle >= (3.0f * PI / 4.0f) && BoatWindAngle <= (5.0f * PI / 4.0f))
        {
            ForwardSailLift = 0.8f * (BoatWindAngle - (PI / 4.0f)) * (BoatWindAngle - (7.0f * PI / 4.0f)) + 3.94784f + 1.49348f;
        }

        // Condition 4: 5π/4 <= x <= 7π/4
        else if (BoatWindAngle >= (5.0f * PI / 4.0f) && BoatWindAngle <= (7.0f * PI / 4.0f))
        {
            ForwardSailLift = -0.1f * (BoatWindAngle - (PI / 4.0f)) * (BoatWindAngle - (7.0f * PI / 4.0f)) + 1.0f;
        }

        // Condition 5: 0 <= x <= π/4
        else if (BoatWindAngle >= 0 && BoatWindAngle <= (PI / 4.0f))
        {
            ForwardSailLift = 4.0f / PI * BoatWindAngle;
        }

        RollSailLift = ForwardSailLift;
        
        float OptimalBoatSailAngle;

        if (0.0f <= BoatWindAngle && BoatWindAngle <= PI / 4.0f) {
            OptimalBoatSailAngle = 0.0f;
        } else if (PI / 4.0f < BoatWindAngle && BoatWindAngle <= PI / 2.0f) {
            OptimalBoatSailAngle = BoatWindAngle - PI / 4.0f;
        } else if (PI / 2.0f < BoatWindAngle && BoatWindAngle <= 3.0f * PI / 4.0f) {
            OptimalBoatSailAngle = -BoatWindAngle + 3.0f * PI / 4.0f;
        } else if (3.0f * PI / 4.0f < BoatWindAngle && BoatWindAngle <= PI) {
            OptimalBoatSailAngle = 2.0f * BoatWindAngle - 3.0f * PI / 2.0f;
        } else if (PI < BoatWindAngle && BoatWindAngle <= 5.0f * PI / 4.0f) {
            OptimalBoatSailAngle = -2.0f * BoatWindAngle + 5.0f * PI / 2.0f;
        } else if (5.0f * PI / 4.0f < BoatWindAngle && BoatWindAngle <= 3.0f * PI / 2.0f) {
            OptimalBoatSailAngle = BoatWindAngle - 5.0f * PI / 4.0f;
        } else if (3.0f * PI / 2.0f < BoatWindAngle && BoatWindAngle <= 7.0f * PI / 4.0f) {
            OptimalBoatSailAngle = -BoatWindAngle + 7.0f * PI / 4.0f;
        } else if (7.0f * PI / 4.0f < BoatWindAngle && BoatWindAngle <= 2.0f * PI) {
            OptimalBoatSailAngle = 0.0f;
    }

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Optimal Boat Sail Angle: %f"), OptimalBoatSailAngle));

        if (FMath::Abs(BoatSailAngle)<OptimalBoatSailAngle){
            ForwardSailLift *= 2 * FMath::Abs(FMath::Abs(BoatSailAngle)-OptimalBoatSailAngle);
            // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Coefficient1: %f"), 2*FMath::Abs(BoatSailAngle)/OptimalBoatSailAngle));
        }

        if (FMath::Abs(BoatSailAngle)>OptimalBoatSailAngle){
            ForwardSailLift *= 2 * FMath::Abs(1 / (OptimalBoatSailAngle - PI/4) * (FMath::Abs(BoatSailAngle) - PI/4));
            // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Coefficient2: %f"), 2*(1 / (OptimalBoatSailAngle - PI/4)) * (FMath::Abs(BoatSailAngle) - PI/4)));
        }
    }

    // Combine normal and lift forces in the lateral direction
    float RollSailForce = RollSailNormal + RollSailLift;

    if (Boat->GetBoatWindAngle() <= PI)
    {
        RollSailForce = -RollSailForce;
    }

    // Combine normal and lift forces in the longitudinal direction
    float ForwardSailForce = ForwardSailNormal + ForwardSailLift - ForwardMaxSpeed;
    
    return FVector2D(RollSailForce*10, ForwardSailForce);  // Return the total force as a 2D vector
}

void UCPP_SailSetting::UpdateSailForce()
{
    // Calculate forces for each sail
    MainSailForce = CalcSailForce(FName(TEXT("Main Sail")));
    JibForce = CalcSailForce(FName(TEXT("Jib")));
    SpinnakerForce = CalcSailForce(FName(TEXT("Spinnaker")));

    // Combine forces from all sails
    TotalForce = MainSailForce + JibForce + SpinnakerForce;

    // Adjust the total force based on the current wind magnitude from the game field
    float PresentWindMagnitude = GameMode->Field->GetPresentWindMagnitude(Boat->GetPawnLocation());
    TotalForce *= PresentWindMagnitude;
}

void UCPP_SailSetting::UpdateWindSailBoundaries()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Outboard: %f"), OutboardWindSailBoundary));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Inboard: %f"), InboardWindSailBoundary));
}


void UCPP_SailSetting::ToggleSpinnaker()
{
    // Toggle the state of the spinnaker (open/close)
    bIsSpinnakerOpen = !bIsSpinnakerOpen;
}