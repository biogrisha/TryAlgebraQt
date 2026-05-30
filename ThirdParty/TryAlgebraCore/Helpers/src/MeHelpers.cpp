#include <Helpers/include/MeHelpers.h>
#include <Me/include/MeNewLine.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeContainer.h>

namespace TryAlgebraCore
{
	void MeHelpers::propagateMeChange(MePath& path, int text_added)
	{
		for (auto& pos : path)
		{
			if (auto mePos = std::get_if<MePos>(&pos))
			{
				mePos->to += text_added;
			}
		}
	}
	MePath MeHelpers::textPosToPath(MeBase* from, size_t pos)
	{
		/*MePath res;
		while(true)
		{
			auto& children = from->getChildren();
			assert(!children.empty());
			auto it = std::lower_bound(
				children.begin(),
				children.end(),
				pos,
				[](const std::unique_ptr<MeBase>& me, size_t value) {
					return me->getChFrom() < value;
				}
			);
			auto me = it->get();
			if (MyRTTI::Is<MeContainer>(me))
			{
				res.push_back(ContPos{ me->getChFrom() });
			}
			if (it->get()->getChFrom() == pos)
			{

			}
		}*/
		return MePath();
	}

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
			if (!child_pos.has_value())
			{
				res.status = GetByPathStatus::outside;
				return res;
			}
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

	void MeHelpers::orderPaths(MePath& path1, MePath& path2)
	{
		for (int i = 0; i < std::min(path1.size(), path2.size()); ++i)
		{
			size_t pos1 = getPosOrFrom(path1[i]);
			size_t pos2 = getPosOrFrom(path2[i]);
			if (pos1 > pos2)
			{
				std::swap(path1, path2);
				return;
			}
			else if (pos1 < pos2)
			{
				return;
			}
		}
		if (path1.size() > path2.size())
		{
			std::swap(path1, path2);
		}
	}

	void MeHelpers::trimToCommonContainer(MePath& path1, MePath& path2)
	{
		size_t min_size = std::min(path1.size(), path2.size());
		path1.resize(min_size);
		path2.resize(min_size);

		for (int i = path1.size() - 1; i >= 2; i -= 1)
		{
			auto path1_pos = std::get_if<ContPos>(&path1[i - 1]);
			auto path2_pos = std::get_if<ContPos>(&path2[i - 1]);
			if (path1_pos && path2_pos)
			{
				if (path1_pos->pos == path2_pos->pos)
				{
					path1.back() = LeafPos(getPosOrFrom(path1.back()));
					path2.back() = LeafPos(getPosOrTo(path2.back()));
					return;
				}
				path1.resize(path1.size() - 2);
				path2.resize(path2.size() - 2);
			}		
		}
		path1.back() = LeafPos(getPosOrFrom(path1.back()));
		path2.back() = LeafPos(getPosOrTo(path2.back()));
	}

	size_t MeHelpers::getPosOrFrom(const std::variant<MePos, ContPos, LeafPos>& v)
	{
		return std::visit([](const auto& x) -> size_t
			{
				using T = std::decay_t<decltype(x)>;

				if constexpr (std::is_same_v<T, MePos>)
				{
					return x.from;
				}
				else
				{
					return x.pos;
				}
			}, v);
	}

	size_t MeHelpers::getPosOrTo(const std::variant<MePos, ContPos, LeafPos>& v)
	{
		return std::visit([](const auto& x) -> size_t
			{
				using T = std::decay_t<decltype(x)>;

				if constexpr (std::is_same_v<T, MePos>)
				{
					return x.to;
				}
				else
				{
					return x.pos;
				}
			}, v);
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
				if(children.empty())
				{
					path.push_back(LeafPos{ from->getChFrom() });
				}
				else
				{
					path.push_back(LeafPos{ children.back()->getChTo()});
				}
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
	void MeHelpers::highlightSelected(MeBase* cont, MePath from_path, MePath to_path, VisualToolkit* vt)
	{
		auto& cont_children = cont->getChildren();
		MePath min = { LeafPos{cont_children.front()->getChFrom()} };
		MePath max = { LeafPos{cont_children.back()->getChTo()} };

		orderPaths(from_path, to_path);
		orderPaths(min, from_path);
		orderPaths(to_path, max);

		trimToCommonContainer(from_path, to_path);
		auto me_from_res = getByPath(cont, from_path);
		if (me_from_res.status == GetByPathStatus::cont || me_from_res.status == GetByPathStatus::last)
		{
			return;
		}
		else if (me_from_res.status == GetByPathStatus::me)
		{
			auto& siblings = me_from_res.me->getParent()->getChildren();
			size_t i = me_from_res.pos.value();
			size_t to = std::get<LeafPos>(to_path.back()).pos;
			while (i < siblings.size() && siblings[i]->getChFrom() < to)
			{
				FRectInst rect;
				rect.Color = { 0,0,0.3,1 };
				rect.Pos = siblings[i]->getPos();
				rect.Size = siblings[i]->getSize();
				vt->mdoc_state->AddRect(rect);
				++i;
			}

		}

	}
}