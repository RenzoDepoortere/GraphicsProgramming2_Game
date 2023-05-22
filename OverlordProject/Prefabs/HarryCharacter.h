#pragma once
class Character;
class HarryCharacter final : public GameObject
{
public:
	// Rule of five
	HarryCharacter(float generalScale);
	~HarryCharacter() override = default;

	HarryCharacter(const HarryCharacter& other) = delete;
	HarryCharacter(HarryCharacter&& other) noexcept = delete;
	HarryCharacter& operator=(const HarryCharacter& other) = delete;
	HarryCharacter& operator=(HarryCharacter&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

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
	void HandleMeshTransform(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isHoldingLeft);
	void HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isHoldingLeft);
};