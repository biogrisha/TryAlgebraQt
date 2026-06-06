#include <Me/include/MeBase.h>

void TryAlgebraCore::MeBase::calculatePos()
{
	for (auto& ch : m_children)
	{
		ch->setPos(ch->getPos() + getPos());
		ch->calculatePos();
	}
}

void TryAlgebraCore::MeBase::draw(VisualToolkit* visual_toolkit)
{
	for (auto& ch : m_children)
	{
		ch->draw(visual_toolkit);
	}
}

void TryAlgebraCore::MeBase::addChild(std::unique_ptr<MeBase>&& child)
{
	m_children.push_back(std::move(child));
}
