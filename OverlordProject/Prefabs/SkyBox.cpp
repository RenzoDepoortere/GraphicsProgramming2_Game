#include "stdafx.h"
#include "SkyBox.h"
#include "Materials/SkyBoxMaterial.h"

void SkyBox::Initialize(const SceneContext& /*sceneContext*/)
{
	// Material
	SkyBoxMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<SkyBoxMaterial>()};
	pMaterial->SetSkyBoxTexture(L"Textures/SkyBox.dds");

	// Model
	ModelComponent* pModel{new ModelComponent(L"Meshes/Box.ovm")};
	pModel->SetMaterial(pMaterial);
	AddComponent(pModel);
}