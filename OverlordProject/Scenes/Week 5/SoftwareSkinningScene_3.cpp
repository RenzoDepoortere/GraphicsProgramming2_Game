#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	// Start
	// *****

	// Enable GUI
	m_SceneContext.settings.enableOnGUI = true;

	// Create colorMaterial
	ColorMaterial* pColorMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };
	pColorMaterial->UseTransparency(true);

	// Create rootObject
	GameObject* pRoot{ new GameObject{} };


	// FirstBone
	// *********

	// Create firstBone
	const float length{ 15.f };
	m_pBone0 = new BoneObject{ pColorMaterial, length };
	pRoot->AddChild(m_pBone0);


	// SecondBone
	// **********

	// Create secondBone
	m_pBone1 = new BoneObject{ pColorMaterial, length };
	m_pBone0->AddBone(m_pBone1);

	// End
	// ***

	// Add root to scene
	AddChild(pRoot);


	// Calculate bindPose
	m_pBone0->CalculateBindPose();

	// boxDrawer
	GameObject* pBoxDrawer{ new GameObject{} };

	const UINT triangleCapacity{ 24 };
	m_pMeshDrawer = new MeshDrawComponent{ triangleCapacity, true };

	pBoxDrawer->AddComponent(m_pMeshDrawer);
	AddChild(pBoxDrawer);

	// Init vertices
	InitializeVertices(length);
}

void SoftwareSkinningScene_3::Update()
{
	// Rotate bones
	// ************

	// If autoRotation
	if (m_AutoRotation)
	{
		// Calculate rotation
		const float degreesPerSecond{ 45.f };
		const float deltaTime{ m_SceneContext.pGameTime->GetElapsed() };

		// Turn back after 45 degrees
		m_BoneRotation += degreesPerSecond * deltaTime * m_RotationSign;
		if (abs(m_BoneRotation) >= 45.f)
		{
			m_RotationSign *= -1;
		}

		// Rotate
		m_pBone0->GetTransform()->Rotate(0.f, 0.f, m_BoneRotation);
		m_pBone1->GetTransform()->Rotate(0.f, 0.f, -m_BoneRotation * 2.f);
	}
	// Manual rotation
	else
	{
		m_pBone0->GetTransform()->Rotate(m_FirstBoneRotation[0], m_FirstBoneRotation[1], m_FirstBoneRotation[2], true);
		m_pBone1->GetTransform()->Rotate(m_SecondBoneRotation[0], m_SecondBoneRotation[1], m_SecondBoneRotation[2], true);
	}

	// Transform boxes
	// ***************

	// Calculate first boneTransform
	XMFLOAT4X4 bonePose = m_pBone0->GetBindPose();
	XMFLOAT4X4 boneWorldTransform = m_pBone0->GetTransform()->GetWorld();

	XMMATRIX bonePoseMatrix = XMLoadFloat4x4(&bonePose);
	XMMATRIX boneWorldMatrix = XMLoadFloat4x4(&boneWorldTransform);

	const XMMATRIX firstBoneTransform = XMMatrixMultiply(bonePoseMatrix, boneWorldMatrix);

	// Calculate secondBoneTransform
	bonePose = m_pBone1->GetBindPose();
	boneWorldTransform = m_pBone1->GetTransform()->GetWorld();

	bonePoseMatrix = XMLoadFloat4x4(&bonePose);
	boneWorldMatrix = XMLoadFloat4x4(&boneWorldTransform);

	const XMMATRIX secondBoneTransform = XMMatrixMultiply(bonePoseMatrix, boneWorldMatrix);

	// Transform vertices
	XMVECTOR originalPosition{};
	XMVECTOR firstBoneTransformedPosition{};
	XMVECTOR secondBoneTransformedPosition{};
	XMVECTOR lerpedPosition{};
	XMFLOAT3 newPosition{};

	for (size_t idx{}; idx < m_SkinnedVertices.size(); ++idx)
	{
		// Get original position
		originalPosition = XMLoadFloat3(&m_SkinnedVertices[idx].originalVertex.Position);

		// Use boneTransform
		firstBoneTransformedPosition = XMVector3TransformCoord(originalPosition, firstBoneTransform);
		secondBoneTransformedPosition = XMVector3TransformCoord(originalPosition, secondBoneTransform);

		// Lerp positions
		lerpedPosition = XMVectorLerp(firstBoneTransformedPosition, secondBoneTransformedPosition, m_SkinnedVertices[idx].blendWeight1);

		// Store new position
		XMStoreFloat3(&newPosition, lerpedPosition);
		m_SkinnedVertices[idx].transformedVertex.Position = newPosition;
	}

	// Update Boxes
	// ************

	// Remove triangles
	m_pMeshDrawer->RemoveTriangles();

	// Give transformedVertices
	QuadPosNormCol transformedVertices{};
	for (size_t idx{}; idx < m_SkinnedVertices.size(); idx += 4)
	{
		transformedVertices.Vertex1 = m_SkinnedVertices[idx].transformedVertex;
		transformedVertices.Vertex2 = m_SkinnedVertices[idx + 1].transformedVertex;
		transformedVertices.Vertex3 = m_SkinnedVertices[idx + 2].transformedVertex;
		transformedVertices.Vertex4 = m_SkinnedVertices[idx + 3].transformedVertex;

		m_pMeshDrawer->AddQuad(transformedVertices);
	}

	// Update buffer
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::OnGUI()
{
	// Auto rotation
	ImGui::Checkbox("AutoRotation", &m_AutoRotation);

	// Manual rotation
	if (m_AutoRotation == false)
	{
		const float rotationSpeed{ 1.f };
		const float rotationMin{ 0.f };
		const float rotationMax{ 360.f };

		ImGui::DragFloat3("First bone rotation", m_FirstBoneRotation, rotationSpeed, rotationMin, rotationMax);
		ImGui::DragFloat3("Second bone rotation", m_SecondBoneRotation, rotationSpeed, rotationMin, rotationMax);
	}
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
#pragma endregion
}