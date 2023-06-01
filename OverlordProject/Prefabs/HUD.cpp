#include "stdafx.h"
#include "HUD.h"

#include "Materials/BasicMaterial_Deferred.h"

void HUD::Initialize(const SceneContext& /*sceneContext*/)
{
	// Health
	const int nrHealthIcons{ 3 };
	GameObject* pObject{};
	SpriteComponent* pComponent{};

	const std::wstring fullHealthString{L"Textures/HUD/Health.png"};
	const float textureWidth{ 64.f };

	for (int idx{}; idx < nrHealthIcons; ++idx)
	{
		// Create object
		pObject = AddChild(new GameObject{});
		pComponent = pObject->AddComponent(new SpriteComponent(fullHealthString));
		
		// Translate
		pObject->GetTransform()->Translate(textureWidth / 2.f * idx, 10.f, 0.9f);

		// Add to vector
		m_pHealthIcons.emplace_back(pComponent);
	}

	//// Beans
	//m_pBeanHUD = AddChild(new GameObject{});
	//m_pBeanHUD->AddComponent(new SpriteComponent(L"Textures/HUD/Bean.png"));

	//m_pBeanHUD->GetTransform()->Translate(0.f, 100.f, .9f);
	//m_pBeanHUD->GetTransform()->Scale(0.5f);
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
	++m_NrBeans;

}