#include "CPP_Field.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ACPP_Field::ACPP_Field()
{
    PrimaryActorTick.bCanEverTick = true;

    Corner1 = FVector2D(-100000.0f, -100000.0f);
    Corner2 = FVector2D(100000.0f, 100000.0f);
    ZoneScale = 200.0f;
    Rows = 0;
    Columns = 0;
}

// Called when the game starts or when spawned
void ACPP_Field::BeginPlay()
{
    Super::BeginPlay();
    InitializeFieldZones(1000, 1000);
    UpdateField();
}

// Called every frame
void ACPP_Field::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACPP_Field::InitializeFieldZones(int32 InRows, int32 InColumns)
{
    // Initialize the field with a specified number of rows and columns
    Rows = InRows;
    Columns = InColumns;
    FieldZones.SetNum(Rows * Columns);

    // Generate random wind and current directions for each zone
    float WindAngle = FMath::FRand() * 2.0f * PI;
    float CurrentAngle = FMath::FRand() * 2.0f * PI;

    for (int32 i = 0; i < Rows; ++i)
    {
        for (int32 j = 0; j < Columns; ++j)
        {
            int32 Index = i * Columns + j;
            FieldZones[Index] = FFieldZonesVariables();
            FieldZones[Index].WindDirection = WindAngle;
            FieldZones[Index].CurrentDirection = CurrentAngle;
        }
    }
}


TArray<int32> ACPP_Field::LocatePresentFieldZone(FVector2D PawnLocation) const
{
    TArray<int32> ZoneLocation;
    float PawnX = PawnLocation.X;
    float PawnY = PawnLocation.Y;

    // Check if the pawn is within the field boundaries
    if (PawnX >= Corner1.X && PawnX <= Corner2.X && PawnY >= Corner1.Y && PawnY <= Corner2.Y)
    {
        int32 ZoneX = FMath::FloorToInt((PawnX - Corner1.X) / ZoneScale);
        int32 ZoneY = FMath::FloorToInt((PawnY - Corner1.Y) / ZoneScale);

        // Ensure the calculated zone is within valid indices
        if (ZoneX >= 0 && ZoneX < Columns && ZoneY >= 0 && ZoneY < Rows) 
        {
            ZoneLocation.Add(ZoneX);
            ZoneLocation.Add(ZoneY);
        }
    }

    return ZoneLocation;
}


float ACPP_Field::GetPresentWindDirection(FVector2D PawnLocation)
{
    // Get the wind direction for the current pawn location
    TArray<int32> ZoneLocation = LocatePresentFieldZone(PawnLocation);

    if (ZoneLocation.Num() == 2)
    {
        int32 ZoneX = ZoneLocation[0];
        int32 ZoneY = ZoneLocation[1];

        if (ZoneX >= 0 && ZoneX < Rows && ZoneY >= 0 && ZoneY < Columns)
        {
            int32 Index = ZoneX * Columns + ZoneY;
            return FieldZones[Index].WindDirection;
        }
    }
    return -1.0f; // Return -1.0f if the location is outside of bounds
}

float ACPP_Field::GetPresentWindMagnitude(FVector2D PawnLocation)
{
    // Get the wind magnitude for the current pawn location
    TArray<int32> ZoneLocation = LocatePresentFieldZone(PawnLocation);

    if (ZoneLocation.Num() == 2)
    {
        int32 ZoneX = ZoneLocation[0];
        int32 ZoneY = ZoneLocation[1];

        if (ZoneX >= 0 && ZoneX < Rows && ZoneY >= 0 && ZoneY < Columns)
        {
            int32 Index = ZoneX * Columns + ZoneY;
            return FieldZones[Index].WindMagnitude;
        }
    }
    return -1.0f; // Return -1.0f if the location is outside of bounds
}

float ACPP_Field::GetPresentCurrentDirection(FVector2D PawnLocation)
{
    // Get the current direction for the current pawn location
    TArray<int32> ZoneLocation = LocatePresentFieldZone(PawnLocation);

    if (ZoneLocation.Num() == 2)
    {
        int32 ZoneX = ZoneLocation[0];
        int32 ZoneY = ZoneLocation[1];

        if (ZoneX >= 0 && ZoneX < Rows && ZoneY >= 0 && ZoneY < Columns)
        {
            int32 Index = ZoneX * Columns + ZoneY;
            return FieldZones[Index].CurrentDirection;
        }
    }
    return -1.0f; // Return -1.0f if the location is outside of bounds
}

float ACPP_Field::GetPresentCurrentMagnitude(FVector2D PawnLocation)
{
    // Get the current magnitude for the current pawn location
    TArray<int32> ZoneLocation = LocatePresentFieldZone(PawnLocation);

    if (ZoneLocation.Num() == 2)
    {
        int32 ZoneX = ZoneLocation[0];
        int32 ZoneY = ZoneLocation[1];

        if (ZoneX >= 0 && ZoneX < Rows && ZoneY >= 0 && ZoneY < Columns)
        {
            int32 Index = ZoneX * Columns + ZoneY;
            return FieldZones[Index].CurrentMagnitude;
        }
    }
    return -1.0f; // Return -1.0f if the location is outside of bounds
}

int32 ACPP_Field::RandomValue(float parameter)
{
    // Generate a random value based on the parameter, return 1 or 0
    static FRandomStream generator(FDateTime::Now().GetMillisecond());
    return generator.FRand() < parameter ? 1 : 0;
}

void ACPP_Field::UpdateField()
{
    // Initialize parameters for field generation
    float xBegin = 0.5f, xEnd = 0.5f, yBegin = 0.5f, yEnd = 0.5f;
    int maxAllowBlank = 14, maxAllowSolid = 14;

    // Phase 1: Generate initial wind map based on rules
    for (int32 i = 0; i < Rows; ++i)
    {
        for (int32 j = 0; j < Columns; ++j)
        {
            int32 Index = i * Columns + j;

            if (j == 0) // First column
            {
                if (i == 0) // Top-left corner
                {
                    FieldZones[Index].WindMagnitude = RandomValue((xBegin + yBegin) / 2);
                }
                else // Other rows in first column
                {
                    int32 AboveIndex = (i - 1) * Columns + j;
                    FieldZones[Index].WindMagnitude = RandomValue((xBegin + (FieldZones[AboveIndex].WindMagnitude ? yEnd : yBegin)) / 2);
                }
            }
            else // Other columns
            {
                if (i == 0) // First row
                {
                    int32 LeftIndex = i * Columns + (j - 1);
                    FieldZones[Index].WindMagnitude = RandomValue((xEnd + (FieldZones[LeftIndex].WindMagnitude ? yBegin : yEnd)) / 2);
                }
                else // All other zones
                {
                    int32 AboveIndex = (i - 1) * Columns + j;
                    int32 LeftIndex = i * Columns + (j - 1);

                    if (FieldZones[AboveIndex].WindMagnitude == 1 && FieldZones[LeftIndex].WindMagnitude == 1)
                    {
                        FieldZones[Index].WindMagnitude = RandomValue((xEnd + yEnd) / 2);
                    }
                    else if (FieldZones[AboveIndex].WindMagnitude == 1 || FieldZones[LeftIndex].WindMagnitude == 1)
                    {
                        FieldZones[Index].WindMagnitude = RandomValue((xEnd + yBegin) / 2);
                    }
                    else
                    {
                        FieldZones[Index].WindMagnitude = RandomValue((xBegin + yBegin) / 2);
                    }
                }
            }
        }
    }

    // Phase 2: Smooth the wind map by ensuring blank and solid regions are within the allowed range
    for (int32 k = 0; k < 5; ++k) // Repeat the smoothing process 5 times
    {
        for (int32 i = 0; i < Rows; ++i)
        {
            for (int32 j = 0; j < Columns; ++j)
            {
                // Skip boundary zones
                if (i < 2 || j < 2 || i > Rows - 3 || j > Columns - 3) continue;

                // Count the number of blank points in the 5x5 area
                int32 sumOfBlank = 24;
                for (int32 l = i - 2; l <= i + 2; ++l)
                {
                    for (int32 m = j - 2; m <= j + 2; ++m)
                    {
                        int32 NeighbourIndex = l * Columns + m;
                        sumOfBlank -= FieldZones[NeighbourIndex].WindMagnitude;
                    }
                }
                int32 Index = i * Columns + j;
                sumOfBlank += FieldZones[Index].WindMagnitude;

                // Adjust the current zone based on the surrounding blank and solid zones
                if (sumOfBlank > maxAllowBlank)
                {
                    FieldZones[Index].WindMagnitude = 0;
                }
                if (24 - sumOfBlank > maxAllowSolid)
                {
                    FieldZones[Index].WindMagnitude = 1;
                }
            }
        }
    }

    // Phase 3: Adjust wind magnitude based on the surrounding points' wind magnitude
    TArray<FFieldZonesVariables> TempFieldZones;
    TempFieldZones.SetNum(Rows * Columns);
    float param = 50; // Smoothing parameter

    for (int32 i = 0; i < Rows; ++i)
    {
        for (int32 j = 0; j < Columns; ++j)
        {
            int32 neighborCount = 0;
            int32 Index = i * Columns + j;

            // Count the number of neighboring zones with wind magnitude of 1
            for (int32 dx = -1; dx <= 1; ++dx)
            {
                for (int32 dy = -1; dy <= 1; ++dy)
                {
                    if (dx == 0 && dy == 0) continue; // Skip the center point

                    int32 ni = i + dx;
                    int32 nj = j + dy;
                    if (ni >= 0 && ni < Rows && nj >= 0 && nj < Columns)
                    {
                        int32 NeighborIndex = ni * Columns + nj;
                        if (FieldZones[NeighborIndex].WindMagnitude == 1)
                        {
                            neighborCount++;
                        }
                    }
                }
            }

            TempFieldZones[Index].WindMagnitude = (neighborCount + param) / param;
        }
    }

    // Copy the smoothed field back to the original field
    for (int32 i = 0; i < Rows; ++i)
    {
        for (int32 j = 0; j < Columns; ++j)
        {
            int32 Index = i * Columns + j;
            FieldZones[Index] = TempFieldZones[Index];
        }
    }

    // Uncomment below code to print the final FieldZones array for debugging
    /*
    for (int32 i = 0; i < Rows; ++i)
    {
        for (int32 j = 0; j < Columns; ++j)
        {
            int32 Index = i * Columns + j;
            UE_LOG(LogTemp, Log, TEXT("Final FieldZones[%d][%d].WindMagnitude = %f"), i, j, FieldZones[Index].WindMagnitude);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Final FieldZones[%d][%d].WindMagnitude = %f"), i, j, FieldZones[Index].WindMagnitude));
            }
        }
    }
    */
}