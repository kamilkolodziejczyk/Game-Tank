#ifndef TANK_HPP_INCLUDED
#define TANK_HPP_INCLUDED

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Command.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"


#include <SFML/Graphics/Sprite.hpp>


class Tank : public Entity
{
	public:
		enum Type
		{
			Tank1,
			Tank2,
			TypeCount
		};


	public:
                                Tank(Type type, const TextureHolder& textures, const FontHolder& fonts);
        virtual unsigned int	getCategory() const;
        Type                    getType()const;
        virtual sf::FloatRect	getBoundingRect() const;
		virtual bool 			isMarkedForRemoval() const;
		bool					isAllied() const;
		float					getMaxSpeed() const;
		void                    setDirection();
		sf::Vector2f            getDirection() const;
		void                    setTraveledDistance(float distance);

		void					increaseFireRate();
		void					increaseSpread();
		void					collectMissiles(unsigned int count);

		void 					fire();
		void					launchMissile();


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
		void					updateMovementPattern(sf::Time dt);
		void					checkPickupDrop(CommandQueue& commands);
		void					checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

		void					createBullets(SceneNode& node, const TextureHolder& textures) const;
		void					createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
		void					createPickup(SceneNode& node, const TextureHolder& textures) const;

		void					updateTexts();

	private:
	    sf::Vector2f            mDirection;
		Type					mType;
		sf::Sprite				mSprite;
		Command 				mFireCommand;
		Command					mMissileCommand;
		sf::Time				mFireCountdown;
		bool 					mIsFiring;
		bool					mIsLaunchingMissile;
		bool 					mIsMarkedForRemoval;

		int						mFireRateLevel;
		int						mSpreadLevel;
		int						mMissileAmmo;

		Command 				mDropPickupCommand;
		float					mTravelledDistance;
		TextNode*				mHealthDisplay;
		TextNode*				mMissileDisplay;
};

#endif // TANK_HPP_INCLUDED
