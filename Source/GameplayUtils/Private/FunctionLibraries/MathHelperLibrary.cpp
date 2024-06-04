#include "FunctionLibraries/MathHelperLibrary.h"

FVector2D UMathHelperLibrary::RandPointInCircle(FVector2D CircleCenter, float CircleRadius)
{
    FVector2D TargetPoint2D = FMath::RandPointInCircle(CircleRadius);
    return CircleCenter + TargetPoint2D;
}

TArray<FVector> UMathHelperLibrary::SplitLineByLength(const FVector& StartPoint, const FVector& EndPoint, float UnitLength)
{
    TArray<FVector> Result;

    //把UnitLen转为恰好合适UnitLength
    FVector Direction = EndPoint - StartPoint;
    float TotalLength = Direction.Size();
    int Num = FMath::CeilToInt( TotalLength / UnitLength);
    UnitLength = TotalLength / Num;

    Direction.Normalize(0.0001);
    for (int i = 0; i < Num; i++)
    {
        Result.Add(StartPoint + Direction * UnitLength * i);
    }
    Result.Add(EndPoint);

    return MoveTemp(Result);
}

FRotator UMathHelperLibrary::MakeRotBetweenVectors(const FRotator& Rotator, const FVector& StartVector, const FVector& EndVector)
{
    FQuat VectorRot = FQuat::FindBetweenVectors(StartVector, EndVector);
    FQuat Quat = VectorRot * Rotator.Quaternion();
    return Quat.Rotator();
}
