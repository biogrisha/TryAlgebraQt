#include <Me/include/MeTerm.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
namespace TryAlgebraCore
{
	void MeTerm::setMeta(const std::wstring& meta)
	{
		if (meta == MeNames::termFunction)
		{
			m_type = Type::Function;
		}
		else if (meta == MeNames::termToken)
		{
			m_type = Type::Token;
		}
	}

	void MeTerm::calculate(VisualToolkit* visual_toolkit)

	{
		float padding = 5 * m_scaling_factor;
		glm::vec2 size = { padding,0 };
		float margin = 0;
		for (auto& ch : m_children)
		{
			if (auto* cont = MyRTTI::Cast<MeContainer>(ch.get()))
			{
				cont->setDrawBorder(true);
			}
			ch->setScalingFactor(m_scaling_factor);
			ch->calculate(visual_toolkit);
			ch->setPos({ size.x + margin, padding * 4 });
			size.x = ch->getSize().x + ch->getPos().x;
			size.y = std::max(size.y, ch->getSize().y);
			margin = m_scaling_factor * 10;
		}
		setSize(size + glm::vec2{ padding, padding * 4 });
		setBearing(getSize().y / 2.);
	}

	void MeTerm::draw(VisualToolkit* vt)
	{
		if (getParent() && MyRTTI::Is<MeTerm>(getParent()->getParent()))
		{
			MeBase::draw(vt);
			return;
		}
		float padding = 2 * m_scaling_factor;
		LineChain border;
		border.width = 1;
		border.color = { 1,1,1,1 };
		border.points.push_back(getPos() + glm::vec2{ padding, padding });
		border.points.push_back(getPos() + glm::vec2(getSize().x - padding, padding));
		border.points.push_back(getPos() + getSize() - glm::vec2{ padding, 0 });
		border.points.push_back(getPos() + glm::vec2(padding, getSize().y));
		border.points.push_back(getPos() + +glm::vec2{ padding, padding });
		vt->mdocState->at(1).addLine(border);
		MeBase::draw(vt);
	}

	void MeTerm::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		if (step_from == StepFrom::inside)
		{
			path.pop_back();
			auto* contPos = std::get_if<ContPos>(&path.back());
			assert(contPos != nullptr);
			int contI = MeHelpers::contIndByPos(contPos->pos, this).value();
			if (dir == StepDir::left)
			{
				if (contI == 0)
				{
					path.pop_back();
					auto* mePos = std::get_if<MePos>(&path.back());
					assert(mePos != nullptr);
					LeafPos leafPos;
					leafPos.pos = mePos->from;
					path.back() = leafPos;
					return;
				}
				else
				{
					contPos->pos = getChildren()[contI - 1]->getChFrom();
					path.push_back(LeafPos{ contPos->pos });
					return;
				}
			}
			else if (dir == StepDir::right)
			{
				if (contI == getChildren().size() - 1)
				{
					path.pop_back();
					auto* mePos = std::get_if<MePos>(&path.back());
					assert(mePos != nullptr);
					LeafPos leafPos;
					leafPos.pos = mePos->to;
					path.back() = leafPos;
					return;
				}
				else
				{
					contPos->pos = getChildren()[contI + 1]->getChFrom();
					path.push_back(LeafPos{ contPos->pos });
					return;
				}
			}
			else if (dir == StepDir::up)
			{
				path.pop_back();
				auto* mePos = std::get_if<MePos>(&path.back());
				assert(mePos != nullptr);
				LeafPos leafPos;
				leafPos.pos = mePos->from;
				path.back() = leafPos;
				return;
			}
			else if (dir == StepDir::down)
			{
				path.pop_back();
				auto* mePos = std::get_if<MePos>(&path.back());
				assert(mePos != nullptr);
				LeafPos leafPos;
				leafPos.pos = mePos->to;
				path.back() = leafPos;
				return;
			}
		}
		else if (step_from == StepFrom::outside)
		{
			//path points at this
			path.back() = MePos(getChFrom(), getChTo());
			if (dir == StepDir::left)
			{
				auto& second_cont = getChildren().back();
				auto& cont_children = second_cont->getChildren();
				path.push_back(ContPos(second_cont->getChFrom()));
				if (cont_children.empty())
				{
					path.push_back(LeafPos(second_cont->getChFrom()));
				}
				else
				{
					path.push_back(LeafPos(cont_children.back()->getChTo()));
				}
			}
			else if (dir == StepDir::right)
			{
				path.push_back(ContPos(getChildren()[0]->getChFrom()));
				path.push_back(LeafPos(getChildren()[0]->getChFrom()));
			}
		}
	}

	std::wstring MeTerm::getName()
	{
		return MeNames::term;
	}

}
