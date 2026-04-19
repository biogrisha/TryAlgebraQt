find_package(glm REQUIRED)

# Create a library target
add_library(TryAlgebraCore2 STATIC 
	MathEditor/include/MathDocument.h
	MathEditor/include/TextBuffer.h

	MathEditor/src/MathDocument.cpp
	MathEditor/src/TextBuffer.cpp


	Me/include/MeBase.h
	Me/include/MeCharacter.h
	Me/include/MeGenerator.h

	Me/src/MeBase.cpp
	Me/src/MeCharacter.cpp
	Me/src/MeGenerator.cpp

	Helpers/include/MeHelpers.h
	Helpers/src/MeHelpers.cpp
)

# Set include directories
target_include_directories(TryAlgebraCore2
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TryAlgebraCore2
		PUBLIC
		MyRTTI
		glm::glm
		MathDocumentRendering
)
