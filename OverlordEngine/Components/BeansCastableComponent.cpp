#include "stdafx.h"
#include "BeansCastableComponent.h"

#include "../OverlordProject/Prefabs/Bean.h"

BeansCastableComponent::BeansCastableComponent(float generalScale, GameObject* pHarry, Spell spell)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, CastableComponent{ spell }
{
}

void BeansCastableComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	// Set defaults
	m_MinNrBeans = 3;
	m_MaxNrBeans = 10;

	m_TimeBetweenBeans = 0.5f;

	m_MaxRandomShootAngle = 5.f;
}

void BeansCastableComponent::Update(const SceneContext& sceneContext)
{
	if (m_SpawnBeans == false) return;

	// Lower timer
	m_CurrentTimer -= sceneContext.pGameTime->GetElapsed();
	if (m_CurrentTimer <= 0)
	{
		m_CurrentTimer = m_TimeBetweenBeans;

		// Calculate random direction

		// Particles

		// Spawn bean
		GetGameObject()->AddChild(new Bean{ m_GeneralScale, m_pHarry, m_PreviousLocation, MathHelper::DirectionTo(m_PreviousLocation, m_pHarry->GetTransform()->GetWorldPosition()) });

		// One bean less
		--m_NrBeansToSpawn;
		if (m_NrBeansToSpawn == 0) m_SpawnBeans = false;
	}
}

void BeansCastableComponent::Activate(GameObject* pHarry)
{
	// If already activated, return
	if (m_HasBeenActivated) return;
	m_HasBeenActivated = true;

	// Spawn random nr beans
	m_NrBeansToSpawn = rand() % (m_MaxNrBeans - m_MinNrBeans + 1) + m_MinNrBeans;
	m_SpawnBeans = true;

	// Get position
	m_pHarry = pHarry;
	m_PreviousLocation = GetGameObject()->GetTransform()->GetWorldPosition();
}