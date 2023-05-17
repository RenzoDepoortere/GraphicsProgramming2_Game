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

	enum CharacterState
	{
		Idle, Moving, Jumping
	};

	// Variables
	// ---------

	// Character
	Character* m_pCharacter{};
	GameObject* m_pCharacterMesh{};

	float m_GeneralScale{};
	float m_ControllerHeight{};

	CharacterState m_CurrentCharacterState{ Idle };
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
	void HandleMeshTransform();
	void HandleAnimations();
};

