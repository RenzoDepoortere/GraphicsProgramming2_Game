#include "stdafx.h"
#include "MainMenu.h"

#include "Scenes/HarryPotterScene.h"

MainMenu::MainMenu(bool isActive)
	: m_IsActive{ isActive }
{
}

void MainMenu::Initialize(const SceneContext& sceneContext)
{
	// Sound
	// -----
	m_pFmod = SoundManager::Get()->GetSystem();

	// Music
	FMOD::Sound* pSound{ nullptr };
	m_pFmod->createStream("Resources/Sounds/Music/MainMenu_Music.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &pSound);
	m_pFmod->playSound(pSound, nullptr, true, &m_pBackgroundChannel);

	if (m_IsActive) m_pBackgroundChannel->setPaused(false);

	// Effects
	m_pFmod->createSound("Resources/Sounds/UI/UI_Hoover.wav", FMOD_DEFAULT, nullptr, &m_pSelectSound);

	// Background
	// ----------

	auto pObject = AddChild(new GameObject{});
	m_pBackground = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/MainMenu.png", XMFLOAT2{ 0.5f, 0.5f }));

	m_pBackground->GetTransform()->Translate(sceneContext.windowWidth / 2, sceneContext.windowHeight / 2, 0.7f);
	if (m_IsActive == false) m_pBackground->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.f });

	// Buttons
	// -------
	const XMFLOAT3 buttonScale{ 0.2f, 0.1f, 1.f };
	m_ButtonSize = { 230.4f,  64.8f };
	const float distanceBetween{ m_ButtonSize.y + 5.f };

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

	position = XMFLOAT3{ sceneContext.windowWidth - m_ButtonSize.x - 20.f, sceneContext.windowHeight / 2.f + m_ButtonSize.y + 20.f, .7f };
	m_pStart->GetTransform()->Translate(position);
	m_pStart->GetTransform()->Scale(buttonScale);

	auto pair = std::make_pair(L"Start", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4 });
	m_FontData.emplace_back(pair);

	// Controls
	pObject = AddChild(new GameObject{});
	m_pControls = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/WhiteScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pControls->SetColor(defaultColor);

	position.y += distanceBetween;
	m_pControls->GetTransform()->Translate(position);
	m_pControls->GetTransform()->Scale(buttonScale);

	pair = std::make_pair(L"Controls", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4 });
	m_FontData.emplace_back(pair);

	// Exit
	pObject = AddChild(new GameObject{});
	m_pExit = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/WhiteScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pExit->SetColor(defaultColor);

	position.y += distanceBetween;
	m_pExit->GetTransform()->Translate(position);
	m_pExit->GetTransform()->Scale(buttonScale);

	pair = std::make_pair(L"Exit", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4 });
	m_FontData.emplace_back(pair);


	// Controls
	// --------

	// Background
	pObject = AddChild(new GameObject{});
	m_pControlsBG = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/Controls.png", XMFLOAT2{ 0.5f, 0.5f }));

	m_pControlsBG->GetTransform()->Translate(sceneContext.windowWidth / 2, sceneContext.windowHeight / 2, 0.8f);
	m_pControlsBG->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.f });

	// Button
	m_ControlLeftRightPos.x = m_ButtonSize.x / 2 + 20.f;
	m_ControlLeftRightPos.y = m_ControlLeftRightPos.x + m_ButtonSize.x;

	pObject = AddChild(new GameObject{});
	m_pControlButton = pObject->AddComponent(new SpriteComponent(L"Textures/Menu/WhiteScreen.png", XMFLOAT2{ 0.5f, 0.5f }));
	m_pControlButton->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.f });

	position = XMFLOAT3{m_ControlLeftRightPos.x , sceneContext.windowHeight - m_ButtonSize.y - 20.f, 0.9f };
	m_pControlButton->GetTransform()->Translate(position);
	m_pControlButton->GetTransform()->Scale(buttonScale);

	pair = std::make_pair(L"Back", XMFLOAT2{ position.x - m_ButtonSize.x / 4, position.y - m_ButtonSize.y / 4 });
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
	const XMFLOAT4 disable{ 1.f, 1.f, 1.f, 0.f };
	const XMFLOAT4 defaultColor{ 0.f, 0.f, 0.7f, 0.5f };

	if (isActive)
	{
		// Show mainMenu
		m_pBackground->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 1.f });

		m_pStart->SetColor(defaultColor);
		m_pControls->SetColor(defaultColor);
		m_pExit->SetColor(defaultColor);

		// Play music
		m_pBackgroundChannel->setPaused(false);
		m_pSoundEffectChannel->setPaused(false);
	}
	else
	{
		// Disable mainMenu
		m_pBackground->SetColor(disable);

		m_pStart->SetColor(disable);
		m_pControls->SetColor(disable);
		m_pExit->SetColor(disable);

		// Stop music
		m_pBackgroundChannel->setPaused(true);
		m_pSoundEffectChannel->setPaused(true);
	}
	m_IsActive = isActive;

	if (m_InControls && isActive)
	{
		m_pControlButton->SetColor(defaultColor);
	}
	else
	{
		m_pControlButton->SetColor(disable);
	}
}

void MainMenu::HandleText()
{
	const XMFLOAT4 white{ 1.f, 1.f, 1.f, 1.f };

	if (m_InControls)
	{
		const auto currentData{ m_FontData[m_FontData.size() - 1] };
		TextRenderer::Get()->DrawText(m_pFont, currentData.first, currentData.second, white);
		return;
	}

	for (int idx{}; idx < m_FontData.size() - 1; ++idx)
	{
		const auto currentData{ m_FontData[idx] };
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

	// Controls
	// --------
	
	if (m_InControls)
	{
		// If inside range
		if (IsInsideRange(m_ControlLeftRightPos.x, m_ControlLeftRightPos.y, 3, mousePos, true))
		{
			// Set select color
			m_pControlButton->SetColor(selectedColor);

			// Disable controls
			if (mousePressed)
			{
				SetControls(false);
			}

			return;
		}
		// Else, color to normal
		else
		{
			m_PlayedSound = false;
			m_pControlButton->SetColor(normalColor);
		}

		// Return
		return;
	}


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


	// Controls
	// --------

	// If inside range
	if (IsInsideRange(m_LeftRightPos.x, m_LeftRightPos.y, 1, mousePos))
	{
		// Set select color
		m_pControls->SetColor(selectedColor);

		// Show controls
		if (mousePressed)
		{
			SetControls(true);
		}

		return;
	}
	// Else, color to normal
	else m_pControls->SetColor(normalColor);


	// Exit
	// ----

	// If inside range
	if (IsInsideRange(m_LeftRightPos.x, m_LeftRightPos.y, 2, mousePos))
	{
		// Set select color
		m_pExit->SetColor(selectedColor);

		// Exit
		if (mousePressed)
		{
			GetScene()->CloseGame();
		}

		return;
	}
	// Else, color to normal
	else m_pExit->SetColor(normalColor);

	// Nothing selected
	m_PlayedSound = false;
}

bool MainMenu::IsInsideRange(float left, float right, int index, const POINT& mousePos, bool lowerWithHalf)
{
	const XMFLOAT2 textCenter{ m_FontData[index].second };
	float bottom{ textCenter.y - m_ButtonSize.y / 2 };
	float top{ textCenter.y + m_ButtonSize.y / 2 };

	if (lowerWithHalf)
	{
		left -= m_ButtonSize.x / 2;
		right -= m_ButtonSize.x / 2;

		bottom += m_ButtonSize.y / 2;
		top += m_ButtonSize.y / 2;
	}

	const bool isInsideX{ left <= mousePos.x && mousePos.x <= right };
	const bool isInsideY{ bottom <= mousePos.y && mousePos.y <= top };

	if (isInsideX && isInsideY)
	{
		// Play sound
		if (m_PlayedSound == false)
		{
			m_PlayedSound = true;
			m_pFmod->playSound(m_pSelectSound, nullptr, false, &m_pSoundEffectChannel);
		}

		return true;
	}

	return false;
}

void MainMenu::SetControls(bool isActive)
{
	if (isActive)
	{
		m_InControls = true;

		// Show BG and button
		const XMFLOAT4 defaultColor{ 0.f, 0.f, 0.7f, 0.5f };

		m_pControlsBG->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 1.f });
		m_pControlButton->SetColor(defaultColor);
	}
	else
	{
		m_InControls = false;

		// Disable BG and button
		const XMFLOAT4 disable{ 1.f, 1.f, 1.f, 0.f };

		m_pControlsBG->SetColor(disable);
		m_pControlButton->SetColor(disable);
	}
}