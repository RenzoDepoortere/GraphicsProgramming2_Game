#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		float passedTicks{ sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed };

		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		if (passedTicks >= m_CurrentClip.duration)
		{
			passedTicks = fmod(passedTicks, m_CurrentClip.duration);
		}


		//2. 
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;

			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		//ELSE
		else
		{
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;

			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}


		//3.
		//Find the enclosing keys
		AnimationKey keyA{}, keyB{};
		keyB.tick = FLT_MAX;

		//Iterate all the keys of the clip and find the following keys:
		for (const auto& currentKey : m_CurrentClip.keys)
		{
			//keyA > Closest Key with Tick before/smaller than m_TickCount
			if (currentKey.tick > keyA.tick && currentKey.tick < m_TickCount)
			{
				keyA = currentKey;
			}

			//keyB > Closest Key with Tick after/bigger than m_TickCount
			if (currentKey.tick < keyB.tick && currentKey.tick > m_TickCount)
			{
				keyB = currentKey;
			}
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		const float blendFactor{ (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick) };

		//Clear the m_Transforms vector
		m_Transforms.clear();

		//FOR every boneTransform in a key (So for every bone)
		for (size_t idx{}; idx < keyA.boneTransforms.size(); ++idx)
		{
			//	Retrieve the transform from keyA (transformA)
			const XMFLOAT4X4 transformA{ keyA.boneTransforms[idx] };

			// 	Retrieve the transform from keyB (transformB)
			const XMFLOAT4X4 transformB{ keyB.boneTransforms[idx] };

			//	Decompose both transforms
			XMVECTOR scaleA{};
			XMVECTOR rotationA{};
			XMVECTOR translationA{};

			XMVECTOR scaleB{};
			XMVECTOR rotationB{};
			XMVECTOR translationB{};

			XMMatrixDecompose(&scaleA, &rotationA, &translationA, XMLoadFloat4x4(&transformA));
			XMMatrixDecompose(&scaleB, &rotationB, &translationB, XMLoadFloat4x4(&transformB));

			//	Lerp between all the transformations (Position, Scale, Rotation)
			const XMVECTOR lerpedScale = XMVectorLerp(scaleA, scaleB, blendFactor);
			const XMVECTOR lerpedRotation = XMQuaternionSlerp(rotationA, rotationB, blendFactor);
			const XMVECTOR lerpedTranslation = XMVectorLerp(translationA, translationB, blendFactor);

			//	Compose a transformation matrix with the lerp-results
			const XMMATRIX transformationMatrix = XMMatrixAffineTransformation(lerpedScale, XMVECTOR{}, lerpedRotation, lerpedTranslation);

			//	Add the resulting matrix to the m_Transforms vector
			XMFLOAT4X4 finalMatrix{};
			XMStoreFloat4x4(&finalMatrix, transformationMatrix);
			m_Transforms.push_back(finalMatrix);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//TODO_W6_()
	
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (const auto& currentClip : m_pMeshFilter->GetAnimationClips())
	{
		//If found,
		if (currentClip.name == clipName)
		{
			//	Call SetAnimation(Animation Clip) with the found clip
			SetAnimation(currentClip);
			return;
		}
	}
	
	// Else,
	// Call Reset
	Reset();
	
	//	Log a warning with an appropriate message
	const LogString warningString{ L"Warning: no animation found with given clipName" };
	Logger::LogWarning(warningString);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO_W6_()
	
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
	if (clipNumber < m_pMeshFilter->GetAnimationClips().size() == false)
	{
		//	Call Reset
		Reset();

		//	Log a warning with an appropriate message
		const LogString warningString{ L"Warning: clipNumber was outside of the vector's range" };
		Logger::LogWarning(warningString);

		//	return
		return;
	}
	//else
	else
	{
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		const AnimationClip requestedClip{ m_pMeshFilter->GetAnimationClips()[clipNumber] };

		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(requestedClip);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//TODO_W6_()
	
	//Set m_ClipSet to true
	m_ClipSet = true;

	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//TODO_W6_()
	
	//If pause is true, set m_IsPlaying to false
	if (pause) m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;

	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	if (m_ClipSet)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		const std::vector<XMFLOAT4X4>& boneTransforms{ m_CurrentClip.keys[0].boneTransforms };

		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	//Else
	else
	{
		//	Create an IdentityMatrix 
		const XMFLOAT4X4 idMatrix{};

		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, idMatrix);
	}
}
