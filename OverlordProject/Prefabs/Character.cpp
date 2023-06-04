#include "stdafx.h"
#include "Character.h"

Character::Character(const CharacterDesc& characterDesc)
	: m_CharacterDesc{ characterDesc }
	, m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime)
	, m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	// Controller
	// ----------
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	// Camera
	// ------
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	// Transform
	m_CameraStartPos = XMFLOAT3{ 0.f, m_CharacterDesc.controller.height * 0.75f, -2.5f };
	pCamera->GetTransform()->Translate(m_CameraStartPos);

	// Distance
	m_DistanceFromCamera = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_CameraStartPos)));
}

void Character::Update(const SceneContext& sceneContext)
{
	Input(sceneContext);
	LockCamera(sceneContext);
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}

void Character::Input(const SceneContext& sceneContext)
{
	// Return if dead
	if (m_IsDead) return;

	if (m_pCameraComponent->IsActive())
	{
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
		InputManager* pInputManager{ sceneContext.pInput };
		const float elapsedTime{ sceneContext.pGameTime->GetElapsed() };

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{ 0.f, 0.f };
		const XMFLOAT2 leftThumbStickPosition{ InputManager::GetThumbstickPosition() };

		const bool forwardInput{ pInputManager->IsActionTriggered(m_CharacterDesc.actionId_MoveForward) };
		const bool backwardInput{ pInputManager->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward) };
		const bool rightInput{ pInputManager->IsActionTriggered(m_CharacterDesc.actionId_MoveRight) };
		const bool leftInput{ pInputManager->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft) };

		const bool isInput{ forwardInput || backwardInput || rightInput || leftInput };

		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
		if (forwardInput)	move.y = 1;
		else if (backwardInput) move.y = -1;
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (abs(move.y) < epsilon) move.y = leftThumbStickPosition.y;

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		if (rightInput) move.x = 1;
		else if (leftInput)	 move.x = -1;
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (abs(move.x) < epsilon) move.x = leftThumbStickPosition.x;

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f };

		//Only if the Left Mouse Button is Down >
		if (true /*InputManager::IsMouseButton(InputState::down, VK_LBUTTON)*/)
		{
			// Store the MouseMovement in the local 'look' variable (cast is required)
			const POINT& mouseMovement{ InputManager::GetMouseMovement() };
			look = XMFLOAT2{ static_cast<float>(mouseMovement.x), static_cast<float>(mouseMovement.y) };
		}
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look
		if (abs(look.x) < epsilon && abs(look.y) < epsilon)
		{
			const XMFLOAT2 rightThumbStickPosition{ InputManager::GetThumbstickPosition(false) };
			look.x = rightThumbStickPosition.x;
			look.y = rightThumbStickPosition.y;
		}


		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		TransformComponent* pTransformComponent{ GetTransform() };
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		XMVECTOR forwardVector{ XMLoadFloat3(&pTransformComponent->GetForward()) };
		XMVECTOR rightVector{ XMLoadFloat3(&pTransformComponent->GetRight()) };


		//***************
		//CAMERA ROTATION

		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		//Make sure this is calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
		m_TotalYaw += look.x * m_CharacterDesc.rotationSpeed * elapsedTime;
		m_TotalPitch += look.y * m_CharacterDesc.rotationSpeed * elapsedTime;

		// Lock pitch
		const float minPitch{ -30.f };
		const float maxPatch{ 60.f };
		if (m_TotalPitch < minPitch) m_TotalPitch = minPitch;
		if (maxPatch < m_TotalPitch) m_TotalPitch = maxPatch;

		//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)
		pTransformComponent->Rotate(m_TotalPitch, m_TotalYaw, 0.f);
		m_Forward = pTransformComponent->GetForward();
		m_Right = pTransformComponent->GetRight();

		//********
		//MOVEMENT
		const bool isInAir{ m_pControllerComponent->GetCollisionFlags() ^ PxControllerCollisionFlag::eCOLLISION_DOWN };
		const bool isHuggingWall{ m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_SIDES };

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		const float moveAcceleration{ m_MoveAcceleration * elapsedTime };

		//If the character is moving (= input is pressed)
		if (isInput && isInAir == false)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			forwardVector = XMLoadFloat3(&pTransformComponent->GetForward());
			rightVector = XMLoadFloat3(&pTransformComponent->GetRight());
			const XMVECTOR currentDir{ XMVector3Normalize(XMVectorAdd(XMVectorScale(forwardVector, move.y), XMVectorScale(rightVector, move.x))) };
			XMStoreFloat3(&m_CurrentDirection, currentDir);

			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += moveAcceleration;
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			if (m_CharacterDesc.maxMoveSpeed < m_MoveSpeed) m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;
		}
		//Else (character is not moving, or stopped moving) (keep momentum in air)
		else if (isInAir == false || (isInAir == true && isHuggingWall == true))
		{
			// Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= moveAcceleration;
			// Make sure the current MoveSpeed doesn't get smaller than zero
			if (m_MoveSpeed < 0) m_MoveSpeed = 0;
		}

		m_IsJumping = isInAir;

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		const XMVECTOR currentDir{ XMLoadFloat3(&m_CurrentDirection) };
		XMFLOAT3 horizontalVelocity{};
		XMStoreFloat3(&horizontalVelocity, XMVectorScale(currentDir, m_MoveSpeed));
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)
		m_TotalVelocity.x = horizontalVelocity.x;
		m_TotalVelocity.z = horizontalVelocity.z;

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
		if (isInAir)
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if (m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed) m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
		}
		//Else If the jump action is triggered
		else if (pInputManager->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
			m_PressedJump = true;
		}
		//Else (=Character is grounded, no input pressed)
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = -0.1f;
			m_PressedJump = false;
		}


		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		const XMVECTOR totalVelocity{ XMLoadFloat3(&m_TotalVelocity) };
		XMFLOAT3 totalDisplacement{};
		XMStoreFloat3(&totalDisplacement, XMVectorScale(totalVelocity, elapsedTime));

		m_pControllerComponent->Move(totalDisplacement);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
}
void Character::LockCamera(const SceneContext& /*sceneContext*/)
{
	// Get raycast variables
	// ---------------------
	const XMFLOAT3 cameraForward{ m_pCameraComponent->GetTransform()->GetForward() };

	// Direction
	XMFLOAT3 rayDirection = cameraForward;
	XMVECTOR rayDirectionVector = XMVectorScale(XMLoadFloat3(&rayDirection), -1.f);
	XMStoreFloat3(&rayDirection, rayDirectionVector);
	const PxVec3 raycastDir{ rayDirection.x, rayDirection.y ,rayDirection.z };

	// Start
	XMFLOAT3 currentPos{ GetTransform()->GetWorldPosition() };
	const XMVECTOR startPos{ XMVectorAdd(XMLoadFloat3(&currentPos), XMVectorScale(rayDirectionVector, 0.5f)) };
	XMStoreFloat3(&currentPos, startPos);

	const PxVec3 raycastStart {currentPos.x, currentPos.y, currentPos.z};

	// Raycast
	PxRaycastBuffer hit{};
	if (GetScene()->GetPhysxProxy()->Raycast(raycastStart, raycastDir, m_DistanceFromCamera - 0.5f, hit))
	{
		PxRigidActor* pHitActor{ hit.block.actor };
		GameObject* pHitGameObject{ reinterpret_cast<BaseComponent*>(pHitActor->userData)->GetGameObject() };

		// If hitObject isn't player
		if (pHitGameObject != this)
		{
			// Set cameraPos to hitPos
			const PxVec3 hitPos{ raycastDir * hit.block.distance * -1.f};
			m_pCameraComponent->GetTransform()->Translate(hitPos.x, hitPos.y, hitPos.z);

			m_ChangedCameraPos = true;
		}
	}
	// Else, if changed pos
	else if (m_ChangedCameraPos)
	{
		m_ChangedCameraPos = false;

		// Set position
		m_pCameraComponent->GetTransform()->Translate(m_CameraStartPos);
	}
}