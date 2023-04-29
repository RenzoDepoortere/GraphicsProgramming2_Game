#include "stdafx.h"
#include "DiffuseMaterial.h"

DiffuseMaterial::DiffuseMaterial()
	: Material<DiffuseMaterial>(L"Effects/PosNormTex3D.fx")
{

}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}
void DiffuseMaterial::SetDiffuseTexture(TextureData* pTextureData)
{
	// Set texture
	m_pDiffuseTexture = pTextureData;

	// Set variables
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial::InitializeEffectVariables()
{

}