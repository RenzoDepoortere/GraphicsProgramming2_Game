#include "stdafx.h"
#include "HarryCharacter.h"

#include "Materials/BasicMaterial_Deferred_Skinned.h"
#include "Materials/BasicMaterial_Deferred.h"

#include "Prefabs/Character.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/MovingSpell.h"
#include "Prefabs/CastObject.h"

#include "Components/DestroyCastableComponent.h"	
#include "Components/BeansCastableComponent.h"
#include "Components/SnailCastableComponent.h"

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
	const bool isAiming{ sceneContext.pInput->IsActionTriggered(CharacterCast) };

	HandleMeshTransform();
	HandleAnimations(isForward, isBackward, isLeft, isRight, isAiming);

	HandleCastingObject(sceneContext, isAiming);
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
	m_pCharacter->GetController()->SetCollisionGroup(CollisionGroup::Group1);

	// Mesh
	// ----
	
	// GameObject
	m_pCharacterMesh = m_pCharacter->AddChild(new GameObject);
	
	m_pCharacterMesh->GetTransform()->Translate(0.f, -characterDesc.controller.height, 0.f);
	m_pCharacterMesh->GetTransform()->Rotate(0.f, 180.f, 0.f);
	m_pCharacterMesh->GetTransform()->Scale(m_GeneralScale);

	m_pCharacterMesh->GetTransform()->SetFollowParentRotation(false);

	ModelComponent* pModel = m_pCharacterMesh->AddComponent(new ModelComponent(L"Meshes/Character/Harry.ovm"));


	// Materials
	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Character/HarryMesh.mtl") };
	BasicMaterial_Deferred_Skinned* pSkinnedDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create materials
		pSkinnedDiffuseMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred_Skinned>();
		pSkinnedDiffuseMaterial->SetDiffuseMap(pLevelMaterials->at(idx));

		// Set material
		pModel->SetMaterial(pSkinnedDiffuseMaterial, static_cast<UINT8>(idx));
	}

	// Animations
	m_pAnimator = pModel->GetAnimator();
	m_pAnimator->Play();

	//Input
	// ----

	// Movement
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

	// Casting
	inputAction = InputAction(CharacterCast, InputState::down, -1, VK_RBUTTON);
	sceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterSpellActivate, InputState::pressed, -1, VK_LBUTTON);
	sceneContext.pInput->AddInputAction(inputAction);
}
void HarryCharacter::InitCastingObject(const SceneContext& /*sceneContext*/)
{
	m_pCastingObject = GetScene()->AddChild(new CastObject{ m_pCharacter });
}

void HarryCharacter::HandleMeshTransform()
{
	// Rotation
	// --------
	if (m_pCharacter->IsJumping()) return;
	const float totalYaw{ m_pCharacter->GetTotalYaw() };
	float angleBuffer{ 180.0f };

	// Calculate currentAngle
	const float currentAngle{ totalYaw + angleBuffer };
	m_pCharacterMesh->GetTransform()->Rotate(0.f, currentAngle, 0.f);
}
void HarryCharacter::HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isAiming)
{
	// Change state
	// ------------
	const CharacterStates initialState{ m_CurrentCharacterState };
	const bool isMoving{ isForward || isLeft || isRight || isBackward };

	// Jumping
	if (m_pCharacter->IsJumping() && (m_pCharacter->PressedJump() || m_CurrentCharacterState == CharacterStates::Jumping))
	{
		m_CurrentCharacterState = CharacterStates::Jumping;
	}
	// Moving
	else if (isMoving)
	{
		m_CurrentCharacterState = CharacterStates::RunForward;
		if (isAiming)
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

void HarryCharacter::HandleCastingObject(const SceneContext& sceneContext, bool isAiming)
{
	// Lambdas
	auto resetCastingObject = [&]()
	{
		m_pCastingObject->GetTransform()->Translate(XMFLOAT3{});
	};

	// If is aiming
	if (isAiming)
	{
		// Check raycast
		XMFLOAT3 hitPos{};
		GameObject* pHitObject{ sceneContext.pCamera->Pick(hitPos, CollisionGroup::Group1) };

		// If hit something
		const bool isDefault{ hitPos.x == 0.f && hitPos.y == 0.f && hitPos.z == 0.f };
		if (isDefault == false)
		{
			// Set pos to hit object
			m_pCastingObject->GetTransform()->Translate(hitPos);

			// Check if is castable
			CastableComponent* pCastable{ pHitObject->GetComponent<DestroyCastableComponent>(true) };
			if (pCastable == nullptr) pCastable = pHitObject->GetComponent<BeansCastableComponent>(true);
			if (pCastable == nullptr) pCastable = pHitObject->GetComponent<SnailCastableComponent>(true);
			if (pCastable == nullptr)
			{
				m_pCastingObject->SetIsCastable(false);
				return;
			}
			if (pCastable->GetCastedTo() == false)
			{
				m_pCastingObject->SetIsCastable(true);
				m_pCastingObject->SetSpell(pCastable->GetSpell());
			}
			else
			{
				m_pCastingObject->SetIsCastable(false);
			}

			// On right click
			const bool spellActivate{ sceneContext.pInput->IsActionTriggered(CharacterSpellActivate) };
			if (spellActivate && pCastable->GetCastedTo() == false)
			{
				// Send spell
				const float spellMovementSpeed{ 15.f };
				m_pMovingSpell = AddChild(new MovingSpell{ spellMovementSpeed, m_pCharacterMesh, pCastable->GetSpell(), pCastable });
				m_pMovingSpell->GetTransform()->Translate(m_pCharacter->GetTransform()->GetWorldPosition());

				// Set casted to
				pCastable->SetCastedTo(true);
			}
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