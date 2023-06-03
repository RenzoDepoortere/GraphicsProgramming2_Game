#include "stdafx.h"
#include "BeansCastableComponent.h"

#include "../OverlordProject/Prefabs/Bean.h"
#include "../OverlordProject/Prefabs/BeansProp.h"
#include "../OverlordProject/Prefabs/HarryCharacter.h"

BeansCastableComponent::BeansCastableComponent(float generalScale, HarryCharacter* pHarry, Spell spell, BeansProp* pProp)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, CastableComponent{ spell }
	, m_pProp{ pProp }
{
}

void BeansCastableComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	// Set defaults
	m_MinNrBeans = 3;
	m_MaxNrBeans = 10;

	m_TimeBetweenBeans = 0.5f;

	m_MaxRandomShootAngle = 5.f;

	// Create beanHolder
	m_pBeanHolder = GetScene()->AddChild(new GameObject{});
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
		const XMFLOAT3 currentPos{ GetTransform()->GetWorldPosition() };
		const XMFLOAT3 direction{ MathHelper::DirectionTo(currentPos, m_pHarry->GetTransform()->GetWorldPosition())};
		XMFLOAT3 spawnLocation{ currentPos };
		spawnLocation.y += 2.f;

		auto pObject = m_pBeanHolder->AddChild(new Bean{ m_GeneralScale, m_pHarry, spawnLocation, direction });
		pObject->GetTransform()->Translate(spawnLocation);

		// One bean less
		--m_NrBeansToSpawn;
		if (m_NrBeansToSpawn == 0) m_SpawnBeans = false;
	}
}

void BeansCastableComponent::Activate(GameObject* /*pHarry*/)
{
	// If already activated, return
	if (m_HasBeenActivated) return;
	m_HasBeenActivated = true;

	// Spawn random nr beans
	m_NrBeansToSpawn = rand() % (m_MaxNrBeans - m_MinNrBeans + 1) + m_MinNrBeans;
	m_SpawnBeans = true;

	// Get position
	m_PreviousLocation = GetGameObject()->GetTransform()->GetWorldPosition();

	// Set object to jump
	m_pProp->SetToJump();
}