#pragma once
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
namespace TryAlgebraCore
{
	inline uint32_t g_min_font_size = 5;
	inline uint32_t g_font_def_height = 20;
	inline glm::vec2 g_caret_def_size = { 2, 20 };
	inline glm::vec2 g_invalid_caret_pos = { -100, -100 };

	namespace MeNames
	{
		inline std::wstring from_to = L"ft";
		inline std::wstring integral = L"∫";
		inline std::wstring integral2 = L"∬";
		inline std::wstring integral3 = L"∭";

		inline std::wstring new_line = L"nl";

		inline std::wstring cont = L"ct";

		inline std::wstring variable = L"vr";
		inline std::wstring varZeroMulti = L"0mlt";
		inline std::wstring varOneMulti = L"1mlt";
		inline std::wstring varUni = L"uni";


		inline std::wstring term = L"trm";
		inline std::wstring termFunction = L"f";
		inline std::wstring termToken = L"tok";

		inline std::wstring power = L"pw";

		inline std::wstring fraction = L"fr";

		inline std::wstring spriteCursorPlacement = L"cur";
		inline std::wstring spritePaste = L"pst";

		inline std::wstring makeMe(const std::wstring& name, const std::wstring& meta, const size_t cont_num)
		{
			std::wstring res;
			if (meta.empty())
			{
				res = L"\\" + name + L"\\{";
			}
			else
			{
				res = L"\\" + name + L"\\" + meta + L"\\{";
			}
			for (size_t i = 0; i < cont_num - 1; ++i)
			{
				res += L"\\,";
			}
			res += L"\\}";
			return res;
		}

		inline std::vector<std::pair<std::wstring, std::wstring>>& getMeTable()
		{
			static std::vector<std::pair<std::wstring, std::wstring>> res;
			if (res.empty())
			{
				res.emplace_back(L"Integral", makeMe(from_to, integral, 2));
				res.emplace_back(L"Double integral", makeMe(from_to, integral2, 2));
				res.emplace_back(L"Triple integral", makeMe(from_to, integral3, 2));
				res.emplace_back(L"Zero-Multi variable", makeMe(variable, varZeroMulti, 1));
				res.emplace_back(L"One-Multi variable", makeMe(variable, varOneMulti, 1));
				res.emplace_back(L"Uni variable", makeMe(variable, varUni, 1));
				res.emplace_back(L"For all", L"∀");
				res.emplace_back(L"Exists", L"∃");
				res.emplace_back(L"Term2", makeMe(term, L"2", 3));
				res.emplace_back(L"Term1", makeMe(term, L"1", 2));
				res.emplace_back(L"Token", makeMe(term, termToken, 1));
				res.emplace_back(L"Power", makeMe(power, L"", 1));
				res.emplace_back(L"Fraction", makeMe(fraction, L"", 2));
				res.emplace_back(L"Cursor placement", L'\\' + spriteCursorPlacement + L"\\\\");
				res.emplace_back(L"Paste selection", L'\\' + spritePaste + L"\\\\");
				res.emplace_back(L"textalpha", L"α");
				res.emplace_back(L"textAlpha", L"Α");

				res.emplace_back(L"textbeta", L"β");
				res.emplace_back(L"textBeta", L"Β");

				res.emplace_back(L"textgamma", L"γ");
				res.emplace_back(L"textGamma", L"Γ");

				res.emplace_back(L"textdelta", L"δ");
				res.emplace_back(L"textDelta", L"Δ");

				res.emplace_back(L"textepsilon", L"ε");
				res.emplace_back(L"textEpsilon", L"Ε");

				res.emplace_back(L"textzeta", L"ζ");
				res.emplace_back(L"textZeta", L"Ζ");

				res.emplace_back(L"texteta", L"η");
				res.emplace_back(L"textEta", L"Η");

				res.emplace_back(L"texttheta", L"θ");
				res.emplace_back(L"textTheta", L"Θ");

				res.emplace_back(L"textiota", L"ι");
				res.emplace_back(L"textIota", L"Ι");

				res.emplace_back(L"textkappa", L"κ");
				res.emplace_back(L"textKappa", L"Κ");

				res.emplace_back(L"textlambda", L"λ");
				res.emplace_back(L"textLambda", L"Λ");

				res.emplace_back(L"textmu", L"μ");
				res.emplace_back(L"textMu", L"Μ");

				res.emplace_back(L"textnu", L"ν");
				res.emplace_back(L"textNu", L"Ν");

				res.emplace_back(L"textxi", L"ξ");
				res.emplace_back(L"textXi", L"Ξ");

				res.emplace_back(L"textomicron", L"ο");
				res.emplace_back(L"textOmicron", L"Ο");

				res.emplace_back(L"textpi", L"π");
				res.emplace_back(L"textPi", L"Π");

				res.emplace_back(L"textrho", L"ρ");
				res.emplace_back(L"textRho", L"Ρ");

				res.emplace_back(L"textsigma", L"σ");
				res.emplace_back(L"textSigma", L"Σ");

				res.emplace_back(L"texttau", L"τ");
				res.emplace_back(L"textTau", L"Τ");

				res.emplace_back(L"textupsilon", L"υ");
				res.emplace_back(L"textUpsilon", L"Υ");

				res.emplace_back(L"textphi", L"φ");
				res.emplace_back(L"textPhi", L"Φ");

				res.emplace_back(L"textchi", L"χ");
				res.emplace_back(L"textChi", L"Χ");

				res.emplace_back(L"textpsi", L"ψ");
				res.emplace_back(L"textPsi", L"Ψ");

				res.emplace_back(L"textomega", L"ω");
				res.emplace_back(L"textOmega", L"Ω");

				res.emplace_back(L"textvarepsilon", L"ϵ");
				res.emplace_back(L"textvartheta", L"ϑ");
				res.emplace_back(L"textvarpi", L"ϖ");
				res.emplace_back(L"textvarrho", L"ϱ");
				res.emplace_back(L"textvarsigma", L"ς");
				res.emplace_back(L"textvarphi", L"ϕ");

				// Calculus / analysis
				res.emplace_back(L"infty", L"∞");
				res.emplace_back(L"partial", L"∂");
				res.emplace_back(L"nabla", L"∇");
				res.emplace_back(L"prime", L"′");

				// Relations
				res.emplace_back(L"neq", L"≠");
				res.emplace_back(L"approx", L"≈");
				res.emplace_back(L"equiv", L"≡");
				res.emplace_back(L"sim", L"∼");
				res.emplace_back(L"simeq", L"≃");

				res.emplace_back(L"leq", L"≤");
				res.emplace_back(L"geq", L"≥");
				res.emplace_back(L"ll", L"≪");
				res.emplace_back(L"gg", L"≫");

				res.emplace_back(L"propto", L"∝");

				// Sets
				res.emplace_back(L"in", L"∈");
				res.emplace_back(L"notin", L"∉");
				res.emplace_back(L"ni", L"∋");

				res.emplace_back(L"subset", L"⊂");
				res.emplace_back(L"supset", L"⊃");
				res.emplace_back(L"subseteq", L"⊆");
				res.emplace_back(L"supseteq", L"⊇");

				res.emplace_back(L"nsubset", L"⊄");
				res.emplace_back(L"nsupset", L"⊅");

				res.emplace_back(L"emptyset", L"∅");

				// Logic
				res.emplace_back(L"forall", L"∀");
				res.emplace_back(L"exists", L"∃");
				res.emplace_back(L"nexists", L"∄");

				res.emplace_back(L"neg", L"¬");
				res.emplace_back(L"land", L"∧");
				res.emplace_back(L"lor", L"∨");

				res.emplace_back(L"therefore", L"∴");
				res.emplace_back(L"because", L"∵");

				// Arithmetic / algebra
				res.emplace_back(L"plusminus", L"±");
				res.emplace_back(L"minusplus", L"∓");
				res.emplace_back(L"times", L"×");
				res.emplace_back(L"div", L"÷");
				res.emplace_back(L"cdot", L"⋅");

				res.emplace_back(L"circ", L"∘");
				res.emplace_back(L"bullet", L"•");

				res.emplace_back(L"oplus", L"⊕");
				res.emplace_back(L"ominus", L"⊖");
				res.emplace_back(L"otimes", L"⊗");
				res.emplace_back(L"oslash", L"⊘");
				res.emplace_back(L"odot", L"⊙");

				// Geometry
				res.emplace_back(L"angle", L"∠");
				res.emplace_back(L"measuredangle", L"∡");
				res.emplace_back(L"perp", L"⊥");
				res.emplace_back(L"parallel", L"∥");
				res.emplace_back(L"nparallel", L"∦");

				// Arrows
				res.emplace_back(L"leftarrow", L"←");
				res.emplace_back(L"rightarrow", L"→");
				res.emplace_back(L"leftrightarrow", L"↔");

				res.emplace_back(L"Leftarrow", L"⇐");
				res.emplace_back(L"Rightarrow", L"⇒");
				res.emplace_back(L"Leftrightarrow", L"⇔");

				res.emplace_back(L"uparrow", L"↑");
				res.emplace_back(L"downarrow", L"↓");
				res.emplace_back(L"updownarrow", L"↕");

				res.emplace_back(L"mapsto", L"↦");

				// Miscellaneous
				res.emplace_back(L"degree", L"°");
				res.emplace_back(L"ellipsis", L"…");
				res.emplace_back(L"cdots", L"⋯");
				res.emplace_back(L"vdots", L"⋮");
				res.emplace_back(L"ddots", L"⋱");

				res.emplace_back(L"aleph", L"ℵ");
				res.emplace_back(L"hbar", L"ℏ");
				res.emplace_back(L"ell", L"ℓ");
				res.emplace_back(L"Re", L"ℜ");
				res.emplace_back(L"Im", L"ℑ");
				res.emplace_back(L"wp", L"℘");

				// Common mathematical constants / symbols
				res.emplace_back(L"sqrt_symbol", L"√");
				res.emplace_back(L"cuberoot", L"∛");
				res.emplace_back(L"fourthroot", L"∜");

				// Proof / definition
				res.emplace_back(L"qed", L"∎");
				res.emplace_back(L"definedas", L"≔");
				res.emplace_back(L"corresponds", L"≙");
			}
			return res;
		}
	}
}