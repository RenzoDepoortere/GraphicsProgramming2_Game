#include "stdafx.h"
#include "PauseMenu.h"

#include "Scenes/HarryPotterScene.h"

void PauseMenu::Initialize(const SceneContext& sceneContext)
{
	const XMFLOAT4 disable{ 1.f, 1.f, 1.f, 0.f };

	// Background
	// ----------
	
	auto pObject = AddChild(new GameObject{});
	m_pBackground = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/BlackScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pBackground->SetColor(disable);
	
	m_pBackground->GetTransform()->Translate(sceneContext.windowWidth / 2, sceneContext.windowHeight / 2, .9f);
	m_pBackground->GetTransform()->Scale(100.f);

	// Buttons
	// -------
	//const float distanceBetween{ 25.f };
	const XMFLOAT3 buttonScale{ 0.2f, 0.1f, 1.f };
	m_ButtonSize = {230.4f,  64.8f };
	XMFLOAT3 position{};

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	// Restart
	pObject = AddChild(new GameObject{});
	m_pRestart = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/WhiteScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pRestart->SetColor(disable);

	position = XMFLOAT3{ sceneContext.windowWidth / 2, sceneContext.windowHeight / 2, .8f };
	m_pRestart->GetTransform()->Translate(position);
	m_pRestart->GetTransform()->Scale(buttonScale);

	auto pair = std::make_pair(L"Restart", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4});
	m_FontData.emplace_back(pair);
}

void PauseMenu::Update(const SceneContext& sceneContext)
{
	// Return if not active
	if (m_IsActive == false) return;

	HandleText();
	HandleButtons(sceneContext);
}

void PauseMenu::SetActive(bool isActive)
{
	m_IsActive = isActive;

	if (isActive)
	{
		// Show pauseMenu
		m_pBackground->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.5f });

		const XMFLOAT4 red{ 0.8f, 0.f, 0.f, 0.5f };
		m_pRestart->SetColor(red);
	}
	else
	{
		// Disable pauseMenu
		const XMFLOAT4 disable{ 1.f, 1.f, 1.f, 0.f };

		m_pBackground->SetColor(disable);

		m_pRestart->SetColor(disable);
	}
}

void PauseMenu::HandleText()
{
	const XMFLOAT4 white{ 1.f, 1.f, 1.f, 1.f };
	for (const auto& currentData : m_FontData)
	{
		TextRenderer::Get()->DrawText(m_pFont, currentData.first, currentData.second, white);
	}
}
void PauseMenu::HandleButtons(const SceneContext& sceneContext)
{
	// Variables
	// ---------
	const float left{ sceneContext.windowWidth / 2 - m_ButtonSize.x / 2 };
	const float right{ sceneContext.windowWidth / 2 + m_ButtonSize.x / 2 };

	const POINT mousePos{ InputManager::GetMousePosition() };
	const bool mousePressed{ InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) };

	const XMFLOAT4 normalColor{ 0.8f, 0.f, 0.f, 0.5f };
	const XMFLOAT4 selectedColor{ 0.8f, 0.f, 0.f, 0.8f };

	// Restart
	// -------

	// If inside range
	if (IsInsideRange(left, right, 0, mousePos))
	{
		// Set select color
		m_pRestart->SetColor(selectedColor);

		// Restart
		if (mousePressed)
		{
			dynamic_cast<HarryPotterScene*>(GetScene())->RestartLevel();
		}

		return;
	}
	// Else, color to normal
	else m_pRestart->SetColor(normalColor);
}

bool PauseMenu::IsInsideRange(float left, float right, int index, const POINT& mousePos)
{
	const XMFLOAT2 textCenter{ m_FontData[index].second };
	const float bottom{ textCenter.y - m_ButtonSize.y / 2 };
	const float top{ textCenter.y + m_ButtonSize.y / 2 };

	const bool isInsideX{ left <= mousePos.x && mousePos.x <= right };
	const bool isInsideY{ bottom <= mousePos.y && mousePos.y <= top };

	return isInsideX && isInsideY;
}