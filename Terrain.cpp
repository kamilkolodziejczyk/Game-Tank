#include "Terrain.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.hpp"
#include "Category.hpp"
#include "CommandQueue.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

namespace
{
	const std::vector<TerrainData> Table = initializeTerrainData();
}


Terrain::Terrain(Type type, const TextureHolder& textures, int lvl)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture))
, mLvl(lvl)
{
	centerOrigin(mSprite);
}

Terrain::Type Terrain::getType() const
{
    return mType;
}
int Terrain::getLvl() const
{
    return mLvl;
}
unsigned int Terrain::getCategory() const
{
    if(mType == Brick)
        return Category::Brick;
	else if(mType == Steel)
        return Category::Steel;
    else if(mType == Eagle)
        return Category::Eagle;
}

sf::FloatRect Terrain::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Terrain::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}
