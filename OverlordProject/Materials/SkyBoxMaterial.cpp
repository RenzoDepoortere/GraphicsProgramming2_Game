#include "stdafx.h"
#include "SkyBoxMaterial.h"

SkyBoxMaterial::SkyBoxMaterial()
	: Material<SkyBoxMaterial>(L"Effects/SkyBox.fx")
{
}

void SkyBoxMaterial::SetSkyBoxTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pSkyBoxTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"m_CubeMap", m_pSkyBoxTexture);
}

void SkyBoxMaterial::InitializeEffectVariables()
{
}