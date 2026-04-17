find_package(glm REQUIRED)

# Create a library target
add_library(TryAlgebraCore2 STATIC 
	Modules/MathEditor/include/MathDocument.h
	Modules/MathEditor/include/TextBuffer.h

	Modules/MathEditor/src/MathDocument.cpp
	Modules/MathEditor/src/TextBuffer.cpp


	Modules/MathEditor/Me/include/MeBase.h
	Modules/MathEditor/Me/include/MeCharacter.h
	Modules/MathEditor/Me/include/MeHelpers.h

	Modules/MathEditor/Me/src/MeBase.cpp
	Modules/MathEditor/Me/src/MeCharacter.cpp
	Modules/MathEditor/Me/src/MeHelpers.cpp
)

# Set include directories
target_include_directories(TryAlgebraCore2
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Modules/MathEditor/include
		${CMAKE_CURRENT_SOURCE_DIR}/Modules/MathEditor/Me/include
)

target_link_libraries(TryAlgebraCore2
		PUBLIC
		MyRTTI
		glm::glm
)
