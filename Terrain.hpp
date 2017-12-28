#ifndef TERRAIN_HPP_INCLUDED
#define TERRAIN_HPP_INCLUDED

#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.hpp"
#include "Category.hpp"
#include "CommandQueue.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "Entity.hpp"
#include <SFML/Graphics/Sprite.hpp>

class Terrain : public Entity
{
public:
    enum Type{
        Brick,
        Steel,
        TypeCount,
        };
public:
                                        Terrain(Type type, const TextureHolder& textures);
    virtual unsigned int                getCategory() const;
    virtual sf::FloatRect               getBoundingRect() const;

protected:
    virtual void                        drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const;

private:
    Type mType;
    sf::Sprite mSprite;
};

#endif // TERRAIN_HPP_INCLUDED
