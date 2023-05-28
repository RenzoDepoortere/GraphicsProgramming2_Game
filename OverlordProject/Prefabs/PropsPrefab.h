#pragma once

class PropsPrefab final : public GameObject
{
public:
	// Rule of five
	PropsPrefab(float generalScale, GameObject* pHarry);
	~PropsPrefab() override = default;

	PropsPrefab(const PropsPrefab& other) = delete;
	PropsPrefab(PropsPrefab&& other) noexcept = delete;
	PropsPrefab& operator=(const PropsPrefab& other) = delete;
	PropsPrefab& operator=(PropsPrefab&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	GameObject* m_pHarry{};
};