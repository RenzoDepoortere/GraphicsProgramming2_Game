#pragma once
class MomentarilyParticlePrefab final : public GameObject
{
public:
	// Rule of five
	MomentarilyParticlePrefab(GameObject* pPropsPrefab, const std::wstring& particleTexturePath, float stayDuration);
	~MomentarilyParticlePrefab() override = default;

	MomentarilyParticlePrefab(const MomentarilyParticlePrefab& other) = delete;
	MomentarilyParticlePrefab(MomentarilyParticlePrefab&& other) noexcept = delete;
	MomentarilyParticlePrefab& operator=(const MomentarilyParticlePrefab& other) = delete;
	MomentarilyParticlePrefab& operator=(MomentarilyParticlePrefab&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	GameObject* m_pPropsPrefab{ nullptr };

	std::wstring m_ParticleTexturePath{};
	float m_StayDuration{};
	float m_ElapsedTime{};
	UINT m_StartParticleCount{};

	ParticleEmitterComponent* m_pEmitter{ nullptr };
};
