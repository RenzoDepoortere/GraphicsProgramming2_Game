#include "stdafx.h"
#include "HUD.h"

#include "Materials/BasicMaterial_Deferred.h"

void HUD::Initialize(const SceneContext& /*sceneContext*/)
{
	// (L"Textures/HUD/Bean_HUD.png"

	// Beans
	m_pBeanMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	m_pBeanMaterial->SetDiffuseMap(L"Textures/HUD/TestTennisBall.jpg");
}
void HUD::Draw(const SceneContext& /*sceneContext*/)
{
	Quad drawQuad{};
	drawQuad.width = 50;
	drawQuad.height = 50;

	QuadRenderer::Get()->Draw(m_pBeanMaterial, drawQuad);
}

void HUD::SetHP(int /*hpAmount*/)
{
	
}
void HUD::AddBean()
{
	++m_NrBeans;

}