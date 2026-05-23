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
			outside
		};
		struct GetByPathRes
		{
			GetByPathStatus status = GetByPathStatus::me;
			std::optional<size_t> pos;
			MeBase* me = nullptr;
		};

		static std::optional<size_t> absToChildPos(const MeBase* from, size_t pos);
		static GetByPathRes getByPath(MeBase* from, const MePath& path);
		static FCaretData getCaretData(MeBase* from, const MePath& path);

		static void updateSelection(VisualToolkit* vt, MeBase* cont, int from, int to);
		static int getAbsCaretPos(MeBase* from, const std::vector<int>& path);
		static std::vector<MePos> getAbsCaretPath(MeBase* from, const std::vector<int>& path);
		static bool isWithinMe(const glm::vec2& pos, MeBase* me);
		static bool isLeft(const glm::vec2& pos, MeBase* me);
		static bool getPathAtPos(MeBase* from, const glm::vec2& pos, std::vector<int>& path);
		static void normalizeSelection(std::vector<int>& from, std::vector<int>& to);


		//me arrangement
		static void alignVertically(const std::vector<std::unique_ptr<MeBase>>& mes, float& center_x);
	};
}