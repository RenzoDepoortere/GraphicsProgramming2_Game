#include "stdafx.h"
#include "MomentarilyParticlePrefab.h"

MomentarilyParticlePrefab::MomentarilyParticlePrefab(const std::wstring& particleTexturePath, float stayDuration)
	: m_ParticleTexturePath{ particleTexturePath }
	, m_StayDuration{ stayDuration }
{
}

void MomentarilyParticlePrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	// Particles
	// ---------
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f, 0.f, 0.f };
	settings.minSize = 0.4f;
	settings.maxSize = 0.6f;
	settings.minEnergy = 0.7f;
	settings.maxEnergy = 1.f;
	settings.minScale = 1.f;
	settings.maxScale = 1.f;
	settings.minEmitterRadius = -1.f;
	settings.maxEmitterRadius = 1.f;
	settings.color = { 1.f, 1.f, 1.f, .6f };

	m_StartParticleCount = 100;
	m_pEmitter = AddComponent(new ParticleEmitterComponent(m_ParticleTexturePath, settings, m_StartParticleCount));
}

void MomentarilyParticlePrefab::Update(const SceneContext& sceneContext)
{
	// Countdown
	m_ElapsedTime += sceneContext.pGameTime->GetElapsed();

	// Lower nr particles
	const float ratio{ m_ElapsedTime / m_StayDuration };
	m_pEmitter->SetParticleCount( static_cast<UINT>(m_StartParticleCount * ratio));

	// Remove self
	if (m_StayDuration <= m_ElapsedTime)
	{
		GetScene()->RemoveChild(this, true);
	}
}