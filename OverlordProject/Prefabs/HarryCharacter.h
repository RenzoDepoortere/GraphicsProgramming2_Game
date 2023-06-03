#pragma once
#include <array>

class Character;
class CastObject;
class MovingSpell;
class HUD;

class HarryCharacter final : public GameObject
{
public:
	// Rule of five
	HarryCharacter(float generalScale, bool isInMainMenu);
	~HarryCharacter() override = default;

	HarryCharacter(const HarryCharacter& other) = delete;
	HarryCharacter(HarryCharacter&& other) noexcept = delete;
	HarryCharacter& operator=(const HarryCharacter& other) = delete;
	HarryCharacter& operator=(HarryCharacter&& other) noexcept = delete;

	// Pass through functions
	Character* GetCharacter() const { return m_pCharacter; }

	void DealDamage(int amount);
	void AddBean();

	bool GetInMainMenu() const { return m_InMainMenu; }
	bool GetInPauseMenu() const { return m_InPauseMenu; }

	void InMainMenu();
	void InPauseMenu(bool isIn);

	void SetSpellHitSoundToPlay(FMOD::Sound* pSound);

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
	Character* m_pCharacter{ nullptr };
	GameObject* m_pCharacterMesh{ nullptr };

	float m_GeneralScale{};
	bool m_InMainMenu{ false };
	bool m_InPauseMenu{ false };
	float m_ControllerHeight{};

	CharacterStates m_CurrentCharacterState{ Idle };
	ModelAnimator* m_pAnimator{};
	bool m_GotHit{ false };

	int m_CurrentHP{};

	// External objects
	CastObject* m_pCastingObject{ nullptr };
	MovingSpell* m_pMovingSpell{ nullptr };
	HUD* m_pHUD{ nullptr };

	FMOD::System* m_pFmod{ nullptr };

	std::array<FMOD::Sound*, 3> m_pFootStepSounds{};
	FMOD::Channel* m_pWalkingChannel{ nullptr };

	std::array<FMOD::Sound*, 3> m_pJumpSounds{};
	FMOD::Channel* m_pJumpChannel{ nullptr };
	bool m_PlayedJumpSound{ false };

	std::array<FMOD::Sound*, 4> m_pCastCallSounds{};
	FMOD::Channel* m_pCastCallChannel{ nullptr };

	std::array<FMOD::Sound*, 4> m_pSmallHitSounds{};
	std::array<FMOD::Sound*, 4> m_pBigHitSounds{};
	FMOD::Sound* m_pDeathSound{};
	FMOD::Channel* m_pHitChannel{};
	bool m_PlayedDeathSound{ false };

	FMOD::Channel* m_pBackgroundChannel{ nullptr };
	FMOD::Channel* m_pAmbianceChannel{ nullptr };

	FMOD::Channel* m_pSpellAimChannel{ nullptr };
	FMOD::Channel* m_pSpellTargetChannel{ nullptr };

	FMOD::Channel* m_pSpellHitChannel{ nullptr };

	// Functions
	// ---------
	void InitHarry(const SceneContext& sceneContext);
	void InitExternals();

	void HandleMeshTransform();
	void HandleAnimations(bool isForward, bool isBackward, bool isLeft, bool isRight);

	void HandleCastingObject(const SceneContext& sceneContext, bool isAiming);
};