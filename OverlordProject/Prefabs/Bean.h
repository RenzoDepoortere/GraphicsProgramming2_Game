#pragma once
#include <array>
class HarryCharacter;

class Bean final : public GameObject
{
public:
	// Rule of five
	Bean(float generalScale, HarryCharacter* pHarry, const XMFLOAT3& spawnLocation, const XMFLOAT3& forceDirection, bool playSpawnSound = true);
	~Bean() override = default;

	Bean(const Bean& other) = delete;
	Bean(Bean&& other) noexcept = delete;
	Bean& operator=(const Bean& other) = delete;
	Bean& operator=(Bean&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	bool m_IsActivated{ true };

	float m_GeneralScale{};
	HarryCharacter* m_pHarry{};

	XMFLOAT3 m_SpawnLocation{};
	int m_NrBeans{};

	XMFLOAT3 m_ForceDirection{};
	float m_Force{};

	float m_TotalYaw{};
	float m_RotationSpeed{};

	bool m_PlaySpawnSound{};

	std::array<FMOD::Sound*, 3> m_pSpawnSound{};
	FMOD::Sound* m_pPickUpSound{ nullptr };
};