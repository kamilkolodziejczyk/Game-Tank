#ifndef PICKUP_HPP_INCLUDED
#define PICKUP_HPP_INCLUDED


#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Tank;

class Pickup : public Entity
{
	public:
		enum Type
		{
			HealthRefill,
			MissileRefill,
			FireSpread,
			FireRate,
			TypeCount
		};


	public:
								Pickup(Type type, const TextureHolder& textures);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;

		void 					apply(Tank& player) const;


	protected:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		Type 					mType;
		sf::Sprite				mSprite;
};

#endif // PICKUP_HPP_INCLUDED
