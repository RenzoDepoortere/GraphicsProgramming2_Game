#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

ComponentTestScene::ComponentTestScene() :
	GameScene(L"ComponentTestScene") {}

void ComponentTestScene::Initialize()
{
	PxPhysics& physx{ PxGetPhysics() };
	PxMaterial* pBouncyMaterial{ physx.createMaterial(0.5f, 0.5f, 1.f) };

	// Ground Plane
	// ------------
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	// Spheres
	// -------

	// Red Sphere (group 0 and ignore group 1 & 2)
	SpherePrefab* pSphereRed{ new SpherePrefab{1,10,XMFLOAT4(Colors::Red)} };
	AddChild(pSphereRed);
	pSphereRed->GetTransform()->Translate(0.f, 30.f, 0.f);

	RigidBodyComponent* pRigidBody{ pSphereRed->AddComponent(new RigidBodyComponent{ false }) };
	pRigidBody->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);

	// Green Sphere (group 1)
	SpherePrefab* pSphereGreen{ new SpherePrefab{1,10,XMFLOAT4(Colors::Green)} };
	AddChild(pSphereGreen);
	pSphereGreen->GetTransform()->Translate(0.5f, 20.f, 0.f);

	pRigidBody = pSphereGreen->AddComponent(new RigidBodyComponent{ false });
	pRigidBody->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);

	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	// Blue Sphere (group 2)
	SpherePrefab* pSphereBlue{ new SpherePrefab{1,10,XMFLOAT4(Colors::Blue)} };
	AddChild(pSphereBlue);
	pSphereBlue->GetTransform()->Translate(0.f, 10.f, 0.f);

	pRigidBody = pSphereBlue->AddComponent(new RigidBodyComponent{ false });
	pRigidBody->AddCollider(PxSphereGeometry(1.f), *pBouncyMaterial);
	
	pRigidBody->SetCollisionGroup(CollisionGroup::Group2);
}

void ComponentTestScene::Update()
{

}

void ComponentTestScene::Draw()
{

}

void ComponentTestScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}