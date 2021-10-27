#include "MyDockTab.h"
#include "Kismet/GameplayStatics.h"
#include "GeometryCache/Classes/GeometryCacheActor.h"
#include "GeometryCache/Classes/GeometryCacheComponent.h"
#include "GeometryCache/Classes/GeometryCache.h"
#include "AlembicLibrary/Public/AbcFile.h"
#include "AlembicLibrary/Public/AbcPolyMesh.h"
#include "AssetRegistryModule.h"
#include "Materials/MaterialInstance.h"

static const FName MyDockTabName("MyQuickSequence");

#define LOCTEXT_NAMESPACE "FQuickSequenceModule"

MyDockTab::MyDockTab()
{
	InitDockTab();
}
MyDockTab::~MyDockTab(){}

MyDockTab MyDockTab::Get()
{
	static TUniquePtr<MyDockTab> Singleton;
	if (!Singleton) {
		Singleton = MakeUnique<MyDockTab>();
	}
	return *Singleton;
}

void MyDockTab::InitDockTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MyDockTabName, FOnSpawnTab::CreateRaw(this, &MyDockTab::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FQuickSequenceTabTitle", "QuickSequence"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void MyDockTab::InvokeDockTab()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MyDockTabName);
}

TSharedRef<class SDockTab> MyDockTab::OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)

					+SVerticalBox::Slot()
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("角色")))
						.OnClicked(FOnClicked::CreateRaw(this, &MyDockTab::OnCharButtonClick))
					]
				]
			]
		];
}

FReply MyDockTab::OnCharButtonClick()
{
	UWorld* World = GEditor->GetAllViewportClients()[0]->GetWorld();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		AGeometryCacheActor* MyActor = Cast<AGeometryCacheActor>(Actor);
		if (MyActor)
		{
			UGeometryCacheComponent* MyComponent = MyActor->GetGeometryCacheComponent();
			UGeometryCache* MyCache = MyComponent->GetGeometryCache();
			UObject* InParent = MyCache->GetOuter();

			MyCache->Materials.Empty();
			UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);

			// Read AbcFile
			FAbcFile* AbcFile = new FAbcFile(TEXT("E:/TestProjects/UE_Pilpeline/Content/RenderLevels/Animation/c001/Cache_kaiSiT01_hi_rig_kaiSiT02.abc"));
			AbcFile->Open();

			// Get UniqueFaceSetNames
			const TArray<FAbcPolyMesh*>& PolyMeshes = AbcFile->GetPolyMeshes();
			TArray<FString> UniqueFaceSetNames;
			for (FAbcPolyMesh* PolyMesh : PolyMeshes)
			{
				if (PolyMesh->bShouldImport)
				{
					for (FString FaceSetName : PolyMesh->FaceSetNames)
					{
						UniqueFaceSetNames.AddUnique(FaceSetName);
					}
					//bRequiresDefaultMaterial |= PolyMesh->FaceSetNames.Num() == 0;
				}
			}

			// find material
			MyCache->Materials.Add(DefaultMaterial);	// todo，正常情况下不会有一个空材质在这。MyCache的Mat数量如果和FaceSet数量对不上，视为错误
			for (FString FaceSetName : UniqueFaceSetNames)
			{
				UMaterialInterface* Material = nullptr;
				TArray<FAssetData> AssetData;
				const UClass* Class = UMaterialInterface::StaticClass();
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				AssetRegistryModule.Get().GetAssetsByClass(Class->GetFName(), AssetData, true);
				FAssetData* MaterialAsset = AssetData.FindByPredicate([=](const FAssetData& Asset)
					{
						return Asset.AssetName.ToString() == FaceSetName;
					});

				if (MaterialAsset)
				{
					UMaterialInterface* FoundMaterialInterface = Cast<UMaterialInterface>(MaterialAsset->GetAsset());
					if (FoundMaterialInterface)
					{
						Material = FoundMaterialInterface;
						UMaterial* BaseMaterial = Cast<UMaterial>(FoundMaterialInterface);
						if (!BaseMaterial)
						{
							if (UMaterialInstance* FoundInstance = Cast<UMaterialInstance>(FoundMaterialInterface))
							{
								BaseMaterial = FoundInstance->GetMaterial();
							}
							if (BaseMaterial)
							{
								bool bNeedsRecompile = false;
								BaseMaterial->SetMaterialUsage(bNeedsRecompile, MATUSAGE_GeometryCache);
							}
						}
						if (Material->IsValidLowLevel())
						{
							if (Material->GetOuter() == GetTransientPackage())
							{
								UMaterial* ExistingTypedObject = FindObject<UMaterial>(InParent, *FaceSetName);
								if (!ExistingTypedObject)
								{
									// This is in for safety, as we do not expect this to happen
									UObject* ExistingObject = FindObject<UObject>(InParent, *FaceSetName);
									if (ExistingObject)
									{
										Material = nullptr;
									}

									Material->Rename(*FaceSetName, InParent);
									Material->SetFlags(RF_Public | RF_Standalone);
									FAssetRegistryModule::AssetCreated(Material);
								}
								else
								{
									ExistingTypedObject->PreEditChange(nullptr);
									Material = ExistingTypedObject;
								}
							}
						}
						else
						{
							// In this case recreate the material
							Material = NewObject<UMaterial>(InParent, *FaceSetName);
							Material->SetFlags(RF_Public | RF_Standalone);
							FAssetRegistryModule::AssetCreated(Material);
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("can't find material: %s"), *FaceSetName);
				}

				// Set to MaterialInterface
				MyCache->Materials.Add((Material != nullptr && Material != NULL) ? Material : DefaultMaterial);
				MyCache->PostEditChange();
				if (Material != UMaterial::GetDefaultMaterial(MD_Surface) && Material)
				{
					Material->PostEditChange();

				}
				UE_LOG(LogTemp, Warning, TEXT("add"));
			}

			MyCache->MarkPackageDirty();
		}
		bool i = true;
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
