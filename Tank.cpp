#include "Tank.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "ResourceHolder.hpp"
#include <iostream>
#include "SoundNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


namespace
{
	const std::vector<TankData> Table = initializeTankData();
}

Tank::Tank(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture))
, mFireCommand()
, mMissileCommand()
, mFireCountdown(sf::Time::Zero)
, mIsFiring(false)
, mIsLaunchingMissile(false)
, mIsMarkedForRemoval(false)
, mFireRateLevel(1)
, mSpreadLevel(1)
, mMissileAmmo(2)
, mDropPickupCommand()
, mTravelledDistance(0.f)
, mHealthDisplay(nullptr)
, mDirection(0,-1)
, mMissileDisplay(nullptr)
{
	centerOrigin(mSprite);

	mFireCommand.category = Category::SceneGroundLayer;
	mFireCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::SceneGroundLayer;
	mMissileCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = Category::SceneGroundLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerTank)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

void Tank::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Tank::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);

		mIsMarkedForRemoval = true;
		return;
	}
    setDirection();
	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt, commands);

	Entity::updateCurrent(dt, commands);

	// Update texts
	updateTexts();
}

unsigned int Tank::getCategory() const
{
	if (isAllied())
		return Category::PlayerTank;
	else
		return Category::EnemyTank;
}
Tank::Type Tank:: getType()const
{
    return mType;
}

sf::FloatRect Tank::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Tank::isMarkedForRemoval() const
{
	return mIsMarkedForRemoval;
}

bool Tank::isAllied() const
{
	return mType == Tank1;
}

float Tank::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Tank::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Tank::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Tank::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Tank::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Tank::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}
void Tank::setTraveledDistance(float distance)
{
    mTravelledDistance=distance;
}
void Tank::updateMovementPattern(sf::Time dt, CommandQueue& commands)
{
	// Enemy airplane: Movement pattern
    if(!isAllied())
    {


            // Moved long enough in current direction: Change direction
            if (mTravelledDistance > Table[mType].distance)
            {

                switch(rand()%4)
                {
                    case direction::Up:
                        if((int)mDirection.x!=0||(int)mDirection.y!=-1)
                        {
                            mDirection.x=0;
                            mDirection.y=-1;
                        }
                        else
                        {
                            mDirection.x=0;
                            mDirection.y=1;
                        }
                        break;
                    case direction::Down:
                        if((int)mDirection.x!=0||(int)mDirection.y!=1)
                        {
                            mDirection.x=0;
                            mDirection.y=1;
                        }
                        else
                        {
                            mDirection.x=0;
                            mDirection.y=-1;
                        }
                        break;
                    case direction::Left:
                        if((int)mDirection.x!=-1||(int)mDirection.y!=0)
                        {
                            mDirection.x=-1;
                            mDirection.y=0;
                        }
                        else
                        {
                            mDirection.x=1;
                            mDirection.y=0;
                        }
                        break;
                    case direction::Right:
                        if((int)mDirection.x!=1||(int)mDirection.y!=0)
                        {
                            mDirection.x=1;
                            mDirection.y=0;
                        }
                        else
                        {
                            mDirection.x=-1;
                            mDirection.y=0;
                        }
                        break;
                }
                mTravelledDistance = 0.f;
            }

            // Compute velocity from direction
            setVelocity(mDirection*getMaxSpeed());

            mTravelledDistance += getMaxSpeed() * dt.asSeconds();
    }



}

void Tank::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0)
		commands.push(mDropPickupCommand);
}

void Tank::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		playLocalSound(commands, SoundEffect::LaunchMissile);
		mIsLaunchingMissile = false;
	}
}

void Tank::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (mSpreadLevel)
	{
		case 1:
			createProjectile(node, type, 0.0f, 0.5f, textures);
			break;

		case 2:
			createProjectile(node, type, -0.33f, 0.33f, textures);
			createProjectile(node, type, +0.33f, 0.33f, textures);
			break;

		case 3:
			createProjectile(node, type, -0.5f, 0.33f, textures);
			createProjectile(node, type,  0.0f, 0.5f, textures);
			createProjectile(node, type, +0.5f, 0.33f, textures);
			break;
	}
}

void Tank::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());;
    float sign =1;
    if(mDirection.x!=0)
    {
        offset.x=yOffset * mSprite.getGlobalBounds().height;
        offset.y=xOffset * mSprite.getGlobalBounds().width;
        velocity.x=projectile->getMaxSpeed();
        velocity.y=0.f;
        sign = mDirection.x;
    }

	else if(mDirection.y!=0)
    {
        offset.x=xOffset * mSprite.getGlobalBounds().width;
        offset.y=yOffset * mSprite.getGlobalBounds().height;
        velocity.x=0.f;
        velocity.y = projectile->getMaxSpeed();
        sign = mDirection.y;
    }

	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Tank::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Tank::updateTexts()
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	if (mMissileDisplay)
	{
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}
void Tank::setDirection()
{
    sf::Vector2f velocity= getVelocity();
    if(velocity.x!=0 || velocity.y!=0)
        mDirection=unitVector(velocity);
}
sf::Vector2f Tank::getDirection()const
{
    return mDirection;
}

void Tank::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition] (SoundNode& node, sf::Time)
		{
			node.playSound(effect, worldPosition);
		});

	commands.push(command);
}
