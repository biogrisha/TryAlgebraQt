#include <Me/include/MeBase.h>

void TryAlgebraCore::MeBase::calculatePos()
{
	for (auto& ch : m_children)
	{
		ch->setPos(ch->getPos() + getPos());
		ch->calculatePos();
	}
}

void TryAlgebraCore::MeBase::addChild(std::unique_ptr<MeBase>&& child)
{
	m_children.push_back(std::move(child));
}
