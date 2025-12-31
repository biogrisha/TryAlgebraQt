#pragma once
#include "ImageBuffer.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

class FAtlas
{
public:
	FAtlas();
	void SetPath(const std::string& AtlasPath);
	void Init();
	FImageBuffer* GetImage();
private:
	std::string Path;
	std::unique_ptr<FImageBuffer> Image;
};