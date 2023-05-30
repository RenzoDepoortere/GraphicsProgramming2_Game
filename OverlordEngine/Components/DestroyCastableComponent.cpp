#include "stdafx.h"
#include "DestroyCastableComponent.h"

DestroyCastableComponent::DestroyCastableComponent(Spell spell)
	: CastableComponent{ spell }
{
}

void DestroyCastableComponent::Initialize(const SceneContext& /*sceneContext*/)
{
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

}