// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkPlayerController.h"

#include "NetVRkFileTransferSystem/Components/NetVRkTransferAccessComponent.h"

#pragma region Framework

ANetVRkPlayerController::ANetVRkPlayerController()
{
	TransferAccessComponent = CreateDefaultSubobject<UNetVRkTransferAccessComponent>(TEXT("TransferAccessComponent"));
	
}

#pragma endregion