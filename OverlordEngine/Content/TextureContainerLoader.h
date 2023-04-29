#pragma once

class TextureContainerLoader : public ContentLoader<TextureData*>
{
public:
	TextureContainerLoader() = default;
	~TextureContainerLoader() override = default;

	TextureContainerLoader(const TextureContainerLoader& other) = delete;
	TextureContainerLoader(TextureContainerLoader&& other) noexcept = delete;
	TextureContainerLoader& operator=(const TextureContainerLoader& other) = delete;
	TextureContainerLoader& operator=(TextureContainerLoader&& other) noexcept = delete;

protected:
	TextureData** LoadContent(const ContentLoadInfo& loadInfo) override;
	void Destroy(TextureData** objToDestroy) override;

private:
	// Variables
	std::vector<TextureData*> m_CreatedTextures{};

	// Functions
	bool ReadMtlFile(const ContentLoadInfo& loadInfo, const std::filesystem::path& assetFile);
};

