#include "stdafx.h"
#include "Torch.h"

Torch::Torch(float generalScale)
	: m_GeneralScale{ generalScale }
{
}

void Torch::Initialize(const SceneContext& /*sceneContext*/)
{
	// Particles
	// ---------
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f, 1.f, 0.f };
	settings.minSize = 0.4f;
	settings.maxSize = 0.6f;
	settings.minEnergy = 0.7f;
	settings.maxEnergy = 1.f;
	settings.minScale = 0.6f;
	settings.maxScale = 0.8f;
	settings.minEmitterRadius = -0.1f;
	settings.maxEmitterRadius = 0.1f;
	settings.color = { 1.f, 1.f, 1.f, .6f };

	m_pEmitter = AddComponent(new ParticleEmitterComponent(L"Textures/Particles/Fire/Fire.png", settings, 100));
}