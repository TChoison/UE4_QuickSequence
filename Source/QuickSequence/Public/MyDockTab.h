#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

class MyDockTab
{
public:
	MyDockTab();
	~MyDockTab();
	
	static MyDockTab Get();
	void InitDockTab();
	void InvokeDockTab();
private:
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	FReply OnCharButtonClick();
};