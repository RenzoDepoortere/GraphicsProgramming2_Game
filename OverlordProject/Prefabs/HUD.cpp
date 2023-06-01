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
	const std::wstring halfHealthString{L"Textures/HUD/HalfHealth.png"};

	const float textureWidth{ 64.f };

	for (int idx{}; idx < nrHealthIcons; ++idx)
	{
		// Create object
		pObject = AddChild(new GameObject{});
		pComponent = pObject->AddComponent(new SpriteComponent(fullHealthString));
		
		// Translate
		pObject->GetTransform()->Translate(textureWidth * idx, 0.f, idx / 10.f);

		// Add to vector
		m_pHealthIcons.emplace_back(pComponent);
	}

	//// Beans
	//m_pBeanHUD = AddChild(new GameObject{});
	//m_pBeanHUD->AddComponent(new SpriteComponent(L"Textures/HUD/Bean.png"));

	//m_pBeanHUD->GetTransform()->Translate(0.f, 100.f, .9f);
	//m_pBeanHUD->GetTransform()->Scale(0.5f);
}
//void HUD::Draw(const SceneContext& sceneContext)
//{
//	for (auto& currentSprite : m_pHealthIcons)
//	{
//		SpriteRenderer::Get()->DrawImmediate()
//	}
//}

void HUD::SetHP(int /*hpAmount*/)
{
	
}
void HUD::AddBean()
{
	++m_NrBeans;

}