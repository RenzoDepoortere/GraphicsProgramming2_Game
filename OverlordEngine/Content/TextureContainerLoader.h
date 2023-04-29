#pragma once

class TextureContainerLoader : public ContentLoader<std::vector<TextureData*>>
{
public:
	TextureContainerLoader() = default;
	~TextureContainerLoader() override = default;

	TextureContainerLoader(const TextureContainerLoader& other) = delete;
	TextureContainerLoader(TextureContainerLoader&& other) noexcept = delete;
	TextureContainerLoader& operator=(const TextureContainerLoader& other) = delete;
	TextureContainerLoader& operator=(TextureContainerLoader&& other) noexcept = delete;

protected:
	std::vector<TextureData*>* LoadContent(const ContentLoadInfo& loadInfo) override;
	void Destroy(std::vector<TextureData*>* objToDestroy) override;

private:
	// Functions
	bool ReadMtlFile(std::vector<TextureData*>* pTextureVector, const std::filesystem::path& assetFile);
};

