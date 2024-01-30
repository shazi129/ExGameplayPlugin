#include "FunctionLibraries/MathHelperLibrary.h"

FVector2D UMathHelperLibrary::RandPointInCircle(FVector2D CircleCenter, float CircleRadius)
{
    FVector2D TargetPoint2D = FMath::RandPointInCircle(CircleRadius);
    return CircleCenter + TargetPoint2D;
}