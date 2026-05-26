#include <Helpers/include/MeHelpers.h>
#include <Me/include/MeNewLine.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeContainer.h>

namespace TryAlgebraCore
{
	std::optional<size_t> MeHelpers::absToChildPos(const MeBase* from, size_t pos)
	{
		auto& children = from->getChildren();
		if (children.empty())
		{
			return std::nullopt;
		}
		auto it = std::lower_bound(
			children.begin(),
			children.end(),
			pos,
			[](const std::unique_ptr<MeBase>& me, size_t value) {
				return me->getChFrom() < value;
			}
		);
		if(it != children.end())
		{
			if (it->get()->getChFrom() != pos)
			{
				return std::nullopt;
			}
			return std::distance(children.begin(), it);
		}
		return std::nullopt;
	}

	MeHelpers::GetByPathRes MeHelpers::getByPath(MeBase* from, const MePath& path)
	{
		GetByPathRes res;
		res.me = from;
		for(int i = 0; i < path.size() - 1; ++i)
		{
			size_t pos = getPosOrFrom(path[i]);
			auto child_pos = absToChildPos(from, pos);
			assert(child_pos.has_value());
			from = from->getChildren()[child_pos.value()].get();
		}

		auto& children = from->getChildren();
		if (children.empty())
		{
			res.me = from;
			res.status = GetByPathStatus::cont;
			return res;
		}
		size_t leaf_pos = std::get<LeafPos>(path.back()).pos;
		auto child_pos = absToChildPos(from, std::get<LeafPos>(path.back()).pos);
		if (child_pos.has_value())
		{
			res.pos = child_pos;
			res.me = from->getChildren()[child_pos.value()].get();
			res.status = GetByPathStatus::me;
		}
		else if (children.back()->getChTo() == leaf_pos)
		{
			res.me = children.back().get();
			res.status = GetByPathStatus::last;
		}
		else
		{
			res.me = from;
			res.status = GetByPathStatus::outside;
		}
		return res;
	}
	FCaretData MeHelpers::getCaretData(MeBase* from, const MePath& path)
	{
		auto res = getByPath(from, path);

		FCaretData caret_data;
		caret_data.Pos = g_invalid_caret_pos;
		caret_data.Size = g_caret_def_size;
		caret_data.Size.y *= res.me->getScalingFactor();
		switch (res.status)
		{
		case MeHelpers::GetByPathStatus::cont:
			//containter is empty
			caret_data.Pos = res.me->getPos();
			break;
		case MeHelpers::GetByPathStatus::last:
			if (MyRTTI::Is<MeNewLine>(res.me))
			{
				auto cont = res.me->getParent();
				//find next line y
				float y = res.me->getPos().y + res.me->getSize().y;
				auto& children = from->getChildren();
				for (int i = children.size() - 2; i >= 0; --i)
				{
					if (MyRTTI::Is<MeNewLine>(children[i].get()))
					{
						break;
					}
					y = std::max(y, children[i]->getPos().y + children[i]->getSize().y);
				}
				caret_data.Pos.x = 0;
				caret_data.Pos.y = y;
			}
			else
			{
				caret_data.Pos.x = res.me->getPos().x + res.me->getSize().x;
				caret_data.Pos.y = res.me->getPos().y + res.me->getBearingY() - g_caret_def_size.y / 2;
			}
			break;
		case MeHelpers::GetByPathStatus::me:
			caret_data.Pos = res.me->getPos();
			caret_data.Pos.y += res.me->getBearingY() - g_caret_def_size.y * res.me->getScalingFactor() / 2;
			break;
		default:
			break;
		}
		return caret_data;
	}

	bool MeHelpers::isWithinMe(const glm::vec2& pos, MeBase* me)
	{
		auto& me_pos = me->getPos();
		auto me_rb = me->getSize() + me_pos;
		return pos.x >= me_pos.x && pos.y >= me_pos.y && pos.x < me_rb.x && pos.y < me_rb.y;
	}

	bool MeHelpers::isLeft(const glm::vec2& pos, MeBase* me)
	{
		float center = me->getPos().x + me->getSize().x / 2;
		return pos.x < center;
	}

	bool MeHelpers::getPathAtPos(MeBase* from, const glm::vec2& pos, MePath& path)
	{
		if (isWithinMe(pos, from))
		{
			auto& children = from->getChildren();
			for (int i = 0; i < children.size(); ++i)
			{
				if (MyRTTI::Is<MeContainer>(children[i].get()))
				{
					path.push_back(ContPos(children[i]->getChFrom()));
				}
				else
				{
					path.push_back(MePos(children[i]->getChFrom(), children[i]->getChTo()));
				}
				
				if (getPathAtPos(children[i].get(), pos, path))
				{
					return true;
				}
				path.pop_back();				
			}
			if (MyRTTI::Is<MeContainer>(from))
			{
				// if pos not pointing at child, but points inside container
				// find me on the same line or last
				path.push_back(LeafPos{children.back()->getChTo()});
				for (int i = 0; i < children.size(); ++i)
				{
					if (children[i]->getPos().y + children[i]->getSize().y > pos.y)
					{
						for (; i < children.size(); ++i)
						{
							if (MyRTTI::Is<MeNewLine>(children[i].get()))
							{
								path.back() = LeafPos{children[i]->getChFrom()};
								break;
							}
						}
						break;
					}
				}
				return true;
			}
			auto last_me_pos = std::get<MePos>(path.back());
			path.back() = LeafPos{ isLeft(pos, from) ? last_me_pos.from : last_me_pos.to };
			return true;
		}
		return false;
	}
	MeBase* MeHelpers::getByTreePath(MeBase* from, const std::vector<size_t>& path)
	{
		for (size_t pos : path)
		{
			from = from->getChildren()[pos].get();
		}
		return from;
	}
	void MeHelpers::alignVertically(const std::vector<std::unique_ptr<MeBase>>& mes, float& center_x)
	{
		for (auto& me : mes)
		{
			float me_center = me->getSize().x / 2;
			me->setPosX(-me_center);
			center_x = std::max(center_x, me_center);
		}
		for (auto& me : mes)
		{
			me->setPosX(me->getPos().x + center_x);
		}
	}
}