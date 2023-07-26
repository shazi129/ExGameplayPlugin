#include "ExMathLibrary.h"

bool UExMathLibrary::InBound2D(FVector2D Point, FVector2D Min, FVector2D Max)
{
	if (Min.X <= Point.X && Point.X <= Max.X && Min.Y <= Point.Y && Point.Y <= Max.Y)
	{
		return true;
	}
	return false;
}

FVector2D UExMathLibrary::CalcRectIntersection(FVector2D StartPoint, FVector2D EndPoint, FVector2D Min, FVector2D Max)
{
	if (StartPoint.X == EndPoint.X)
	{
		return EndPoint.Y >= StartPoint.Y ? FVector2D(StartPoint.X, Max.Y) : FVector2D(StartPoint.X, Min.Y);
	}
	else if (StartPoint.Y == EndPoint.Y)
	{
		return EndPoint.X >= StartPoint.X ? FVector2D(Max.X, StartPoint.Y) : FVector2D(Min.X, StartPoint.Y);
	}

	float k = (EndPoint.Y - StartPoint.Y) / (EndPoint.X - StartPoint.X);

	FVector2D V1 = FVector2D(Min.X, k * (Min.X - StartPoint.X) + StartPoint.Y);

	if (UExMathLibrary::InBound2D(V1, Min, Max))
	{
		return EndPoint.X >= StartPoint.X ? FVector2D(Max.X, k * (Max.X - StartPoint.X) + StartPoint.Y) : V1;
	}
	else
	{
		return EndPoint.Y >= StartPoint.Y ? FVector2D((Max.Y - StartPoint.Y) / k + StartPoint.X, Max.Y) : FVector2D((Min.Y - StartPoint.Y) / k + StartPoint.X, Min.Y);
	}
}

float UExMathLibrary::GetRotationAngle(FVector2D FromVector, FVector2D ToVector)
{
	FromVector.Normalize();
	ToVector.Normalize();

	float Dot = FVector2D::DotProduct(FromVector, ToVector);

	//方向， 逆时针旋转为负
	float Direction = 1;

	float Cros = FVector2D::CrossProduct(FromVector, ToVector) * -1;
	if (FMath::Abs(Cros) > SMALL_NUMBER)
	{
		Direction = Cros / FMath::Abs(Cros);
	}

	float Angle = (FMath::Acos(Dot) * 180) / PI;
	return Angle * Direction;
}

FVector UExMathLibrary::RotateVector(const FVector& InVector, const FRotator& InRotator)
{
	double VectorLenght = InVector.Length();
	FVector VectorNormal = InVector.GetSafeNormal();
	return (VectorNormal.Rotation() + InRotator).Vector() * VectorLenght;
}
