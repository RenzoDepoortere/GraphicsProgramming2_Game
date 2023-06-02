#include "stdafx.h"
#include "HUD.h"

#include "Materials/BasicMaterial_Deferred.h"

HUD::HUD(int maxHP)
	: m_MaxHP{ maxHP }
{
}

void HUD::Initialize(const SceneContext& /*sceneContext*/)
{
	// Health
	// ------
	GameObject* pObject{};
	SpriteComponent* pComponent{};

	const std::wstring fullHealthString{L"Textures/HUD/Health.png"};
	const float textureWidth{ 64.f };

	for (int idx{}; idx < m_MaxHP / 2; ++idx)
	{
		// Create object
		pObject = AddChild(new GameObject{});
		pComponent = pObject->AddComponent(new SpriteComponent(fullHealthString));
		
		// Translate
		pObject->GetTransform()->Translate(textureWidth / 2.f * idx, 10.f, 0.7f);

		// Add to vector
		m_pHealthIcons.emplace_back(pComponent);
	}

	// Beans
	// -----

	// Icon
	pObject = AddChild(new GameObject{});
	m_pBeanHUD = pObject->AddComponent(new SpriteComponent(L"Textures/HUD/Bean.png"));
	
	m_pBeanHUD->SetColor(XMFLOAT4{ 1.f, 1.f, 1.f, 0.f });

	m_pBeanHUD->GetTransform()->Translate(0.f, 100.f, .7f);
	m_pBeanHUD->GetTransform()->Scale(0.5f);

	// Text
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
	m_FontPosition = XMFLOAT2{ 40.f, 150.f };
}
void HUD::Update(const SceneContext& sceneContext)
{
	// If no bean gained, return
	if (m_BeanGained == false) return;
	
	ChangeBeanIcon(sceneContext);
	ChangeBeanText();
}


void HUD::SetHP(int hpAmount)
{
	// Calculate full and half hearts
	const int remainingFullHearts{ hpAmount / 2 };
	const bool hasHalfHeart{ static_cast<bool>(hpAmount % 2) };

	// If half left, change full into half
	const std::wstring halfHealthString{ L"Textures/HUD/HalfHealth.png" };
	if (hasHalfHeart)
	{
		m_pHealthIcons[remainingFullHearts]->SetTexture(halfHealthString);
	}

	// "Delete" all hearts from start index	
	const size_t startIdxToDelete = hasHalfHeart ? remainingFullHearts + 1 : remainingFullHearts;
	if (startIdxToDelete < m_pHealthIcons.size())
	{
		for (size_t idx{ startIdxToDelete }; idx < m_pHealthIcons.size(); ++idx)
			m_pHealthIcons[idx]->GetTransform()->Scale(0.f);
	}
}
void HUD::AddBean()
{
	// Add bean
	++m_NrBeans;

	// Prepare opacity change
	m_BeanGained = true;
	m_Dissapearing = false;
	m_HoldTime = 0.f;
}

void HUD::ChangeBeanIcon(const SceneContext& sceneContext)
{
	const float deltaTime{ sceneContext.pGameTime->GetElapsed() };

	// If holdTime
	if (0 < m_HoldTime)
	{
		// Decrease and return
		m_HoldTime -= deltaTime;
		return;
	}

	// Calculate new opacity
	// ---------------------
	XMFLOAT4 currentColor{ m_pBeanHUD->GetColor() };

	float newOpacity{};
	float changeSpeed{ 0.4f };

	// If dissapearing
	if (m_Dissapearing)
	{
		// New opacity
		newOpacity = currentColor.w - changeSpeed * deltaTime;

		// If full opacity
		if (newOpacity <= 0.f)
		{
			// Clamp and stop change
			newOpacity = 0.f;

			m_BeanGained = false;
		}
	}
	// If appearing
	else
	{
		// New opacity
		changeSpeed *= 4.f;
		newOpacity = currentColor.w + changeSpeed * deltaTime;

		// If fully opaque
		if (1.f <= newOpacity)
		{
			// Clamp and hold for a moment
			newOpacity = 1.f;

			m_HoldTime = 2.f;
			m_Dissapearing = true;
		}
	}

	// Set opacity
	currentColor.w = newOpacity;
	m_pBeanHUD->SetColor(currentColor);
}
void HUD::ChangeBeanText()
{
	TextRenderer::Get()->DrawText(m_pFont, std::to_wstring(m_NrBeans), m_FontPosition, m_pBeanHUD->GetColor());
}