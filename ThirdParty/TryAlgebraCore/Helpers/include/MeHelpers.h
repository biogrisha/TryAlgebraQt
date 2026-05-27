#pragma once
#include <string>
#include <vector>
#include <optional>
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>
#include <MathEditor/include/Structs.h>

namespace TryAlgebraCore
{
	class MeHelpers
	{
	public:
		enum class GetByPathStatus
		{
			last,
			cont,
			me,
			outside,
			none
		};
		struct GetByPathRes
		{
			GetByPathStatus status = GetByPathStatus::none;
			std::optional<size_t> pos;
			MeBase* me = nullptr;
		};

		static std::optional<size_t> absToChildPos(const MeBase* from, size_t pos);
		static GetByPathRes getByPath(MeBase* from, const MePath& path);
		static FCaretData getCaretData(MeBase* from, const MePath& path);
		static void orderPaths(MePath& path1, MePath& path2);
		static void trimToCommonContainer(MePath& path1, MePath& path2);
		static size_t getPosOrFrom(const std::variant<MePos, ContPos, LeafPos>& v);

		static size_t getPosOrTo(const std::variant<MePos, ContPos, LeafPos>& v);

		static bool isWithinMe(const glm::vec2& pos, MeBase* me);
		static bool isLeft(const glm::vec2& pos, MeBase* me);
		static bool getPathAtPos(MeBase* from, const glm::vec2& pos, MePath& path);
		static MeBase* getByTreePath(MeBase* from, const std::vector<size_t>& path);

		//me arrangement
		static void alignVertically(const std::vector<std::unique_ptr<MeBase>>& mes, float& center_x);

		//me drawing
		static void highlightSelected(MeBase* cont, MePath from, MePath to, VisualToolkit* vt);
	};
}