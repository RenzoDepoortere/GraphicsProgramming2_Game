#include "stdafx.h"
#include "HUD.h"

#include "Materials/BasicMaterial_Deferred.h"

void HUD::Initialize(const SceneContext& /*sceneContext*/)
{
	// Health


	//// Beans
	//m_pBeanHUD = new GameObject{};
	//m_pBeanHUD->AddComponent(new SpriteComponent(L"Textures/HUD/Bean.png"));
	//AddChild(m_pBeanHUD);

	//m_pBeanHUD->GetTransform()->Translate(0.f, 100.f, .9f);
	//m_pBeanHUD->GetTransform()->Scale(0.5f);
}

void HUD::SetHP(int /*hpAmount*/)
{
	
}
void HUD::AddBean()
{
	++m_NrBeans;

}