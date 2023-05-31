#include "stdafx.h"
#include "Snail.h"

#include "Materials/BasicMaterial_Deferred.h"

#include "Components/SnailCastableComponent.h"

#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"
#include "Prefabs/Trail.h"

#include <algorithm>

Snail::Snail(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{generalScale}
	, m_pHarry{ pHarry }
	, m_CurrentGoalPosition{ 1 }
{
}

void Snail::Initialize(const SceneContext& /*sceneContext*/)
{
	// Trail
	// -----
	m_pTrailObject = GetScene()->AddChild(new GameObject{});

	// Snail
	// -----
	
	// Castable
	m_pCastableComponent = AddComponent(new SnailCastableComponent{ CastableComponent::Rictusempra, CastableComponent::Spongify, this });

	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Enemies/Snail/Snail.ovm"}) };

	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Enemies/Snail/Snail.png");
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.8f, 0.5f);

	m_pRigidbody = AddComponent(new RigidBodyComponent(false));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Enemies/Snail/Snail.ovpc");
	m_pRigidbody->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale * 2.f} }, *pDefaultMaterial);

	m_pRigidbody->SetConstraint(RigidBodyConstraint::AllRot, false);

	// Transform
	GetTransform()->Scale(m_GeneralScale * 2.f);
}
void Snail::Update(const SceneContext& sceneContext)
{
	HandleTimer(sceneContext);
	HandleStunEvents();

	HandlePathing(sceneContext);
	HandleTransform(sceneContext);

	HandleAttacking(sceneContext);
	HandleTrail(sceneContext);
}

void Snail::SetStunned()
{
	// Set to stunned
	m_CurrentSnailState = Stunned;
	m_HasToSpin = true;
	m_IsAttackStun = false;
}
void Snail::Push(const XMFLOAT3& source)
{
	// Reset timer
	m_CurrentTime = 0.f;

	// Prepare push
	m_PushSource = source;
	m_HasToPush = true;
}

void Snail::HarryHit()
{
	DamageHarry(1);
}
void Snail::RemoveTrail(Trail* pTrail)
{
	// Remove from vector
	const auto it = std::ranges::find(m_pTrails, pTrail);
	m_pTrails.erase(it);

	// Delete
	m_pTrailObject->RemoveChild(pTrail, true);
}

void Snail::HandleTimer(const SceneContext& sceneContext)
{
	const float deltaTime{ sceneContext.pGameTime->GetElapsed() };

	// ATTACK
	// ------
	if (0 < m_AttackCooldown) m_AttackCooldown -= deltaTime;

	// STUN
	// ----
	
	// If not stunned, return
	if (m_CurrentSnailState != Stunned) return;

	// Count up
	m_CurrentTime += deltaTime;

	// First treshold
	float tresholdTime = 1.5f;
	if (tresholdTime <= m_CurrentTime)
	{
		if (m_IsAttackStun)
		{
			// Go back to pathing
			m_IsAttackStun = false;
			m_CurrentTime = 0.f;

			m_pRigidbody->SetConstraint(RigidBodyConstraint::TransY, true);

			m_CurrentSnailState = Pathing;
		}
		else
		{
			// Stop spinning and enable casting to snail
			m_pRigidbody->SetConstraint(RigidBodyConstraint::AllRot, false);
			m_pCastableComponent->SetCastedTo(false);
		}
	}

	// Second treshold
	tresholdTime = 5.f;
	if (tresholdTime <= m_CurrentTime)
	{
		// Go back to pathing after x time
		m_CurrentTime = 0.f;
		m_CurrentSnailState = Pathing;

		m_pCastableComponent->SwitchSpell();
	}
}
void Snail::HandleStunEvents()
{
	// If not stunned, return
	if (m_CurrentSnailState != Stunned) return;

	// Spin
	if (m_HasToSpin)
	{
		m_HasToSpin = false;

		// Unlock constaints
		m_pRigidbody->SetConstraint(RigidBodyConstraint::RotY, true);

		// Spin
		const float rotationStrength{ 15.f };
		m_pRigidbody->AddTorque(XMFLOAT3{ 0.f, rotationStrength, 0.f }, PxForceMode::eIMPULSE);
	}

	// Push
	if (m_HasToPush)
	{
		m_HasToPush = false;

		// Add force
		const float force{ 10.f };
		const XMFLOAT3 direction{ MathHelper::DirectionTo(m_PushSource, GetTransform()->GetWorldPosition()) };
		const XMVECTOR forceVector{ XMVectorScale(XMLoadFloat3(&direction), force) };

		XMFLOAT3 desiredForce{};
		XMStoreFloat3(&desiredForce, forceVector);

		m_pRigidbody->AddForce(desiredForce, PxForceMode::eIMPULSE);
	}
}

void Snail::HandlePathing(const SceneContext& /*sceneContext*/)
{
	// If not pathing, return
	if (m_CurrentSnailState != Pathing) return;

	// Calculate desired location
	// --------------------------
	const XMFLOAT3 currentPos{ GetTransform()->GetWorldPosition() };
	const XMFLOAT3 desiredPos{ m_PathPositions[m_CurrentGoalPosition] };
	const XMFLOAT3 desiredDirection{ MathHelper::DirectionTo(currentPos, desiredPos) };

	const float movementSpeed{ 13.f };
	const XMVECTOR desiredForce{ XMVectorScale(XMLoadFloat3(&desiredDirection), movementSpeed) };
	XMFLOAT3 force{};
	XMStoreFloat3(&force, desiredForce);

	m_pRigidbody->AddForce(force);
	m_CurrentTarget = desiredPos;

	// Check if reachedDestination
	// ---------------------------
	const float minSqrdDistanceBetwn{ 2.5f };

	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), desiredPos, false) };
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		// If going forward
		if (m_IsReturning == false)
		{
			// Increase
			++m_CurrentGoalPosition;

			// If reached end, change direction
			if (m_PathPositions.size() == m_CurrentGoalPosition)
			{
				m_CurrentGoalPosition = static_cast<int>(m_PathPositions.size() - 2);
				m_IsReturning = true;
			}
		}
		// If going backward
		else
		{
			// Decrease
			--m_CurrentGoalPosition;

			// If reached end, change direction
			if (m_CurrentGoalPosition < 0)
			{
				m_CurrentGoalPosition = 1;
				m_IsReturning = false;
			}
		}
	}
}
void Snail::HandleTransform(const SceneContext& /*sceneContext*/)
{
	// If not stunned
	// --------------
	if (m_CurrentSnailState != Stunned)
	{
		// Rotate towards target
		InstantRotation();

		//// Get angles
		//const XMFLOAT3 currentForward{ GetTransform()->GetForward() };
		//const XMFLOAT3 worldForward{ 1, 0, 0 };
		//const XMFLOAT3 directionToTarget{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), m_CurrentTarget) };
		//
		//const XMVECTOR angleBetweenWorld{ XMVector3AngleBetweenNormals(XMLoadFloat3(&worldForward), XMLoadFloat3(&directionToTarget)) };
		//const XMVECTOR angleBetweenObject{ XMVector3AngleBetweenNormals(XMLoadFloat3(&worldForward), XMLoadFloat3(&directionToTarget)) };

		//const float currentAngle{ XMVectorGetY(angleBetweenObject) };
		//const float desiredAngle{ XMVectorGetY(angleBetweenWorld) };

		//// Calculate required rotation
		//float angleDifference{ desiredAngle - currentAngle };
		//angleDifference = XMScalarModAngle(angleDifference);

		//const int rotationDirection = (0.f < angleDifference) ? 1 : -1;
		//const float rotationSpeed{ 10.f };
		//
		//const float finalAngle{ rotationSpeed * sceneContext.pGameTime->GetElapsed() * rotationDirection };
		//GetTransform()->Rotate(0.f, finalAngle + MathHelper::toRadians(180.f), 0.f, false);
	}
}

void Snail::HandleAttacking(const SceneContext& /*sceneContext*/)
{
	// If not pathing, return
	if (m_CurrentSnailState != Pathing) return;

	// If Harry is close enough
	const float minSqrdDistanceBetwn{ 10.f };

	const XMFLOAT3 harryPos{ m_pHarry->GetCharacter()->GetTransform()->GetWorldPosition() };
	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), harryPos, false)};
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		// Prepare charge
		m_CurrentTarget = harryPos;
		m_pRigidbody->SetConstraint(RigidBodyConstraint::TransY, false);

		// Charge towards him
		const float force{ 7.5f };
		const XMVECTOR forceVector{ XMVectorScale(XMVector3Normalize(XMLoadFloat3(&direction)), force) };
		
		XMFLOAT3 desiredForce{};
		XMStoreFloat3(&desiredForce, forceVector);

		m_pRigidbody->AddForce(desiredForce, PxForceMode::eIMPULSE);

		// Set to attack stun
		m_CurrentSnailState = Stunned;
		m_IsAttackStun = true;

		// Rotate
		InstantRotation();
	}
}
void Snail::HandleTrail(const SceneContext& sceneContext)
{
	// If is stunned, return
	if (m_CurrentSnailState == Stunned) return;

	// Countdown
	m_TrailTimer -= sceneContext.pGameTime->GetElapsed();
	if (m_TrailTimer <= 0.f)
	{
		const float trailFrequency{ 0.3f };
		m_TrailTimer = trailFrequency;

		// Spawn trail
		Trail* pTrail{ m_pTrailObject->AddChild(new Trail{ m_pHarry->GetCharacter(), this})};
		pTrail->GetTransform()->Translate(GetTransform()->GetWorldPosition());
		m_pTrails.emplace_back(pTrail);
	}
}

void Snail::CheckHarry()
{
	// If not attacking, return
	if (m_IsAttackStun == false) return;

	// If Harry is close enough
	const float minSqrdDistanceBetwn{ 3.f };

	const XMFLOAT3 harryPos{ m_pHarry->GetCharacter()->GetTransform()->GetWorldPosition() };
	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), harryPos, false) };
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		DamageHarry(2);
	}
}

void Snail::InstantRotation()
{
	// Calculate rotation
	const XMFLOAT3 worldForward{ 1, 0, 0 };
	const XMFLOAT3 directionToTarget{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), m_CurrentTarget) };

	const XMVECTOR angleBetweenWorld{ XMVector3AngleBetweenNormals(XMLoadFloat3(&worldForward), XMLoadFloat3(&directionToTarget)) };
	const float desiredAngle{ XMVectorGetY(angleBetweenWorld) };

	const float finalAngle{ desiredAngle + MathHelper::toRadians(180.f) };
	GetTransform()->Rotate(0.f, finalAngle, 0.f, false);
}
void Snail::DamageHarry(int amount)
{
	// If still on cooldown, return
	if (0 < m_AttackCooldown) return;

	const float attackCooldown{ 1.f };
	m_AttackCooldown = attackCooldown;

	// Deal damage
	m_pHarry->DealDamage(amount);
}