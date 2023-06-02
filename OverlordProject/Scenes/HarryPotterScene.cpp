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

// Menu
#include "Prefabs/MainMenu.h"
#include "Prefabs/PauseMenu.h"

HarryPotterScene::HarryPotterScene()
	: GameScene(L"HarryPotterScene")
{
	// Main menu
	SetUpdateChildren(false);
}

void HarryPotterScene::Initialize()
{
	// SceneSettings
	// -------------
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.showInfoOverlay = false;
	m_SceneContext.useDeferredRendering = true;

	// Mouse settings
	// --------------
	if (m_InStartMenu == false)
	{
		m_SceneContext.pInput->ForceMouseToCenter(true);
		m_SceneContext.pInput->CursorVisible(false);
	}

	// Spawn Prefabs
	// -------------
	const float generalScale{ 0.025f };

	// Character
	m_pHarry = AddChild(new HarryCharacter(generalScale, m_InStartMenu));

	// Enemies
	AddChild(new EnemiesPrefab(generalScale, m_pHarry));

	// Map
	AddChild(new CastlePrefab(generalScale));

	// Props
	AddChild(new PropsPrefab(generalScale, m_pHarry));

	// Menu
	m_pMainMenu = AddChild(new MainMenu{ m_InStartMenu });
	m_pPauseMenu = AddChild(new PauseMenu{});
}

void HarryPotterScene::Update()
{
	HandleInput();
	HandlePauseMenu();
	HandleMainMenu();

	if (m_HasToReset) DeleteChildren();
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
	if (m_pHarry)
	{
		// Harry
		m_pHarry->GetCharacter()->DrawImGui();
		//m_pHarry->GetParticleEmitter()->DrawImGui();
	}

	// Shadow
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	//ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);
}

void HarryPotterScene::HandleInput()
{
	// No input when in start menu
	if (m_InStartMenu) return;

	// Toggle hide and set mouse, if not paused
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, '1') && m_IsPaused == false)
	{
		m_CenterMouse = !m_CenterMouse;
		m_SceneContext.pInput->CursorVisible(!m_CenterMouse);
	}

	// If pressed ESC
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, VK_ESCAPE) && m_pPauseMenu)
	{
		// If paused
		if (m_IsPaused)
		{
			// Center mouse
			m_CenterMouse = true;
			m_SceneContext.pInput->CursorVisible(false);

			// Unpause
			m_IsPaused = false;
			m_pPauseMenu->SetActive(false);

			// Let children know
			m_pHarry->InPauseMenu(false);

			// Continue updating
			SetUpdateChildren(true);
		}
		// Else
		else
		{
			// Free mouse
			m_CenterMouse = false;
			m_SceneContext.pInput->CursorVisible(true);

			// Pause
			m_IsPaused = true;
			m_pPauseMenu->SetActive(true);

			// Let children know
			m_pHarry->InPauseMenu(true);

			// Stop updating
			SetUpdateChildren(false);
		}
	}

	// Force if mouse is centered
	m_SceneContext.pInput->ForceMouseToCenter(m_CenterMouse);
}

void HarryPotterScene::HandlePauseMenu()
{
	if (m_IsPaused == false) return;
	if (m_pPauseMenu == nullptr) return;

	// Manually update when game is paused
	m_pPauseMenu->Update(m_SceneContext);
}
void HarryPotterScene::HandleMainMenu()
{
	if (m_InStartMenu == false) return;
	if (m_pMainMenu == nullptr) return;

	// Manually update when game is paused
	m_pMainMenu->Update(m_SceneContext);
}

void HarryPotterScene::DeleteChildren()
{
	// Delete children
	ClearScene();
	m_pHarry = nullptr;
	m_pMainMenu = nullptr;
	m_pPauseMenu = nullptr;

	// Re-Initialize
	Initialize();

	// Reset bool
	m_HasToReset = false;
}

void HarryPotterScene::SetMainMenu(bool goToMenu)
{
	// If going to menu
	if (goToMenu)
	{
		// Let children know
		m_pHarry->InMainMenu();

		// Disable pauseMenu
		m_IsPaused = false;
		m_pPauseMenu->SetActive(false);
		
		m_CenterMouse = false;
		m_SceneContext.pInput->CursorVisible(true);

		// Pause game and enable menu
		SetUpdateChildren(false);

		m_InStartMenu = true;
		m_pMainMenu->SetActive(true);
	}
	// Else
	else
	{
		// Start game and disable menu
		m_InStartMenu = false;
		m_pMainMenu->SetActive(false);
		m_CenterMouse = true;

		// Restart level
		RestartLevel();
	}
}
void HarryPotterScene::RestartLevel()
{
	m_HasToReset = true;

	m_IsPaused = false;
	SetUpdateChildren(true);
}