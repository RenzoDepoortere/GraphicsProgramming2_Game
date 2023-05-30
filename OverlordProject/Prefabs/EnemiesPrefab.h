#pragma once
class HarryCharacter;

class EnemiesPrefab final : public GameObject
{
public:
	// Rule of five
	EnemiesPrefab(float generalScale, HarryCharacter* pHarry);
	~EnemiesPrefab() override = default;

	EnemiesPrefab(const EnemiesPrefab& other) = delete;
	EnemiesPrefab(EnemiesPrefab&& other) noexcept = delete;
	EnemiesPrefab& operator=(const EnemiesPrefab& other) = delete;
	EnemiesPrefab& operator=(EnemiesPrefab&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{};
};