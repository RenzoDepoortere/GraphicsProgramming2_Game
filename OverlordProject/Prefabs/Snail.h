#pragma once
class HarryCharacter;

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

protected:
	// Functionality
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Enums
	// -----
	enum SnailState
	{
		Pathing, Attacking, Stunned
	};

	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{};

	SnailState m_CurrentSnailState{ Pathing };
	RigidBodyComponent* m_pRigidbody{};

	std::vector<XMFLOAT3> m_PathPositions{};
	int m_CurrentGoalPosition{};
	bool m_IsReturning{ false };

	XMFLOAT3 m_CurrentTarget{};
	float m_TotalYaw{};

	//std::vector

	// Functions
	// ---------
	void HandlePathing(const SceneContext& sceneContext);
	void HandleTransform(const SceneContext& sceneContext);

	void HandleTrail(const SceneContext& sceneContext);
};