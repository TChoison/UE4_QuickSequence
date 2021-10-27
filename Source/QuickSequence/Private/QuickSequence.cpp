// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuickSequence.h"
#include "QuickSequenceStyle.h"
#include "QuickSequenceCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "MyDockTab.h"

static const FName QuickSequenceTabName("QuickSequence");

#define LOCTEXT_NAMESPACE "FQuickSequenceModule"

void FQuickSequenceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FQuickSequenceStyle::Initialize();
	FQuickSequenceStyle::ReloadTextures();

	FQuickSequenceCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FQuickSequenceCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FQuickSequenceModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FQuickSequenceModule::RegisterMenus));
	
	//FGlobalTabmanager::Get()->RegisterNomadTabSpawner(QuickSequenceTabName, FOnSpawnTab::CreateRaw(this, &FQuickSequenceModule::OnSpawnPluginTab))
	//	.SetDisplayName(LOCTEXT("FQuickSequenceTabTitle", "QuickSequence"))
	//	.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FQuickSequenceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FQuickSequenceStyle::Shutdown();

	FQuickSequenceCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(QuickSequenceTabName);
}

TSharedRef<SDockTab> FQuickSequenceModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FQuickSequenceModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("QuickSequence.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

FReply FQuickSequenceModule::OnCharButtonClick()
{
	return FReply::Handled();
}

void FQuickSequenceModule::PluginButtonClicked()
{
	//FGlobalTabmanager::Get()->TryInvokeTab(QuickSequenceTabName);
	MyDockTab::Get().InvokeDockTab();
}

void FQuickSequenceModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FQuickSequenceCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FQuickSequenceCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuickSequenceModule, QuickSequence)