// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "QuickSequenceStyle.h"

class FQuickSequenceCommands : public TCommands<FQuickSequenceCommands>
{
public:

	FQuickSequenceCommands()
		: TCommands<FQuickSequenceCommands>(TEXT("QuickSequence"), NSLOCTEXT("Contexts", "QuickSequence", "QuickSequence Plugin"), NAME_None, FQuickSequenceStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};