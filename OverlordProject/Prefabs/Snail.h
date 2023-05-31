#pragma once
class HarryCharacter;
class SnailCastableComponent;
class Trail;

class Snail final : public GameObject
{
public:
	// Rule of five
	Snail(float generalScale, HarryCharacter* pHarry);
	~Snail() override = default;

	Snail(const Snail& other) = delete;
	Snail(Snail&& other) noexcept = delete;
	Snail& operator=(const Snail& other) = delete;
	Snail& operator=(Snail&& other) noexcept = delete;

	// Functionality
	void AddPathPosition(const XMFLOAT3& position) { m_PathPositions.push_back(position); }
	
	void SetStunned();
	void Push(const XMFLOAT3& source);

	void HarryHit();
	void RemoveTrail(Trail* pTrail);

protected:
	// Functionality
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Enums
	// -----
	enum SnailState
	{
		Pathing, Stunned
	};

	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{};

	SnailState m_CurrentSnailState{ Pathing };
	RigidBodyComponent* m_pRigidbody{};
	SnailCastableComponent* m_pCastableComponent{};

	std::vector<XMFLOAT3> m_PathPositions{};
	int m_CurrentGoalPosition{};
	bool m_IsReturning{ false };

	GameObject* m_pTrailObject{};
	std::vector<Trail*> m_pTrails{};
	float m_TrailTimer{};

	float m_AttackCooldown{};

	XMFLOAT3 m_CurrentTarget{};
	float m_TotalYaw{};

	bool m_HasToSpin{ false };
	bool m_HasToPush{ false };
	XMFLOAT3 m_PushSource{};
	float m_CurrentTime{};
	bool m_IsAttackStun{ false };

	// Functions
	// ---------
	void HandleTimer(const SceneContext& sceneContext);
	void HandleStunEvents();

	void HandlePathing(const SceneContext& sceneContext);
	void HandleTransform(const SceneContext& sceneContext);

	void HandleAttacking(const SceneContext& sceneContext);
	void HandleTrail(const SceneContext& sceneContext);

	void InstantRotation();
	void DamageHarry(int amount);
};