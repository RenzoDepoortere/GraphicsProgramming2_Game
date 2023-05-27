#include "stdafx.h"
#include "HarryPotterScene.h"

// Character
#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"

// Map
#include "Prefabs/SkyBox.h"
#include "Prefabs/CastlePrefab.h"

// Props
#include "Prefabs/PropsPrefab.h"

void HarryPotterScene::Initialize()
{
	// SceneSettings
	// -------------
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;


	// Spawn Prefabs
	// -------------
	const float generalScale{ 0.025f };

	// Character
	m_pHarry = AddChild(new HarryCharacter(generalScale));

	// Map
	AddChild(new CastlePrefab(generalScale));
	AddChild(new SkyBox());

	// Props
	AddChild(new PropsPrefab(generalScale));
}

void HarryPotterScene::Update()
{
	// Hide and set mouse
	//m_SceneContext.pInput->ForceMouseToCenter(true);
	//m_SceneContext.pInput->CursorVisible(false);
}

void HarryPotterScene::OnGUI()
{
	m_pHarry->GetCharacter()->DrawImGui();
}