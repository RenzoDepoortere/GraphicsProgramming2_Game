#pragma once
class Character;

class HarryPotterScene final : public GameScene
{
public:
	// Rule of Five
	HarryPotterScene() : GameScene(L"HarryPotterScene") {	}
	~HarryPotterScene() override = default;
	HarryPotterScene(const HarryPotterScene& other) = delete;
	HarryPotterScene(HarryPotterScene&& other) noexcept = delete;
	HarryPotterScene& operator=(const HarryPotterScene& other) = delete;
	HarryPotterScene& operator=(HarryPotterScene&& other) noexcept = delete;

private:
	// Enums
	// -----
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	enum CharacterStates
	{
		Idle, 
		RunForward, RunLeft, RunRight, RunBackward, 
		Jumping,
		Hit, Death
	};

	// Variables
	// ---------

	// Character
	Character* m_pCharacter{};
	GameObject* m_pCharacterMesh{};

	float m_GeneralScale{};
	float m_ControllerHeight{};

	float m_CurrentAngle{};

	CharacterStates m_CurrentCharacterState{ Idle };
	ModelAnimator* m_pAnimator{};

	// Functions
	// ---------

	// Scene
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

	// Init
	void InitMap();
	void InitPlayer();

	// Character
	void HandleMeshTransform(bool isForward, bool isBackward, bool isLeft, bool isRight);
	void HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight);
};

