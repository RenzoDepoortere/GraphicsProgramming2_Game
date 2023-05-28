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
	//AddChild(new BeansProp{m_GeneralScale, CastableComponent::Diffindo});
	GameObject* pProp{ GetScene()->AddChild(new BeansProp{m_GeneralScale, m_pHarry, CastableComponent::Diffindo}) };
	pProp->GetTransform()->Translate(7.f, -8.4f, -79.f);
}