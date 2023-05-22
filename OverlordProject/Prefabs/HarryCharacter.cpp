#include "stdafx.h"
#include "HarryCharacter.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/UberMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

#include "Prefabs/Character.h"
#include "Prefabs/CubePrefab.h"

HarryCharacter::HarryCharacter(float generalScale)
	: m_GeneralScale{ generalScale }
{
}

void HarryCharacter::Initialize(const SceneContext& sceneContext)
{
	InitHarry(sceneContext);
	InitCastingObject(sceneContext);
}

void HarryCharacter::Update(const SceneContext& sceneContext)
{
	// Actions
	const bool isForward{ sceneContext.pInput->IsActionTriggered(CharacterMoveForward) };
	const bool isBackward{ sceneContext.pInput->IsActionTriggered(CharacterMoveBackward) };
	const bool isLeft{ sceneContext.pInput->IsActionTriggered(CharacterMoveLeft) };
	const bool isRight{ sceneContext.pInput->IsActionTriggered(CharacterMoveRight) };
	const bool isHoldingLeft{ InputManager::IsMouseButton(InputState::down, VK_LBUTTON) };

	HandleMeshTransform(isForward, isBackward, isLeft, isRight, isHoldingLeft);
	HandleAnimations(isForward, isBackward, isLeft, isRight, isHoldingLeft);

	HandleCastingObject(sceneContext, isHoldingLeft);
}

// Member functions
// ----------------
void HarryCharacter::InitHarry(const SceneContext& sceneContext)
{
	// Physics
	// -------
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Character Controller
	// -------------------
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	characterDesc.controller.height *= 0.6f;
	m_ControllerHeight = characterDesc.controller.height;

	characterDesc.maxMoveSpeed = 6.0f;
	characterDesc.moveAccelerationTime = 0.3f;
	characterDesc.maxFallSpeed = 17.5f;
	characterDesc.fallAccelerationTime = 1.0f;
	characterDesc.JumpSpeed = 7.0f;
	characterDesc.rotationSpeed = 30.f;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(-5.f, -6.f, -79.f);

	// Mesh
	// ----
	m_pCharacterMesh = AddChild(new GameObject);
	ModelComponent* pModel = m_pCharacterMesh->AddComponent(new ModelComponent(L"Meshes/Harry.ovm"));

	// Materials
	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Character/skharrymesh.mtl") };
	DiffuseMaterial_Skinned* pSkinnedDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create materials
		pSkinnedDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
		pSkinnedDiffuseMaterial->SetDiffuseTexture(pLevelMaterials->at(idx));

		// Set material
		pModel->SetMaterial(pSkinnedDiffuseMaterial, static_cast<UINT8>(idx));
	}

	// Animations
	m_pAnimator = pModel->GetAnimator();
	m_pAnimator->Play();

	m_pCharacterMesh->GetTransform()->Scale(m_GeneralScale);

	//Input
	// ----
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	sceneContext.pInput->AddInputAction(inputAction);

	// Other
	// -----
	m_CurrentAngle = m_pCharacter->GetTotalYaw();
}
void HarryCharacter::InitCastingObject(const SceneContext& /*sceneContext*/)
{
	m_pCastingObject = new CubePrefab{};
	AddChild(m_pCastingObject);
}

void HarryCharacter::HandleMeshTransform(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isHoldingLeft)
{
	// Correctly transform model
	// *************************
	TransformComponent* pControllerTransform{ m_pCharacter->GetController()->GetTransform() };

	// Position
	// --------
	const XMFLOAT3 controllerPosition{ pControllerTransform->GetWorldPosition() };
	const float characterBuffer{ 0.5f };
	m_pCharacterMesh->GetTransform()->Translate(controllerPosition.x, controllerPosition.y - m_ControllerHeight / 2.f - characterBuffer, controllerPosition.z);

	// Rotation
	// --------
	if (m_pCharacter->IsJumping()) return;
	const float totalYaw{ m_pCharacter->GetTotalYaw() };
	float angleBuffer{ 180.0f };

	// If button not held, rotate according to movement
	if (isHoldingLeft == false)
	{
		if (isForward)       angleBuffer = 180.f;
		else if (isBackward) angleBuffer = 0.f;
		else if (isLeft)     angleBuffer = 90.f;
		else if (isRight)    angleBuffer = -90.f;
		else			     return;
	}

	// Calculate currentAngle
	m_CurrentAngle = totalYaw + angleBuffer;
	m_pCharacterMesh->GetTransform()->Rotate(0.f, m_CurrentAngle, 0.f);
}
void HarryCharacter::HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isHoldingLeft)
{
	// Change state
	// ------------
	const CharacterStates initialState{ m_CurrentCharacterState };
	const bool isMoving{ isForward || isLeft || isRight || isBackward };

	// Jumping
	if (m_pCharacter->IsJumping())
	{
		m_CurrentCharacterState = CharacterStates::Jumping;
	}
	// Moving
	else if (isMoving)
	{
		m_CurrentCharacterState = CharacterStates::RunForward;
		if (isHoldingLeft)
		{
			if (isForward)       m_CurrentCharacterState = CharacterStates::RunForward;
			else if (isBackward) m_CurrentCharacterState = CharacterStates::RunBackward;
			else if (isLeft)     m_CurrentCharacterState = CharacterStates::RunLeft;
			else if (isRight)    m_CurrentCharacterState = CharacterStates::RunRight;
		}
	}
	// Idle
	else
	{
		m_CurrentCharacterState = CharacterStates::Idle;
	}

	// Change animations, if necessary
	// -------------------------------
	if (initialState != m_CurrentCharacterState)
	{
		m_pAnimator->SetAnimation(static_cast<int>(m_CurrentCharacterState));
		m_pAnimator->Play();
	}
}

void HarryCharacter::HandleCastingObject(const SceneContext& sceneContext, bool isHoldingLeft)
{
	// Lambdas
	auto resetCastingObject = [&]()
	{
		m_pCastingObject->GetTransform()->Translate(XMFLOAT3{});
	};

	// If is casting
	if (isHoldingLeft)
	{
		// Check raycast
		XMFLOAT3 hitPos{};
		sceneContext.pCamera->Pick(hitPos);

		// If hit something
		const bool isDefault{ hitPos.x == 0.f && hitPos.y == 0.f && hitPos.z == 0.f };
		if (isDefault == false)
		{
			// Set pos to hit object
			m_pCastingObject->GetTransform()->Translate(hitPos);
		}
		// Else, reset
		else resetCastingObject();
	}
	// Reset
	else
	{
		resetCastingObject();
	}
}