#include "stdafx.h"
#include "DestroyCastableComponent.h"

#include "../OverlordProject/Prefabs/HarryCharacter.h"

DestroyCastableComponent::DestroyCastableComponent(Spell spell, HarryCharacter* pHarry)
	: CastableComponent{ spell }
	, m_pHarry{ pHarry }
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
	if (m_HasToDelete) GetScene()->RemoveChild(GetGameObject(), true);
}

void DestroyCastableComponent::Activate(GameObject* /*pHarry*/)
{
	// If deleting, return
	if (m_HasToDelete) return;
	m_HasToDelete = true;

	// Spawn particles

	// Sound
	const int randomIdx{ rand() % static_cast<int>(m_pDestroySounds.size()) };
	m_pHarry->SetSpellHitSoundToPlay(m_pDestroySounds[randomIdx]);
}