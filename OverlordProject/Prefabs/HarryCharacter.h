#pragma once
class Character;
class ParticleEmitterComponent;
class MovingSpell;

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
	ParticleEmitterComponent* GetParticleEmitter() const { return m_pCastingObject; }

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

	// Casting object
	ParticleEmitterComponent* m_pCastingObject{ nullptr };
	MovingSpell* m_pMovingSpell{ nullptr };

	// Functions
	// ---------
	void InitHarry(const SceneContext& sceneContext);
	void InitCastingObject(const SceneContext& sceneContext);

	void HandleMeshTransform(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isAiming);
	void HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight, bool isAiming);

	void HandleCastingObject(const SceneContext& sceneContext, bool isAiming);
};