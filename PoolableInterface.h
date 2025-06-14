// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};


class RUHUIK_API IPoolableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable")
	void OnSpawnFromPool();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable")
	void OnReturnToPool();
};