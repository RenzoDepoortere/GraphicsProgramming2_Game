#pragma once

class CastlePrefab final : public GameObject
{
public:
	// Rule of five
	CastlePrefab(float generalScale);
	~CastlePrefab() override = default;

	CastlePrefab(const CastlePrefab& other) = delete;
	CastlePrefab(CastlePrefab&& other) noexcept = delete;
	CastlePrefab& operator=(const CastlePrefab& other) = delete;
	CastlePrefab& operator=(CastlePrefab&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
};