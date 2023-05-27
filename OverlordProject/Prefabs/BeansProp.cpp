#include "stdafx.h"
#include "BeansProp.h"

#include "Components/BeansCastableComponent.h"

#include "Prefabs/CubePrefab.h"

BeansProp::BeansProp(float generalScale, CastableComponent::Spell spell)
	: m_GeneralScale{ generalScale }
	, m_Spell{ spell }
{
}

void BeansProp::Initialize(const SceneContext& /*sceneContext*/)
{
	// castableComponent
	AddComponent(new BeansCastableComponent{ m_Spell, m_GeneralScale });

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
	pActor->AddCollider(PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial);

	// Visual box
	AddChild(new CubePrefab{});
}