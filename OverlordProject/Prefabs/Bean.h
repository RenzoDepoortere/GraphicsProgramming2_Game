#pragma once
class Bean final : public GameObject
{
public:
	// Rule of five
	Bean(float generalScale, const XMFLOAT3& spawnLocation, const XMFLOAT3& forceDirection);
	~Bean() override = default;

	Bean(const Bean& other) = delete;
	Bean(Bean&& other) noexcept = delete;
	Bean& operator=(const Bean& other) = delete;
	Bean& operator=(Bean&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	XMFLOAT3 m_SpawnLocation{};
	int m_NrBeans{};

	XMFLOAT3 m_ForceDirection{};
	float m_Force{};
};