#include "World.hpp"
#include "SpriteNode.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "TextNode.hpp"
#include <SFML/Graphics/RenderWindow.hpp>


World::World(sf::RenderWindow& window, FontHolder& fonts)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mFonts(fonts)
, mTextures()
, mSceneGraph()
, mSceneLayers()
, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.f)
, mSpawnPosition(mWorldView.getSize().x / 2.f,  mWorldView.getSize().y / 2.f)
, mPlayerTank(nullptr)
, mEnemySpawnPoints()
, mActiveEnemies()
{
	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	mPlayerTank->setVelocity(0.f, 0.f);

	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Collision detection and response (may destroy entities)
	handleCollisions();
    mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt,mCommandQueue);
	adaptPlayerPosition();

	spawnEnemies();

	spawnTerrains();
}

void World::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}
bool World::hasAlivePlayer() const
{
	return !mPlayerTank->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !mWorldBounds.contains(mPlayerTank->getPosition());
}

void World::loadTextures()
{
	mTextures.load(Textures::Tank1, "Media/Textures/Tank.png");
	mTextures.load(Textures::Desert, "Media/Textures/Desert.png");
	mTextures.load(Textures::Tank2, "Media/Textures/enemy.png");

	mTextures.load(Textures::Bullet, "Media/Textures/Bullet.png");
	mTextures.load(Textures::Missile, "Media/Textures/Missile.png");

	mTextures.load(Textures::Brick, "Media/Textures/Brick.png");
	mTextures.load(Textures::Eagle, "Media/Textures/Eagle.png");
	mTextures.load(Textures::Steel, "Media/Textures/Steel.png");

	mTextures.load(Textures::HealthRefill, "Media/Textures/HealthRefill.png");
	mTextures.load(Textures::MissileRefill, "Media/Textures/MissileRefill.png");
	mTextures.load(Textures::FireSpread, "Media/Textures/FireSpread.png");
	mTextures.load(Textures::FireRate, "Media/Textures/FireRate.png");


}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == Ground) ? Category::SceneGroundLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Desert);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add player's Tank
	std::unique_ptr<Tank> player(new Tank(Tank::Tank1, mTextures,mFonts));
	mPlayerTank = player.get();
	mPlayerTank->setPosition(mSpawnPosition);
	mPlayerTank->setVelocity(40.f, 40.f);
	mSceneLayers[Ground]->attachChild(std::move(player));

    addTerrains();

	// Add enemy tank
	addEnemies();


}

void World::addEnemies()
{
	// Add enemies to the spawn point container

	addEnemy(Tank::Tank2,  100.f, 200.f);

	addEnemy(Tank::Tank2, 330.f, 330.f);


	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.y < rhs.y;
	});
}

void World::addTerrains()
{
    addTerrain(Terrain::Brick, 100.f, -100.f);
    addTerrain(Terrain::Steel, 330.f, -330.f);
     addTerrain(Terrain::Brick, 100.f, -132.f);
    addTerrain(Terrain::Steel, 430.f, -330.f);
     addTerrain(Terrain::Brick, 132.f, -132.f);
    addTerrain(Terrain::Steel, 530.f, -330.f);
     addTerrain(Terrain::Brick, 400.f, -100.f);
    addTerrain(Terrain::Eagle, 683.f, -736.f);
     addTerrain(Terrain::Brick, 619.f, -736.f);
      addTerrain(Terrain::Brick, 619.f, -704.f);
       addTerrain(Terrain::Brick, 619.f, -672.f);
        addTerrain(Terrain::Brick, 651.f, -672.f);
       addTerrain(Terrain::Brick, 683.f, -672.f);
        addTerrain(Terrain::Brick, 715.f, -672.f);
        addTerrain(Terrain::Brick, 747.f, -672.f);
         addTerrain(Terrain::Brick, 747.f, -704.f);
          addTerrain(Terrain::Brick, 747.f, -736.f);



    std::sort(mTerrainSpawnPoints.begin(), mTerrainSpawnPoints.end(), [] (SpawnPointTerrain lhs, SpawnPointTerrain rhs)
              {
                  return lhs.y < rhs.y;
              });
}

void World::addEnemy(Tank::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}
void World::addTerrain(Terrain::Type type, float relX, float relY)
{
    SpawnPointTerrain spawn(type, mTerrainSpawnPosition.x + relX, mTerrainSpawnPosition.y - relY);
    mTerrainSpawnPoints.push_back(spawn);
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for(SceneNode::Pair pair: collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerTank, Category::EnemyTank))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& enemy = static_cast<Tank&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}

		else if (matchesCategories(pair, Category::PlayerTank, Category::Pickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
		}

		else if (matchesCategories(pair, Category::EnemyTank, Category::AlliedProjectile)
			  || matchesCategories(pair, Category::PlayerTank, Category::EnemyProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);


			tank.damage(projectile.getDamage());
			projectile.destroy();
		}
		else if(matchesCategories(pair,  Category::Brick,Category::AlliedProjectile)|| matchesCategories(pair,Category::Eagle,Category::AlliedProjectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }
        else if(matchesCategories(pair, Category::Brick, Category::Projectile)  || matchesCategories(pair, Category::Eagle,Category::Projectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }

        else if(matchesCategories(pair, Category::Brick, Category::EnemyProjectile)  || matchesCategories(pair, Category::Eagle,Category::EnemyProjectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }
         else if(matchesCategories(pair, Category::EnemyProjectile, Category::Steel)  || matchesCategories(pair,Category::Projectile, Category::Steel )  || matchesCategories(pair,Category::AlliedProjectile, Category::Steel ))
        {
            auto& projectile = static_cast<Projectile&>(*pair.first);
            auto& steel = static_cast<Terrain&>(*pair.second);



            projectile.destroy();
        }
	}
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayerTank->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerTank->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerTank->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f)
		mPlayerTank->setVelocity(velocity.x,0.f);
    else if( velocity.y != 0.f)
        mPlayerTank->setVelocity(0.f,velocity.y);

}
CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mEnemySpawnPoints.empty()
		&& mEnemySpawnPoints.back().y > getBattlefieldBounds().top)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();

		std::unique_ptr<Tank> enemy(new Tank(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);

		mSceneLayers[Ground]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}
}
void World::spawnTerrains()
{
    while (!mTerrainSpawnPoints.empty()
		&& mTerrainSpawnPoints.back().y > getBattlefieldBounds().top)
	{
		SpawnPointTerrain spawn = mTerrainSpawnPoints.back();

		std::unique_ptr<Terrain> terra(new Terrain(spawn.type, mTextures));
        terra->setPosition(spawn.x, spawn.y);


		mSceneLayers[Ground]->attachChild(std::move(terra));

		mTerrainSpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyTank;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.destroy();
	});

	mCommandQueue.push(command);
}

void World::guideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyTank;
	enemyCollector.action = derivedAction<Tank>([this] (Tank& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
	});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this] (Projectile& missile, sf::Time)
	{
		// Ignore unguided bullets
		if (!missile.isGuided())
			return;

		float minDistance = std::numeric_limits<float>::max();
		Tank* closestEnemy = nullptr;

		// Find closest enemy
		for(Tank* enemy: mActiveEnemies)
		{
			float enemyDistance = distance(missile, *enemy);

			if (enemyDistance < minDistance)
			{
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}

		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
