#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	// Get windowVariables
	const SceneContext& sceneContext{ m_pScene->GetSceneContext() };

	const float windowWidth{ sceneContext.windowWidth };
	const float windowHeight{ sceneContext.windowHeight };

	const int halfWidth{ static_cast<int>(windowWidth / 2.f) };
	const int halfHeight{ static_cast<int>(windowHeight / 2.f) };

	// Convert to NDC space
	POINT mousePos{ InputManager::GetMousePosition() };
	mousePos.x = (mousePos.x - halfWidth) / halfWidth;
	mousePos.y = (halfHeight - mousePos.y) / halfHeight;
	
	// Get nearPoint and farPoint
	const XMFLOAT4 nearFloat4{ mousePos.x, mousePos.y, 0, 0 };
	const XMFLOAT4 farFloat4{ mousePos.x, mousePos.y, 1, 0 };

	const XMMATRIX inversedViewProjectionMatrix{ XMLoadFloat4x4(&m_ViewProjectionInverse) };

	const XMVECTOR nearPointVector = XMVector4Transform(XMLoadFloat4(&nearFloat4), inversedViewProjectionMatrix);
	const XMVECTOR farPointVector = XMVector4Transform(XMLoadFloat4(&farFloat4), inversedViewProjectionMatrix);

	XMFLOAT4 nearPoint{};
	XMStoreFloat4(&nearPoint, nearPointVector);
	XMFLOAT4 farPoint{};
	XMStoreFloat4(&farPoint, farPointVector);

	// Raycast
	const PxVec3 raycastStart{ nearPoint.x, nearPoint.y, nearPoint.z };
	const PxVec3 raycastDir{ farPoint.x, farPoint.y, farPoint.z };

	PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	PxRaycastBuffer hit{};
	if (m_pScene->GetPhysxProxy()->Raycast(raycastStart, raycastDir, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		PxRigidActor* pHitActor{ hit.block.actor };
		pHitActor->getOwnerClient();
		m_pScene->GetPhysxProxy()->GetPhysxScene().get
	}

	// Return
	return nullptr;
}