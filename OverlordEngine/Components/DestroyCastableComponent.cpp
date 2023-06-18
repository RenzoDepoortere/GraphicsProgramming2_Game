#include "stdafx.h"
#include "DestroyCastableComponent.h"

#include "../OverlordProject/Prefabs/HarryCharacter.h"
#include "../OverlordProject/Prefabs/MomentarilyParticlePrefab.h"

DestroyCastableComponent::DestroyCastableComponent(Spell spell, HarryCharacter* pHarry, GameObject* pPropsPrefab)
	: CastableComponent{ spell }
	, m_pHarry{ pHarry }
	, m_pPropsPrefab{ pPropsPrefab }
{
}

void DestroyCastableComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	// Sounds
	FMOD::System* pFmod{ SoundManager::Get()->GetSystem() };
	pFmod->createSound("Resources/Sounds/Props/Vines/Vines_1.wav", FMOD_DEFAULT, nullptr, &m_pDestroySounds[0]);
	pFmod->createSound("Resources/Sounds/Props/Vines/Vines_2.wav", FMOD_DEFAULT, nullptr, &m_pDestroySounds[1]);

	// Rigidbody
	m_pRigidBodyComponent = GetGameObject()->GetComponent<RigidBodyComponent>(true);
}

void DestroyCastableComponent::Update(const SceneContext& /*sceneContext*/)
{
	if (m_HasToDelete) GetTransform()->Translate(0.f, 0.f, 0.f);
}

void DestroyCastableComponent::Activate(GameObject* /*pHarry*/)
{
	// If deleting, return
	if (m_HasToDelete) return;
	m_HasToDelete = true;

	// Spawn particles
	GameObject* pobject = m_pPropsPrefab->AddChild(new MomentarilyParticlePrefab{m_pPropsPrefab, L"Textures/Particles/Leaf/Leaf.png", 1.f });
	
	XMFLOAT3 spawnPos{ GetTransform()->GetWorldPosition() };
	spawnPos.y += 2.f;
	pobject->GetTransform()->Translate(spawnPos);

	// Sound
	const int randomIdx{ rand() % static_cast<int>(m_pDestroySounds.size()) };
	m_pHarry->SetSpellHitSoundToPlay(m_pDestroySounds[randomIdx]);
}