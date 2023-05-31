#pragma once
class Character;
class CastObject;
class MovingSpell;
class HUD;

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

	// Pass through functions
	Character* GetCharacter() const { return m_pCharacter; }
	
	void DealDamage(int amount);
	void AddBean();

protected:
	// Functionality
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
		CharacterJump,
		CharacterCast,
		CharacterSpellActivate
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

	CharacterStates m_CurrentCharacterState{ Idle };
	ModelAnimator* m_pAnimator{};

	int m_CurrentHP{};

	// External objects
	CastObject* m_pCastingObject{ nullptr };
	MovingSpell* m_pMovingSpell{ nullptr };
	HUD* m_pHUD{ nullptr };

	// Functions
	// ---------
	void InitHarry(const SceneContext& sceneContext);
	void InitExternals();

	void HandleMeshTransform();
	void HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isAiming);

	void HandleCastingObject(const SceneContext& sceneContext, bool isAiming);
};