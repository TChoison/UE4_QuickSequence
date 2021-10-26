// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuickSequenceCommands.h"

#define LOCTEXT_NAMESPACE "FQuickSequenceModule"

void FQuickSequenceCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "QuickSequence", "Bring up QuickSequence window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
