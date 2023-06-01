#include "stdafx.h"
#include "PropsPrefab.h"

#include "Prefabs/Vines.h"
#include "Prefabs/BeansProp.h"

#include "Components/CastableComponent.h"

PropsPrefab::PropsPrefab(float generalScale, GameObject* pHarry)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
{
}

void PropsPrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	// Vines
	GetScene()->AddChild(new Vines(m_GeneralScale));

	// Bean props
	// ----------

	// Chest
	GameObject* pProp{ GetScene()->AddChild(new BeansProp{m_GeneralScale, m_pHarry, CastableComponent::Alahomora, L"Chest"})};
	pProp->GetTransform()->Translate(20.f, -7.6f, -83.4f);
	pProp->GetTransform()->Rotate(0.f, 90.f, 0.f);
}