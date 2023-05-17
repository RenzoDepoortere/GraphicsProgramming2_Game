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
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	// Variables
	Character* m_pCharacter{};
	GameObject* m_pCharacterMesh{};

	float m_GeneralScale{};
	float m_ControllerHeight{};

	// Functions
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

	void InitMap();
	void InitPlayer();

	void HandleMeshTransform();
};

