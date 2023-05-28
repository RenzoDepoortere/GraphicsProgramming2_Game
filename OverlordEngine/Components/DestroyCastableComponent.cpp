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

// (Can't get the gameObject to be deleted, so objects will be set to origin instead)
void DestroyCastableComponent::Activate(GameObject* /*pHarry*/)
{
	// If already activated, return
	if (m_HasBeenActivated) return;
	m_HasBeenActivated = true;

	// Spawn particles


	//// Delete
	//GetGameObject()->GetParent()->RemoveChild(GetGameObject(), true);

	// Set object to origin
	GetTransform()->Translate(XMFLOAT3{});

	// Set collision to origin 
	//m_pRigidBodyComponent->Translate({});
}