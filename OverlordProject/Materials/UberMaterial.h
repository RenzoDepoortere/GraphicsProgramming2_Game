#pragma once
class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() override = default;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetDiffuseTexture(TextureData* pTextureData);
	void SetSpecularLevelTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetEnvironmentCubeTexture(const std::wstring& assetFile);
	void SetOpacityMapTexture(const std::wstring& assetFile);

protected:
	virtual void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{};
	TextureData* m_pSpecularLevelTexture{};
	TextureData* m_pNormalTexture{};
	TextureData* m_pEnvironmentCubeTexture{};
	TextureData* m_pOpacityMapTexture{};
};

