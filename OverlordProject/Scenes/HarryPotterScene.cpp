#include "stdafx.h"
#include "HarryPotterScene.h"

// Character
#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"

// Map
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

	// Props
	AddChild(new PropsPrefab(generalScale, m_pHarry->GetCharacter()));
}

void HarryPotterScene::Update()
{
	// Hide and set mouse
	//m_SceneContext.pInput->ForceMouseToCenter(true);
	//m_SceneContext.pInput->CursorVisible(false);
}

void HarryPotterScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
	if (m_DrawShadowMap) {

		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void HarryPotterScene::OnGUI()
{
	// Harry
	m_pHarry->GetCharacter()->DrawImGui();
	m_pHarry->GetParticleEmitter()->DrawImGui();

	// Shadow
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);
}