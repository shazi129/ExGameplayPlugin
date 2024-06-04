// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/GeneralAsyncActionObject.h"

#include "AsyncActionSubsystem.h"

void UGeneralAsyncActionObject::OnLatentActionCompletedFunc()
{
	UAsyncActionSubsystem * AsyncActionSubsystem = UAsyncActionSubsystem::GetSubsystem(this);
	if(AsyncActionSubsystem)
	{
		AsyncActionSubsystem->RemoveActionObject(this);
	}
	OnLatentActionCompleted.ExecuteIfBound();
}
