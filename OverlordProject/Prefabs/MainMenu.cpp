#include "stdafx.h"
#include "MainMenu.h"

#include "Scenes/HarryPotterScene.h"

MainMenu::MainMenu(bool isActive)
	: m_IsActive{ isActive }
{
}

void MainMenu::Initialize(const SceneContext& sceneContext)
{
	// Background
	// ----------

	auto pObject = AddChild(new GameObject{});
	m_pBackground = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/MainMenu.png", XMFLOAT2{ 0.5f, 0.5f }));

	m_pBackground->GetTransform()->Translate(sceneContext.windowWidth / 2, sceneContext.windowHeight / 2, 0.9f);
	if (m_IsActive == false) m_pBackground->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.f });

	// Buttons
	// -------
	const XMFLOAT3 buttonScale{ 0.2f, 0.1f, 1.f };
	m_ButtonSize = { 230.4f,  64.8f };
	//const float distanceBetween{ m_ButtonSize.y + 50.f };

	m_LeftRightPos.x = sceneContext.windowWidth - m_ButtonSize.x * 1.5f - 20.f;
	m_LeftRightPos.y = m_LeftRightPos.x + m_ButtonSize.x;

	XMFLOAT3 position{};
	XMFLOAT4 defaultColor{ 0.f, 0.f, 0.7f, 0.5f };
	if (m_IsActive == false) defaultColor = XMFLOAT4{ 1.f, 1.f, 1.f, 0.f };

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	// Start
	pObject = AddChild(new GameObject{});
	m_pStart = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/WhiteScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pStart->SetColor(defaultColor);

	position = XMFLOAT3{ sceneContext.windowWidth - m_ButtonSize.x - 20.f, sceneContext.windowHeight / 2.f + m_ButtonSize.y + 20.f, .9f };
	m_pStart->GetTransform()->Translate(position);
	m_pStart->GetTransform()->Scale(buttonScale);

	auto pair = std::make_pair(L"Start", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4 });
	m_FontData.emplace_back(pair);
}

void MainMenu::Update(const SceneContext& sceneContext)
{
	// Return if not active
	if (m_IsActive == false) return;

	HandleText();
	HandleButtons(sceneContext);
}

void MainMenu::SetActive(bool isActive)
{
	m_IsActive = isActive;

	if (isActive)
	{
		// Show mainMenu
		const XMFLOAT4 defaultColor{ 0.f, 0.f, 0.7f, 0.5f };

		m_pBackground->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 1.f });

		m_pStart->SetColor(defaultColor);
		//m_pRestart->SetColor(red);
		//m_pExit->SetColor(red);
	}
	else
	{
		// Disable mainMenu
		const XMFLOAT4 disable{ 1.f, 1.f, 1.f, 0.f };

		m_pBackground->SetColor(disable);

		m_pStart->SetColor(disable);
		//m_pRestart->SetColor(disable);
		//m_pExit->SetColor(disable);
	}
}

void MainMenu::HandleText()
{
	const XMFLOAT4 white{ 1.f, 1.f, 1.f, 1.f };
	for (const auto& currentData : m_FontData)
	{
		TextRenderer::Get()->DrawText(m_pFont, currentData.first, currentData.second, white);
	}
}
void MainMenu::HandleButtons(const SceneContext& /*sceneContext*/)
{
	// Variables
	// ---------
	const POINT mousePos{ InputManager::GetMousePosition() };
	const bool mousePressed{ InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) };

	const XMFLOAT4 normalColor{ 0.f, 0.f, 0.7f, 0.5f };
	const XMFLOAT4 selectedColor{ 0.f, 0.f, 0.7f, 0.8f };

	// Start
	// -----
	
	// If inside range
	if (IsInsideRange(m_LeftRightPos.x, m_LeftRightPos.y, 0, mousePos))
	{
		// Set select color
		m_pStart->SetColor(selectedColor);

		// Start game
		if (mousePressed)
		{
			dynamic_cast<HarryPotterScene*>(GetScene())->SetMainMenu(false);
		}

		return;
	}
	// Else, color to normal
	else m_pStart->SetColor(normalColor);
}

bool MainMenu::IsInsideRange(float left, float right, int index, const POINT& mousePos)
{
	const XMFLOAT2 textCenter{ m_FontData[index].second };
	const float bottom{ textCenter.y - m_ButtonSize.y / 2 };
	const float top{ textCenter.y + m_ButtonSize.y / 2 };

	const bool isInsideX{ left <= mousePos.x && mousePos.x <= right };
	const bool isInsideY{ bottom <= mousePos.y && mousePos.y <= top };

	return isInsideX && isInsideY;
}