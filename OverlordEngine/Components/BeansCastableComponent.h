#pragma once
#include "CastableComponent.h"

class BeansProp;
class HarryCharacter;

class BeansCastableComponent final : public CastableComponent
{
public:
	// Rule of five
	BeansCastableComponent(float generalScale, HarryCharacter* pHarry, Spell spell, BeansProp* pProp);
	virtual ~BeansCastableComponent() override = default;

	BeansCastableComponent(const BeansCastableComponent& other) = delete;
	BeansCastableComponent(BeansCastableComponent&& other) noexcept = delete;
	BeansCastableComponent& operator=(const BeansCastableComponent& other) = delete;
	BeansCastableComponent& operator=(BeansCastableComponent&& other) noexcept = delete;

	// Functionality
	virtual void Activate(GameObject* pHarry) override;

	// Getters and Setters
	void SetMinNrBeans(int minNrBeans) { m_MinNrBeans = minNrBeans; }
	int GetMinNrBeans() const { return m_MinNrBeans; }
	void SetMaxNrBeans(int maxNrBeans) { m_MaxNrBeans = maxNrBeans; }
	int GetMaxNrBeans() const { return m_MaxNrBeans; }

	void SetTimeBetweenBeans(float timeBetween) { m_TimeBetweenBeans = timeBetween; }
	float GetTimeBetweenBeans() const { return m_TimeBetweenBeans; }

	void SetMaxRandomShootAngle(float maxRandomAngle) { m_MaxRandomShootAngle = maxRandomAngle; }
	float GetMaxRandomShootAngle() const { return m_MaxRandomShootAngle; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Member variables
	// ----------------
	bool m_HasBeenActivated{ false };
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{};
	BeansProp* m_pProp{};

	// Beans
	int m_MinNrBeans{}, m_MaxNrBeans{};
	int m_NrBeansToSpawn{};

	bool m_SpawnBeans{};
	GameObject* m_pBeanHolder{};

	// Timer
	float m_TimeBetweenBeans{};
	float m_CurrentTimer{};

	// Spawning
	float m_MaxRandomShootAngle{};

	XMFLOAT3 m_PreviousLocation{};
};

