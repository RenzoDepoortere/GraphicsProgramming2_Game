#include "stdafx.h"
#include "HarryPotterScene.h"

// Character
#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"

// Enemies
#include "Prefabs/EnemiesPrefab.h"

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
	m_SceneContext.useDeferredRendering = true;

	// Mouse settings
	// --------------
	m_SceneContext.pInput->ForceMouseToCenter(true);
	m_SceneContext.pInput->CursorVisible(false);

	// Spawn Prefabs
	// -------------
	const float generalScale{ 0.025f };

	// Character
	m_pHarry = AddChild(new HarryCharacter(generalScale));

	// Enemies
	AddChild(new EnemiesPrefab(generalScale, m_pHarry));

	// Map
	AddChild(new CastlePrefab(generalScale));

	// Props
	AddChild(new PropsPrefab(generalScale, m_pHarry->GetCharacter()));
}

void HarryPotterScene::Update()
{
	// Toggle hide and set mouse
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, '1'))
	{
		m_CenterMouse = !m_CenterMouse;
		m_SceneContext.pInput->CursorVisible(!m_CenterMouse);
	}

	m_SceneContext.pInput->ForceMouseToCenter(m_CenterMouse);
}

void HarryPotterScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
	if (m_DrawShadowMap)
	{
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void HarryPotterScene::OnGUI()
{
	// Harry
	m_pHarry->GetCharacter()->DrawImGui();
	//m_pHarry->GetParticleEmitter()->DrawImGui();

	// Shadow
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	//ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);
}