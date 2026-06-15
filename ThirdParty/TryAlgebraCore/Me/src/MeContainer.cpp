#include <Me/include/MeContainer.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeNewLine.h>
#include <Helpers/include/MeHelpers.h>
#include <algorithm>
#include <iostream>
namespace TryAlgebraCore
{
	void MeContainer::calcLine(VisualToolkit* visual_toolkit, size_t end)
	{
		float max_bearing = 0;
		float x = 0;
		for (int i = end_line_i; i < std::min(m_children.size(), end); ++i)
		{
			auto& ch = m_children[i];
			ch->setScalingFactor(m_scaling_factor);
			ch->calculate(visual_toolkit);
			max_bearing = std::max(max_bearing, ch->getBearingY());
			ch->setPosY(-ch->getBearingY());
			ch->setPosX(x);
			x += ch->getSize().x;
		}
		float max_y_offset = 0;
		for (int i = end_line_i; i < std::min(m_children.size(), end); ++i)
		{
			auto& ch = m_children[i];
			ch->setPosY(ch->getPos().y + next_line_y + max_bearing);
			max_y_offset = std::max(max_y_offset, ch->getPos().y + ch->getSize().y);
		}
		next_line_y = max_y_offset;
		m_size.x = std::max(x, m_size.x);
		m_size.y = next_line_y;
		end_line_i = std::min(m_children.size(), end);
	}
	void MeContainer::addEmptyLine()
	{
		m_size.y += g_caret_def_size.y * m_scaling_factor;
	}

	void MeContainer::draw(VisualToolkit* vt)
	{
		//FRectInst rect;
		//rect.Color = { 1,1,1,0.5 };
		//rect.Pos = getPos();
		//rect.Size = getSize();
		//vt->mdocState->AddRect(rect);
		MeBase::draw(vt);
	}

	void MeContainer::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		auto res = MeHelpers::getByPath(this, path);
		auto parent_cont = res.status == MeHelpers::GetByPathStatus::cont ? res.me : res.me->getParent();
		assert(parent_cont);
		auto grandparent = parent_cont->getParent();
		if (res.status == MeHelpers::GetByPathStatus::cont
			|| (res.status == MeHelpers::GetByPathStatus::last && dir == StepDir::right)
			|| (res.status == MeHelpers::GetByPathStatus::me && res.pos == 0 && dir == StepDir::left)

			|| (dir == StepDir::down && (res.status == MeHelpers::GetByPathStatus::last
				|| MeHelpers::isLastLine(parent_cont, res.pos.value())))

			|| (dir == StepDir::up && (res.pos.has_value() &&
				MeHelpers::isFirstLine(parent_cont, res.pos.value())
				|| (res.status == MeHelpers::GetByPathStatus::last && MeHelpers::isFirstLine(parent_cont, parent_cont->getChildren().size()))
				)))
		{
			//this means that cont is empty
			//step left will chose next cont
			if (!grandparent)
			{
				return;
			}
			grandparent->step(dir, StepFrom::inside, path);
		}
		else if (dir == StepDir::right)
		{
			auto next_me = res.me;
			if (next_me->getChildren().empty())
			{
				path.back() = LeafPos(next_me->getChTo());
			}
			else
			{
				next_me->step(dir, StepFrom::outside, path);
			}
		}
		else if (dir == StepDir::left)
		{
			MeBase* prev = nullptr;
			if (res.status == MeHelpers::GetByPathStatus::last)
			{
				prev = res.me;
			}
			else
			{
				prev = parent_cont->getChildren()[res.pos.value() - 1].get();
			}
			if (prev->getChildren().empty())
			{
				path.back() = LeafPos(prev->getChFrom());
			}
			else
			{
				prev->step(dir, StepFrom::outside, path);
			}
		}
		else if (dir == StepDir::down)
		{
			auto& siblings = parent_cont->getChildren();
			assert(res.pos.has_value());
			int i = res.pos.value();
			for (; i < siblings.size(); ++i)
			{
				if (MyRTTI::Is<MeNewLine>(siblings[i]))
				{
					++i;
					for (; i < siblings.size(); ++i)
					{
						if (siblings[i]->getPos().x > res.me->getPos().x)
						{
							i--;
							break;
						}
						else if (MyRTTI::Is<MeNewLine>(siblings[i]))
						{
							break;
						}
					}
					break;
				}
			}
			if (i < siblings.size())
			{
				path.back() = LeafPos(siblings[i]->getChFrom());
			}
			else
			{
				path.back() = LeafPos(siblings.back()->getChTo());
			}
		}
		else if (dir == StepDir::up)
		{
			auto& siblings = parent_cont->getChildren();
			int i = res.pos.has_value() ? res.pos.value() : siblings.size();
			--i;
			float pos_x = res.status == MeHelpers::GetByPathStatus::last ? res.me->getPos().x + res.me->getSize().x : res.me->getPos().x;
			for (; i >= 0; --i)
			{
				if (MyRTTI::Is<MeNewLine>(siblings[i]))
				{
					if (siblings[i]->getPos().x <= pos_x)
					{
						break;
					}
					--i;
					for (; i >= 0; --i)
					{
						if (siblings[i]->getPos().x <= pos_x)
						{
							break;
						}
						else if (MyRTTI::Is<MeNewLine>(siblings[i]))
						{
							break;
						}
					}
					break;
				}
			}
			if (i < siblings.size())
			{
				path.back() = LeafPos(siblings[i]->getChFrom());
			}
			else
			{
				path.back() = LeafPos(siblings.back()->getChTo());
			}
		}

	}
	void MeContainer::calculate(VisualToolkit* visual_toolkit)
	{
		bool ends_with_new_line = false;
		for (int i = 0; i < m_children.size(); ++i)
		{
			if (MyRTTI::Is<MeNewLine>(m_children[i].get()))
			{
				calcLine(visual_toolkit, i + 1);
				ends_with_new_line = (i == m_children.size() - 1);
			}
		}
		if (ends_with_new_line)
		{
			addEmptyLine();
		}
		else
		{
			calcLine(visual_toolkit);
		}
		setSizeX(std::max(getSize().x, m_scaling_factor * 10));
		setSizeY(std::max(getSize().y, m_scaling_factor * g_caret_def_size.y));
	}
	std::wstring MeContainer::getName()
	{
		return MeNames::cont;
	}
}
