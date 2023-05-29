#include "stdafx.h"
#include "DeferredRenderer.h"

#include "DeferredLightRenderer.h"

DeferredRenderer::~DeferredRenderer()
{
	//Release Resources
	//Delete Objects
	for (int idx{ 1 }; idx < RT_COUNT; ++idx)	// Skip main RT
	{
		SafeDelete(m_GBuffer[idx]);
	}
	SafeDelete(m_pLightPassRenderer);
}

void DeferredRenderer::Initialize()
{
	//Create & Collect GBuffer RTs
	const auto width = m_GameContext.windowWidth;
	const auto height = m_GameContext.windowHeight;

	const auto pDefaultRenderTarget = m_GameContext.pGame->GetRenderTarget(); // Main RT with depthBuffer
	m_pDefaultRenderTargetView = pDefaultRenderTarget->GetRenderTargetView(); // Main RTV
	m_pDefaultDepthStencilView = pDefaultRenderTarget->GetDepthStencilView(); // Main DSV

	m_GBuffer[static_cast<int>(eGBufferId::LightAccumulation)] = pDefaultRenderTarget;
	m_GBuffer[static_cast<int>(eGBufferId::Diffuse)] = CreateRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_GBuffer[static_cast<int>(eGBufferId::Specular)] = CreateRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_GBuffer[static_cast<int>(eGBufferId::Normal)] = CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_GBuffer[static_cast<int>(eGBufferId::Shadow)] = CreateRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	//Collect References of RTVs
	for (int idx{}; idx < RT_COUNT; ++idx)
	{
		m_RenderTargetViews[idx] = m_GBuffer[idx]->GetRenderTargetView();
	}

	//Collect SRVs (Ambient/Diffuse/Specular/Normal) + DepthSRV
	for (int idx{}; idx < SRV_COUNT - 1; ++idx)
	{
		m_ShaderResourceViews[idx] = m_GBuffer[idx]->GetColorShaderResourceView();
	}
	m_ShaderResourceViews[static_cast<int>(eGBufferId::Depth)] = pDefaultRenderTarget->GetDepthShaderResourceView(); // Depth SRV

	//LightPassRenderer Init
	m_pLightPassRenderer = new DeferredLightRenderer();
	m_pLightPassRenderer->Initialize(m_GameContext.d3dContext);

	const auto descDefault = pDefaultRenderTarget->GetDesc();
	m_pLightPassRenderer->CreateReadOnlyDSV(m_GameContext.d3dContext, descDefault.pDepth, descDefault.depthFormat);
}

RenderTarget* DeferredRenderer::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format) const
{
	RENDERTARGET_DESC rtDesc{};

	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.colorFormat = format;

	rtDesc.enableColorBuffer = true;
	rtDesc.enableColorSRV = true;

	const auto pRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	HANDLE_ERROR(pRenderTarget->Create(rtDesc))

	return pRenderTarget;
}

void DeferredRenderer::Begin(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//1. Clear the G-Buffer RTVs
	const auto clearColor = XMFLOAT4{ Colors::Black };
	
	for (int idx{}; idx < RT_COUNT; ++idx)
	{
		pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[idx], &clearColor.x);
	}
	pDeviceContext->ClearDepthStencilView(m_pDefaultDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	//2. Bind GBuffer RTVs + DSV
	pDeviceContext->OMSetRenderTargets(RT_COUNT, m_RenderTargetViews, m_pDefaultDepthStencilView);

	//3. DRAW GEOMETRY
	//... (Render Scene)
}

void DeferredRenderer::End(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//1. Geometry Pass is finished > Unbind GBuffer RTVs
	ID3D11RenderTargetView* rTargets[RT_COUNT] = { nullptr };
	rTargets[0] = m_pDefaultRenderTargetView;
	pDeviceContext->OMSetRenderTargets(RT_COUNT, rTargets, nullptr);

	//2. Restore Main RenderTarget WITHOUT DSV (DepthBuffer SRV is used in pipeline)
	//pDeviceContext->OMSetRenderTargets(1, &m_pDefaultRenderTargetView, nullptr);

	//3. Directional Light Pass
	m_pLightPassRenderer->DirectionalLightPass(sceneContext, m_ShaderResourceViews);

	//4. Volumetric Light Pass
	m_pLightPassRenderer->VolumetricLightPass(sceneContext, m_ShaderResourceViews, m_pDefaultRenderTargetView);

	//5. Unbind G-Buffer SRVs (Diffuse, Specular, Normal & Depth)
	ID3D11ShaderResourceView* pSRV[SRV_COUNT - 1] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, SRV_COUNT - 1, pSRV);

	//6. Reset Game RenderTarget (back to normal)
	m_GameContext.pGame->SetRenderTarget(nullptr);


	//DEBUG >> Visualize GBUFFER
	//Draw ImGui (to default RT)
	if(m_DrawImGui)
	{
		Debug_DrawGBuffer();
	}
}

void DeferredRenderer::DrawImGui()
{
	ImGui::Checkbox("Draw GBuffer", &m_DrawImGui);

	if(m_DrawImGui)
		ImGui::SliderInt("RTV ID", &m_VizRTVid, 0, RT_COUNT - 1);
}

void DeferredRenderer::Debug_DrawGBuffer() const
{
	//FULL SCREEN
	if(m_VizRTVid >= 1)
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[m_VizRTVid]);

	const auto aspectRatio = m_GameContext.windowWidth / float(m_GameContext.windowHeight);
	const auto width = static_cast<int>(m_GameContext.windowWidth / 3.f);
	const auto height = static_cast<int>(width / aspectRatio);

	Quad renderQuad{
		0,
		int(m_GameContext.windowHeight) - height,
		width,
		height
	};

	//GBUFFER SRVs (only Diffuse/Specular/Normal) - skip Ambient & Depth
	for (auto i{ 1 }; i < SRV_COUNT-1; ++i)
	{
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[i], renderQuad);
		renderQuad.x += width;
	}
}
