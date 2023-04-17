#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/FixedCamera.h"

#define POSITIVE_BALL_DIRECTION 1;
#define NEGATIVE_BALL_DIRECTION -1;

PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	PxPhysics& pxPhsyx{ PxGetPhysics() };
	PxMaterial* pDefaultMaterial{ pxPhsyx.createMaterial(0.5f,0.5f,0.2f) };

	// Background
	// ----------
	const XMFLOAT4 backgroundColor{ 0.0f, 0.0f, 0.0f, 1.f };
	m_SceneContext.settings.clearColor = backgroundColor;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	// Camera
	// ------
	m_SceneContext.pCamera->GetTransform()->Translate(XMFLOAT3{ 0.f, 5.f, -10.f });

	// Cubes
	// -----
	const XMFLOAT3 cubeDimensions{ 0.5f, 2.f, 1.f };
	const XMFLOAT4 cubeColor{ Colors::AliceBlue };

	const PxBoxGeometry cubeGeometry{ cubeDimensions.x / 2.f, cubeDimensions.y / 2.f, cubeDimensions.z / 2.f };

	const float horizontalDistanceFromMiddle{ 7.5f };

	// Left Cube
	m_pLeftCube = new CubePrefab{ cubeDimensions, cubeColor };
	AddChild(m_pLeftCube);

	RigidBodyComponent* pRigidBody{ m_pLeftCube->AddComponent(new RigidBodyComponent{false}) };
	pRigidBody->AddCollider(cubeGeometry, *pDefaultMaterial);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransY | RigidBodyConstraint::TransZ, false);
	pRigidBody->SetConstraint(RigidBodyConstraint::RotX | RigidBodyConstraint::RotY | RigidBodyConstraint::RotZ, false);

	m_pLeftCube->GetTransform()->Translate(XMFLOAT3{ -horizontalDistanceFromMiddle,0.f,0.f });

	// Right Cube
	m_pRightCube = new CubePrefab{ cubeDimensions, cubeColor };
	AddChild(m_pRightCube);

	pRigidBody = m_pRightCube->AddComponent(new RigidBodyComponent{ false });
	pRigidBody->AddCollider(cubeGeometry, *pDefaultMaterial);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransY | RigidBodyConstraint::TransZ, false);
	pRigidBody->SetConstraint(RigidBodyConstraint::RotX | RigidBodyConstraint::RotY | RigidBodyConstraint::RotZ, false);

	m_pRightCube->GetTransform()->Translate(XMFLOAT3{ horizontalDistanceFromMiddle,0.f,0.f });

	// Sphere
	// ------
	const float sphereRadius{ 0.5f };
	const int sphereSteps{ 12 };
	const XMFLOAT4 sphereColor{ Colors::Crimson };

	const PxSphereGeometry sphereGeometry{ sphereRadius };

	m_pBall = new SpherePrefab{ sphereRadius, sphereSteps, sphereColor };
	AddChild(m_pBall);

	pRigidBody = m_pBall->AddComponent(new RigidBodyComponent{ false });
	pRigidBody->AddCollider(sphereGeometry, *pDefaultMaterial, true);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransZ, false);
	pRigidBody->SetKinematic(true);

	// Boundaries
	// ----------
	float overExtensions{ 2.f };
	const float verticalDistanceFromMiddle{ 7.5f };

	XMFLOAT3 boundaryDimensions{ (horizontalDistanceFromMiddle + overExtensions) * 2.f, 1.f, 1.f };
	const XMFLOAT4 boundaryColor{ backgroundColor };

	PxBoxGeometry boundaryGeometry{ boundaryDimensions.x / 2.f, boundaryDimensions.y / 2.f, boundaryDimensions.z / 2.f };

	// Horizontal Boundaries
	// *********************

	// Upper-boundary
	GameObject* pUpperBoundary{ new CubePrefab{boundaryDimensions, boundaryColor} };
	AddChild(pUpperBoundary);

	pRigidBody = pUpperBoundary->AddComponent(new RigidBodyComponent{ true });
	pRigidBody->AddCollider(boundaryGeometry, *pDefaultMaterial);

	pUpperBoundary->GetTransform()->Translate(XMFLOAT3{ 0.f, verticalDistanceFromMiddle, 0.f });

	// Lower-boundary
	GameObject* pLowerBoundary{ new CubePrefab{boundaryDimensions, boundaryColor} };
	AddChild(pLowerBoundary);

	pRigidBody = pLowerBoundary->AddComponent(new RigidBodyComponent{ true });
	pRigidBody->AddCollider(boundaryGeometry, *pDefaultMaterial);

	pLowerBoundary->GetTransform()->Translate(XMFLOAT3{ 0.f, -verticalDistanceFromMiddle, 0.f });

	// Vertical Boundaries
	// *******************
	boundaryDimensions = XMFLOAT3{ 1.f, (verticalDistanceFromMiddle + overExtensions) * 2.f, 1.f };
	boundaryGeometry = PxBoxGeometry{ boundaryDimensions.x / 2.f, boundaryDimensions.y / 2.f, boundaryDimensions.z / 2.f };

	overExtensions = 2.5f;

	// Left-boundary
	GameObject* pLeftBoundary{ new CubePrefab{boundaryDimensions, boundaryColor} };
	AddChild(pLeftBoundary);

	pRigidBody = pLeftBoundary->AddComponent(new RigidBodyComponent{ true });
	pRigidBody->AddCollider(boundaryGeometry, *pDefaultMaterial);

	pLeftBoundary->GetTransform()->Translate(XMFLOAT3{ -(horizontalDistanceFromMiddle + overExtensions), 0.f, 0.f });

	// Right-boundary
	GameObject* pRightBoundary{ new CubePrefab{boundaryDimensions, boundaryColor} };
	AddChild(pRightBoundary);

	pRigidBody = pRightBoundary->AddComponent(new RigidBodyComponent{ true });
	pRigidBody->AddCollider(boundaryGeometry, *pDefaultMaterial);

	pRightBoundary->GetTransform()->Translate(XMFLOAT3{ (horizontalDistanceFromMiddle + overExtensions), 0.f, 0.f });

	// EXTRA
	// -----

	// Box movement boundaries
	m_UpperBoxBoundary = verticalDistanceFromMiddle - cubeDimensions.y / 2.f - boundaryDimensions.z / 2.f;
	m_LowerBoxBoundary = -verticalDistanceFromMiddle + cubeDimensions.y / 2.f + boundaryDimensions.z / 2.f;

	// Trigger event
	m_pBall->SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		// Paddle collision
		if (action == PxTriggerAction::ENTER && pOther == m_pRightCube)
		{
			m_BallHorizontalMovementDirection = NEGATIVE_BALL_DIRECTION;
		}
		else if (action == PxTriggerAction::ENTER && pOther == m_pLeftCube)
		{
			m_BallHorizontalMovementDirection = POSITIVE_BALL_DIRECTION;
		}
		// Boundary collision
		else if (action == PxTriggerAction::ENTER && pOther == pUpperBoundary)
		{
			m_BallVerticalMovementDirection = NEGATIVE_BALL_DIRECTION;
		}
		else if (action == PxTriggerAction::ENTER && pOther == pLowerBoundary)
		{
			m_BallVerticalMovementDirection = POSITIVE_BALL_DIRECTION;
		}
		else if (action == PxTriggerAction::ENTER && (pOther == pLeftBoundary || pOther == pRightBoundary))
		{
			ResetBall();
		}
	});
}

void PongScene::Update()
{
	Input();
	BallBouncing();
}

void PongScene::Draw()
{

}

void PongScene::OnGUI()
{

}


void PongScene::Input()
{
	// Input
	// =====

	// Sphere
	// ------
	const bool isMoving{ m_BallHorizontalMovementDirection != 0 || m_BallVerticalMovementDirection != 0 };
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, VK_SPACE))
	{
		if (isMoving)
		{
			ResetBall();
		}
		else
		{
			m_BallHorizontalMovementDirection = POSITIVE_BALL_DIRECTION;
			m_BallVerticalMovementDirection = POSITIVE_BALL_DIRECTION;
		}
	}

	// Cubes
	// -----
	const float deltaTime{ m_SceneContext.pGameTime->GetElapsed() };
	const float movementSpeed{ 10.f };
	
	XMFLOAT3 newPos{};
	XMFLOAT3 oldPos{};

	// Move Right cube
	oldPos = m_pRightCube->GetTransform()->GetPosition();
	newPos = oldPos;

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_UP))
	{
		newPos.y += movementSpeed * deltaTime;

		// If box reached over upperBoundary, get back
		if (newPos.y >= m_UpperBoxBoundary)
		{
			newPos.y = oldPos.y;
		}
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_DOWN))
	{
		newPos.y -= movementSpeed * deltaTime;

		// If box reached over lowerBoundary, get back
		if (newPos.y <= m_LowerBoxBoundary)
		{
			newPos.y = oldPos.y;
		}
	}

	m_pRightCube->GetTransform()->Translate(newPos);

	// Move Left cube
	oldPos = m_pLeftCube->GetTransform()->GetPosition();
	newPos = oldPos;

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'R'))
	{
		newPos.y += movementSpeed * deltaTime;

		// If box reached over upperBoundary, get back
		if (newPos.y >= m_UpperBoxBoundary)
		{
			newPos.y = oldPos.y;
		}
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'F'))
	{
		newPos.y -= movementSpeed * deltaTime;

		// If box reached over lowerBoundary, get back
		if (newPos.y <= m_LowerBoxBoundary)
		{
			newPos.y = oldPos.y;
		}
	}

	m_pLeftCube->GetTransform()->Translate(newPos);
}

void PongScene::BallBouncing()
{
	const float ballMovementSpeed{ 5.f };
	const XMFLOAT3 oldPos{ m_pBall->GetTransform()->GetPosition() };

	// Calculate new position
	XMFLOAT3 newPos{ oldPos };
	newPos.x += m_SceneContext.pGameTime->GetElapsed() * ballMovementSpeed * m_BallHorizontalMovementDirection;
	newPos.y += m_SceneContext.pGameTime->GetElapsed() * ballMovementSpeed * m_BallVerticalMovementDirection;

	// Set new position
	m_pBall->GetTransform()->Translate(newPos);
}

void PongScene::ResetBall()
{
	m_pBall->GetTransform()->Translate(XMFLOAT3{});

	m_BallHorizontalMovementDirection = 0;
	m_BallVerticalMovementDirection = 0;
}