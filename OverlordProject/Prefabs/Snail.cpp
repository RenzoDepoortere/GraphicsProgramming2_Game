#include "stdafx.h"
#include "Snail.h"

#include "Materials/BasicMaterial_Deferred.h"

#include "Components/SnailCastableComponent.h"

Snail::Snail(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{generalScale}
	, m_pHarry{ pHarry }
	, m_CurrentGoalPosition{ 1 }
{
}

void Snail::Initialize(const SceneContext& /*sceneContext*/)
{
	// Castable
	AddComponent(new SnailCastableComponent{ CastableComponent::Rictusempra, CastableComponent::Diffindo, this });

	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Enemies/Snail/Snail.ovm"}) };

	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Enemies/Snail/Snail.png");
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_pRigidbody = AddComponent(new RigidBodyComponent(false));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Enemies/Snail/Snail.ovpc");
	m_pRigidbody->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale * 2.f} }, *pDefaultMaterial);

	m_pRigidbody->SetConstraint(RigidBodyConstraint::AllRot, false);

	// Transform
	GetTransform()->Scale(m_GeneralScale * 2.f);
}

void Snail::Update(const SceneContext& sceneContext)
{
	HandlePathing(sceneContext);
	HandleTransform(sceneContext);

	HandleTrail(sceneContext);
}

void Snail::SetStunned()
{
	m_CurrentSnailState = Stunned;
	m_DegreesSpinned = 0.f;
}
void Snail::Push(const XMFLOAT3& /*source*/)
{

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

	const float movementSpeed{ 10.f };
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
void Snail::HandleTransform(const SceneContext& sceneContext)
{
	// If not stunned
	// --------------
	if (m_CurrentSnailState != Stunned)
	{
		// Rotate towards target
		const XMFLOAT3 currentForward{ GetTransform()->GetForward() };
		const XMFLOAT3 desiredForward{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), m_CurrentTarget) };
		const XMVECTOR angleBetween{ XMVector3AngleBetweenNormals(XMLoadFloat3(&currentForward), XMLoadFloat3(&desiredForward)) };

		const float rotationSpeed{ 1.f };
		m_TotalYaw += MathHelper::toDegrees(XMVectorGetY(angleBetween)) * rotationSpeed * sceneContext.pGameTime->GetElapsed();
		GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);
	}
}

void Snail::HandleTrail(const SceneContext& /*sceneContext*/)
{

}