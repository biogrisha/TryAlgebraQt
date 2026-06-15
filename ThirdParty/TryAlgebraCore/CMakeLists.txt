find_package(glm REQUIRED)

# Create a library target
add_library(TryAlgebraCore STATIC 
	MathEditor/include/MathDocument.h
	MathEditor/include/TextBuffer.h
	MathEditor/include/DocumentNavigation.h
	MathEditor/include/Structs.h

	MathEditor/src/MathDocument.cpp
	MathEditor/src/TextBuffer.cpp
	MathEditor/src/DocumentNavigation.cpp


	Me/include/MeBase.h
	Me/include/MeCharacter.h
	Me/include/MeParser.h
	Me/include/MeNewLine.h
	Me/include/MeContainer.h
	Me/include/MeGlobals.h
	Me/include/MeFromTo.h
	Me/include/MeAtlas.h
	Me/include/MeVariable.h
	

	Me/src/MeBase.cpp
	Me/src/MeCharacter.cpp
	Me/src/MeParser.cpp
	Me/src/MeNewLine.cpp
	Me/src/MeContainer.cpp
	Me/src/MeFromTo.cpp
	Me/src/MeAtlas.cpp
	Me/src/MeVariable.cpp
	

	Helpers/include/MeHelpers.h
	Helpers/include/EnumFlags.h
	Helpers/src/MeHelpers.cpp 

	TRS/Trs.h
	TRS/Trs.cpp
	TRS/PatternMatching.h
	TRS/PatternMatching.cpp
)

# Set include directories
target_include_directories(TryAlgebraCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TryAlgebraCore
		PUBLIC
		MyRTTI
		glm::glm
		MathDocumentRendering
)
