#include "stdafx.h"
#include "BeansCastableComponent.h"

#include "../OverlordProject/Prefabs/Bean.h"

BeansCastableComponent::BeansCastableComponent(Spell spell, float generalScale)
	: CastableComponent{ spell }
	, m_GeneralScale{ generalScale }
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

		// Spawn bean
		GetGameObject()->GetScene()->AddChild(new Bean{m_GeneralScale, m_PreviousLocation, m_ShootDirection});

		// One bean less
		--m_NrBeansToSpawn;
		if (m_NrBeansToSpawn == 0) m_SpawnBeans = false;
	}
}

void BeansCastableComponent::Activate(const XMFLOAT3& spellPosition)
{
	// If already activated, return
	if (m_HasBeenActivated) return;
	m_HasBeenActivated = true;

	// Spawn random nr beans
	m_NrBeansToSpawn = rand() % (m_MaxNrBeans - m_MinNrBeans + 1) + m_MinNrBeans;
	m_SpawnBeans = true;

	// DEBUG: SPAWN 1
	m_NrBeansToSpawn = 1;

	// Calculate ShootDirection
	const XMFLOAT3 desiredDirection{ MathHelper::DirectionTo(GetTransform()->GetPosition(), spellPosition) };

	//const XMFLOAT3 upDirection{ 0, 1, 0 };
	//const XMVECTOR upVector{ XMLoadFloat3(&upDirection) };


	XMStoreFloat3(&m_ShootDirection, XMLoadFloat3(&desiredDirection));
	m_PreviousLocation = GetGameObject()->GetTransform()->GetWorldPosition();

	// Delete
	//GetGameObject()->GetScene()->RemoveChild(GetGameObject(), true);

	// Set object to origin
	GetTransform()->Translate(XMFLOAT3{});
}