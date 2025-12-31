#include "Atlas.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using json = nlohmann::json;
FAtlas::FAtlas()
{

}

void FAtlas::SetPath(const std::string& AtlasPath)
{
	Path = AtlasPath;
}

void FAtlas::Init()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(Path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	Image = MyRTTI::MakeTypedUnique<FImageBuffer>();
	Image->SetExtent({uint32_t(texWidth), uint32_t(texHeight)});
	Image->AddUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	Image->Init();
	Image->UpdateImageFromData(pixels);
}

FImageBuffer* FAtlas::GetImage()
{
	return Image.get();
}
