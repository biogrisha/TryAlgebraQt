find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Freetype REQUIRED)
find_package(Boost REQUIRED COMPONENTS signals2)

# Create a library target
add_library(MathDocumentRendering STATIC 
	include/MathDocumentRendering.h
	include/GlyphAtlasRendering.h
	include/TextFromAtlasRendering.h
	src/MathDocumentRendering.cpp
	src/GlyphAtlasRendering.cpp
	src/TextFromAtlasRendering.cpp
)

# Set include directories
target_include_directories(MathDocumentRendering
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MathDocumentRendering
		PRIVATE
		glfw
		glm::glm
		Vulkan::Vulkan
		Freetype::Freetype
		PUBLIC
		MyRTTI
		VulkanFramework
		MathLibrary
)
