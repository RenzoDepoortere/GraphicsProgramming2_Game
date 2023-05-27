#include "stdafx.h"
#include "ParticleMaterial.h"

ParticleMaterial::ParticleMaterial():
Material(L"Effects/ParticleRenderer.fx")
{
}

void ParticleMaterial::InitializeEffectVariables()
{
}

void ParticleMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gParticleTexture", m_pTexture);
}