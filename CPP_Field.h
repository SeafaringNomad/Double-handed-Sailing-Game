#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Field.generated.h"

USTRUCT()
struct SAILING_API FFieldZonesVariables
{
	GENERATED_BODY()

public:
	float WindDirection;
	float WindMagnitude;
	float CurrentDirection;
	float CurrentMagnitude;

	FFieldZonesVariables()
		: WindDirection(0.0f)
		, WindMagnitude(1.0f)
		, CurrentDirection(0.0f)
		, CurrentMagnitude(10.0f)
	{
	}
};

UCLASS()
class SAILING_API ACPP_Field : public AActor
{
	GENERATED_BODY()
    
public:    
	// Constructor
	ACPP_Field();

protected:
	// AActor overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// Helper function
	int32 RandomValue(float parameter);

	// Field properties
	float ZoneScale;
	FVector2D Corner1;
	FVector2D Corner2;

	// Field zone data
	TArray<FFieldZonesVariables> FieldZones;
	int32 Rows;
	int32 Columns;

public:
	// Field zone functions
	void UpdateField();
	void InitializeFieldZones(int32 InRows, int32 InColumns);
	TArray<int32> LocatePresentFieldZone(FVector2D PawnLocation) const;
	float GetPresentWindDirection(FVector2D PawnLocation);
	float GetPresentWindMagnitude(FVector2D PawnLocation);
	float GetPresentCurrentDirection(FVector2D PawnLocation);
	float GetPresentCurrentMagnitude(FVector2D PawnLocation);
};
