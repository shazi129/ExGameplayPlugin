#include "FunctionLibraries/MovementHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void FMovementData::Reset()
{
	Timestamp = 0;
	Velocity.Set(0, 0, 0);
	Acceleration.Set(0, 0, 0);
}

FString FMovementData::ToString() const
{
	FString Result;
	Result.Append(FString::Printf(TEXT("        Timestamp: %lld \n"), Timestamp));
	Result.Append(FString::Printf(TEXT("     Acceleration: %s \n"), *Acceleration.ToString()));
	Result.Append(FString::Printf(TEXT("         Velocity: %s \n"), *Velocity.ToString()));
	Result.Append(FString::Printf(TEXT("        Locaction: %s \n"), *Transform.GetLocation().ToString()));
	Result.Append(FString::Printf(TEXT("         Rotation: %s \n"), *Transform.GetRotation().Rotator().ToString()));
	return Result;
}

FMovementData UMovementHelperLibrary::PredictMovement(const FMovementData& MovementData, const FPredictMovementParmeter& PredictParamter, float DeltaSeconds)
{
	FMovementData NewMovementData = MovementData;

	//通过速度预测到的移动距离
	FVector Distance = FVector::ZeroVector;

	//根据速度计算相对位移
	if (!MovementData.Velocity.IsNearlyZero() || !MovementData.Acceleration.IsNearlyZero())
	{
		//新速度
		NewMovementData.Velocity = MovementData.Velocity + MovementData.Acceleration * DeltaSeconds;
		if (PredictParamter.MaxSpeed > 0.0001f)
		{
			float NewSpeed = NewMovementData.Velocity.Size();
			if (NewSpeed > PredictParamter.MaxSpeed)
			{
				NewSpeed = PredictParamter.MaxSpeed;
				NewMovementData.Velocity.Normalize();
				NewMovementData.Velocity = NewMovementData.Velocity * NewSpeed;
			}
		}

		//如果出现方向相反的情况, 不做后退处理
		if (NewMovementData.Velocity.Dot(MovementData.Velocity) < 0)
		{
			NewMovementData.Velocity = MovementData.Velocity * 0.01;
			NewMovementData.Acceleration = FVector::ZeroVector;
		}

		//运动的距离
		Distance = (MovementData.Velocity + NewMovementData.Velocity) * DeltaSeconds / 2;
	}

	//根据base对运动距离进行修正
	if (PredictParamter.BasedData.bHasBasedTransform)
	{
		//根据base得到的绝对位移
		Distance = UKismetMathLibrary::TransformDirection(PredictParamter.BasedData.BasedTransform, Distance);
	}

	NewMovementData.Transform.SetLocation(MovementData.Transform.GetLocation() + Distance);
	return NewMovementData;
}
