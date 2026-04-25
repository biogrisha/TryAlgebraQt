find_package(glm REQUIRED)

# Create a library target
add_library(TryAlgebraCore2 STATIC 
	MathEditor/include/MathDocument.h
	MathEditor/include/TextBuffer.h

	MathEditor/src/MathDocument.cpp
	MathEditor/src/TextBuffer.cpp


	Me/include/MeBase.h
	Me/include/MeCharacter.h
	Me/include/MeParser.h
	Me/include/MeNewLine.h
	Me/include/MeContainer.h
	Me/include/MeGlobals.h
	

	Me/src/MeBase.cpp
	Me/src/MeCharacter.cpp
	Me/src/MeParser.cpp
	Me/src/MeNewLine.cpp
	Me/src/MeContainer.cpp
	

	Helpers/include/MeHelpers.h
	Helpers/src/MeHelpers.cpp)

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
