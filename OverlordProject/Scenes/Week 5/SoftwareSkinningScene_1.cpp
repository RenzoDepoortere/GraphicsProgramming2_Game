#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	// Enable GUI
	m_SceneContext.settings.enableOnGUI = true;

	// Create colorMaterial
	ColorMaterial* pColorMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };

	// Create rootObject
	GameObject* pRoot{ new GameObject{} };

	// Create firstBone
	const float length{ 15.f };
	m_pBone0 = new BoneObject{ pColorMaterial, length };
	pRoot->AddChild(m_pBone0);

	// Create secondBone
	m_pBone1 = new BoneObject{ pColorMaterial, length };
	m_pBone0->AddBone(m_pBone1);

	// Add root to scene
	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
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
}

void SoftwareSkinningScene_1::OnGUI()
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