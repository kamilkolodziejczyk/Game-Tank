#include "World.hpp"
#include "SpriteNode.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "TextNode.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>
#include "SoundNode.hpp"

World::World(sf::RenderWindow& window, FontHolder& fonts, SoundPlayer& sounds)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mFonts(fonts)
, mTextures()
, mSceneGraph()
, mSounds(sounds)
, mSceneLayers()
, enemySpawned(false)
, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.f)
, mSpawnPosition(mWorldView.getSize().x / 2.f,  mWorldView.getSize().y / 2.f)
, mTerrainSpawnPosition(0,0)
, mPlayerTank(nullptr)
, mEnemySpawnPoints()
, mActiveEnemies()
, mActiveTerrain()
, mActiveProjectile()
, mActivePickups()
, mCurrLvl(1)
, mMaxLvl(10)
, mEagleAlive(true)
{
	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{

       spawnEnemies();

	// Scroll the world, reset player velocity
	mPlayerTank->setVelocity(0.f, 0.f);

	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

    collision();
	// Collision detection and response (may destroy entities)
	//handleCollisions();

    mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt,mCommandQueue);
	adaptPlayerPosition(*mPlayerTank);
	adaptPlayerPosition();


	updateSounds();
}
void World::edit(sf::Time dt)
{

    if(mPlayerTank!=nullptr)
    {
        mPlayerTank->destroy();
        mPlayerTank=nullptr;
    }
	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);


	// Collision detection and response (may destroy entities)
	//handleCollisions();
    mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt,mCommandQueue);



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
    if(mCurrLvl==mMaxLvl&&mActiveEnemies.empty()&&mEnemySpawnPoints.empty())
        return true;
	return false;
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


    std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));

	// Add player's Tank
	std::unique_ptr<Tank> player(new Tank(Tank::Tank1, mTextures,mFonts));
	mPlayerTank = player.get();
	mPlayerTank->setPosition(mSpawnPosition);
	mPlayerTank->setVelocity(40.f, 40.f);
	mSceneLayers[Ground]->attachChild(std::move(player));

    addTerrains("map.bin");

	spawnTerrains();
	// Add enemy tank
	addEnemies();



}

void World::addEnemies()
{
	// Add enemies to the spawn point container
	switch(mCurrLvl)
	{
	    case 1:for(int i=0;i<2;i++)
                {
                    int n=1+rand()%5;
                    addEnemy(Tank::Tank2, n);
                }
                break;
        case 2:for(int i=0;i<3;i++)
                {
                    int n=1+rand()%5;
                    addEnemy(Tank::Tank2, n);
                }
                break;
        default:for(int i=0;i<2;i++)
                {
                    int n=1+rand()%5;
                    addEnemy(Tank::Tank2, n);
                }
                break;

	}





	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
}


void World::addEnemy(Tank::Type type,int n)
{
	SpawnPoint spawn(type, n);
	mEnemySpawnPoints.push_back(spawn);
}
void World::addTerrain(Terrain::Type type, float relX, float relY, int lvl)
{
    SpawnPointTerrain spawn(type, relX, relY,lvl);
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

bool World::isNextLvl()
{
    if(mActiveEnemies.empty())
        return true;

    return false;
}
void World::nextLvl()
{
    if(mCurrLvl<mMaxLvl)
    {
        mCurrLvl+=1;
        clearWorld();
        //spawnTerrains();
        addEnemies();
        if(mPlayerTank!=nullptr)
        mPlayerTank->setPosition(mSpawnPosition);
    }

}
void World::prevLvl()
{
    if(mCurrLvl>1)
    {
        mCurrLvl-=1;
        destroyTerrain();
        //spawnTerrains();

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
//			updateAciveEnemys();
		}

		else if (matchesCategories(pair, Category::PlayerTank, Category::Pickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
		}

		else if (matchesCategories(pair, Category::EnemyTank, Category::AlliedProjectile)
			  || matchesCategories(pair, Category::PlayerTank, Category::EnemyProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);



			tank.damage(projectile.getDamage());
			projectile.destroy();
//			updateAciveEnemys();


		}
		else if(matchesCategories(pair,  Category::Brick,Category::AlliedProjectile)
          || matchesCategories(pair,Category::Eagle,Category::AlliedProjectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }
        else if(matchesCategories(pair, Category::Brick, Category::Projectile)
                || matchesCategories(pair, Category::Eagle,Category::Projectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }

        else if(matchesCategories(pair, Category::Brick, Category::EnemyProjectile)
                || matchesCategories(pair, Category::Eagle,Category::EnemyProjectile ))
        {
            auto& brick = static_cast<Terrain&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);


            brick.destroy();
            projectile.destroy();
        }
         else if(matchesCategories(pair, Category::EnemyProjectile, Category::Steel)
                 || matchesCategories(pair,Category::Projectile, Category::Steel )
                || matchesCategories(pair,Category::AlliedProjectile, Category::Steel ))
        {
            auto& projectile = static_cast<Projectile&>(*pair.first);
            auto& steel = static_cast<Terrain&>(*pair.second);



            projectile.destroy();
        }
         else if(matchesCategories(pair,  Category::Terrain,Category::Terrain))
        {
            auto& terrain1 = static_cast<Terrain&>(*pair.first);
            auto& terrain2 = static_cast<Terrain&>(*pair.second);


            terrain1.destroy();
            terrain2.destroy();
            //eraseDuplicates(terrain1);
           // eraseDuplicates(terrain2);
        }
        else if(matchesCategories(pair, Category::PlayerTank, Category::Steel)
                 || matchesCategories(pair,Category::PlayerTank, Category::Brick )
                || matchesCategories(pair,Category::PlayerTank, Category::Eagle ))
        {
            auto& player = static_cast<Tank&>(*pair.first);
            auto& terr = static_cast<Terrain&>(*pair.second);

//            adaptPlayerPosition(mPlayerTank,terr);


        }
        else if(matchesCategories(pair, Category::EnemyTank, Category::Steel)
                 || matchesCategories(pair,Category::EnemyTank, Category::Brick )
                || matchesCategories(pair,Category::EnemyTank, Category::Eagle ))
        {
            auto& enemy = static_cast<Tank&>(*pair.first);


            enemy.setTraveledDistance(std::numeric_limits<float>::max());


        }
	}
}
void World::updateActiveEnemys()
{

    for (auto it = mActiveEnemies.begin(); it != mActiveEnemies.end(); ) {
            if ( it != mActiveEnemies.end()&&(*it)->isDestroyed()) {
            it = mActiveEnemies.erase(it);
              } else {
                    ++it;
                }
            }
}

void World::updateSounds()
{
    mSounds.setListenerPosition(mPlayerTank->getWorldPosition());

    mSounds.removeStoppedSounds();
}
void World::adaptPlayerPosition(Tank* tank,Terrain *terr)
{
    sf::FloatRect terrainBounds=terr->getBoundingRect();
    sf::Vector2f direction=tank->getDirection();
	sf::Vector2f position = tank->getPosition();
	sf::FloatRect playerBounds = tank->getBoundingRect();
	if(direction.x>0)
        position.x=terrainBounds.left-playerBounds.width/2;
	else if(direction.x<0)
        position.x=terrainBounds.left+terrainBounds.width+playerBounds.width/2;
	else if(direction.y<0)
        position.y=terrainBounds.top+terrainBounds.height+playerBounds.height/2;
    else if(direction.y>0)
        position.y=terrainBounds.top-playerBounds.height/2;
	tank->setPosition(position);
}
void World::adaptPlayerPosition(Tank &tank)
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	const float borderDistance = 40.f;

	sf::Vector2f position = tank.getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	if(position.x==viewBounds.left + borderDistance||position.x==viewBounds.left + viewBounds.width - borderDistance
    ||position.y== viewBounds.top + borderDistance||position.y==viewBounds.top + viewBounds.height - borderDistance)
        tank.setTraveledDistance(std::numeric_limits<float>::max());
	tank.setPosition(position);
}
void World::adaptPlayerPosition()
{
    for(Tank *ptr:mActiveEnemies)
        adaptPlayerPosition(*ptr);
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
void World::destroyAllEnemis()
{
    while (!mActiveEnemies.empty())
	{
		mActiveEnemies.back()->destroy();

		// Enemy is spawned, remove from the list to spawn
		mActiveEnemies.pop_back();
			updateActiveEnemys();
	}

}
void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mEnemySpawnPoints.empty()&&mActiveEnemies.size()<3)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();

		std::unique_ptr<Tank> enemy(new Tank(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		mActiveEnemies.push_back(enemy.get());
		mSceneLayers[Ground]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}

	//CollectEnemis();
}
void World::spawnTerrains()
{
    if(!mTerrainSpawnPoints.empty())
        for(unsigned i=0;i<mTerrainSpawnPoints.size();i++)
        {
            //std::cout<<"lvl = "<<mTerrainSpawnPoints[i].lvl<<" mCurrLvl = "<<mCurrLvl<<std::endl;
            if(mTerrainSpawnPoints[i].lvl==mCurrLvl)
            {
               // std::cout<<"dodano"<<std::endl;
                SpawnPointTerrain spawn = mTerrainSpawnPoints[i];

                std::unique_ptr<Terrain> terra(new Terrain(spawn.type, mTextures,spawn.lvl));
                terra->setPosition(spawn.x, spawn.y);
                mActiveTerrain.push_back(terra.get());

                mSceneLayers[Ground]->attachChild(std::move(terra));
            }

        }
}

/*bool collision(const SceneNode& lhs, const SceneNode& rhs)
{
	return lhs.getBoundingRect().intersects(rhs.getBoundingRect());
}*/

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
	mCommandQueue.push(missileGuider);
}
void World::CollectEnemis()
{
    Command enemyCollector;
	enemyCollector.category = Category::EnemyTank;
	enemyCollector.action = derivedAction<Tank>([this] (Tank& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
        {
            mActiveEnemies.push_back(&enemy);
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(enemyCollector);

}
void World::CollectTerrain()
{
    Command terrainCollector;
	terrainCollector.category = Category::Terrain;
	terrainCollector.action = derivedAction<Terrain>([this] (Terrain& terrain, sf::Time)
	{
		if (!terrain.isDestroyed())
        {
            mActiveTerrain.push_back(&terrain);
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(terrainCollector);

}
void World::CollectProjectile()
{
    //mActiveProjectile.clear();
    Command ProjectileCollector;
	ProjectileCollector.category = Category::Projectile;
	ProjectileCollector.action = derivedAction<Projectile>([this] (Projectile& projectile, sf::Time)
	{
		if (!projectile.isDestroyed())
        {
            mActiveProjectile.push_back(&projectile);
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(ProjectileCollector);
}
void World::CollectPickups()
{
    //mActiveProjectile.clear();
    Command PickupCollector;
	PickupCollector.category = Category::Pickup;
	PickupCollector.action = derivedAction<Pickup>([this] (Pickup& pickup, sf::Time)
	{
		if (!pickup.isDestroyed())
        {
            mActivePickups.push_back(&pickup);
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(PickupCollector);
}

void World::destroyTerrain()
{
     while (!mActiveTerrain.empty())
	{
		mActiveTerrain.back()->destroy();

		// Enemy is spawned, remove from the list to spawn
		mActiveTerrain.pop_back();
	}

	spawnTerrains();

}
void World::destroyProjectile()
{

	 //mActiveProjectile.clear();
    Command ProjectileDestroyer;
	ProjectileDestroyer.category = Category::Projectile;
	ProjectileDestroyer.action = derivedAction<Projectile>([this] (Projectile& projectile, sf::Time)
	{
		if (!projectile.isDestroyed())
        {
            projectile.destroy();
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(ProjectileDestroyer);

}
void World::destroyPickups()
{

//mActiveProjectile.clear();
    Command PickupDestroyer;
	PickupDestroyer.category = Category::Pickup;
	PickupDestroyer.action = derivedAction<Pickup>([this] (Pickup& pickup, sf::Time)
	{
		if (!pickup.isDestroyed())
        {
            pickup.destroy();
           // std::cout<<"dodano "<<mActiveEnemies.size()<<std::endl;
        }

	});
	mCommandQueue.push(PickupDestroyer);

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
void World::addTerrains(const char *file_name)
{

    std::ifstream file;
    file.open (file_name, std::ios::binary);
    if(!file.is_open())
    {
  // error! maybe the file doesn't exist.

        std::cerr<< "blad";
        return;
    }
    while(true)
    {
        Terrain::Type type;
        int x,y,lvl=1;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            file.read(reinterpret_cast<char*>(&x), sizeof(x));
            file.read(reinterpret_cast<char*>(&y), sizeof(y));
            file.read(reinterpret_cast<char*>(&lvl),sizeof(lvl));


        if (!file)
            break;
        addTerrain(type, x, y,lvl);

    // Do something with xyz
    }


    //std::cout<<"ilosc elementow: "<<mTerrainSpawnPoints.size()<<std::endl;
    std::sort(mTerrainSpawnPoints.begin(), mTerrainSpawnPoints.end(), [] (SpawnPointTerrain lhs, SpawnPointTerrain rhs)
              {
                  return lhs.y < rhs.y;
              });
}
bool World::updateActiveTerrain(Terrain *terr)
{

        for (auto it = mActiveTerrain.begin(); it != mActiveTerrain.end(); ) {
        if (terr->getBoundingRect().intersects((*it)->getBoundingRect())) {
               eraseDuplicates(*it);
                terr->destroy();
              (*it)->destroy();
            it = mActiveTerrain.erase(it);

            return false;

                } else {
                    ++it;
                }
    }
    return true;
}
void World::addTerrain(sf::Vector2i position,Terrain::Type type)
{

        position=adjustTerrainPosition(position);
        std::unique_ptr<Terrain> terra(new Terrain(type, mTextures,mCurrLvl));
        terra->setPosition(position.x, position.y);

//        mActiveTerrain.push_back(terra);
        if(updateActiveTerrain(terra.get()))
        {
            addTerrain(type,position.x, position.y,mCurrLvl);
            mActiveTerrain.push_back(terra.get());
           // std::cout<<"spawn = "<<mTerrainSpawnPoints.size()<<" active = "<<mActiveTerrain.size()<<std::endl;
            mSceneLayers[Ground]->attachChild(std::move(terra));
        }
        else
        {
            delete terra.get();

        }
}
void World::eraseDuplicates(Terrain* terrain)
{
    sf::Vector2f position = terrain->getPosition();
    SpawnPointTerrain spawn(terrain->getType(),(int)position.x,(int)position.y,terrain->getLvl());
    for (auto it = mTerrainSpawnPoints.begin(); it != mTerrainSpawnPoints.end(); ) {
        if (*it== spawn) {
            it = mTerrainSpawnPoints.erase(it);

                } else {
                    ++it;
                }
}
}
void World::collisionEnemyXTerrain()
{

        for(auto itT = mActiveTerrain.begin(); itT != mActiveTerrain.end(); )
           {
               auto temp = itT;
               if ((*itT)->getBoundingRect().intersects(mPlayerTank->getBoundingRect()))
                    {
                        adaptPlayerPosition(mPlayerTank,*itT);
                    }
               for (auto itE = mActiveEnemies.begin(); itE != mActiveEnemies.end();++itE )
               {
                   if ((*itE)->getBoundingRect().intersects((*itT)->getBoundingRect()))
                    {
                        (*itE)->setTraveledDistance(std::numeric_limits<float>::max());
                        adaptPlayerPosition(*itE,*itT);
                    }
                    if(!(*itE)->isDestroyed()&&(*itE)->getBoundingRect().intersects(mPlayerTank->getBoundingRect()))
                    {
                        mPlayerTank->damage((*itE)->getHitpoints());
                        (*itE)->destroy();
                    }
               }

                for (auto itP = mActiveProjectile.begin(); itP != mActiveProjectile.end(); )
                  {
                        if ((*itP)->getBoundingRect().intersects((*itT)->getBoundingRect()))
                        {
                           if((*itT)->getCategory()&Category::Brick || (*itT)->getCategory()&Category::Eagle)
                            {
                                if((*itT)->getCategory()&Category::Eagle)
                                {
                                     mEagleAlive=false;

                                }

                                (*itT)->destroy();
                                itT = mActiveTerrain.erase(itT);
                                (*itP)->destroy();
                                itP = mActiveProjectile.erase(itP);
                            }
                            else if((*itT)->getCategory()&Category::Steel)
                            {
                              (*itP)->destroy();
                                itP = mActiveProjectile.erase(itP);
                            }


                        }
                       /* else if((*itP)->getBoundingRect().intersects(mPlayerTank->getBoundingRect()))
                        {
                            std::cout<<"obrazenia = "<<(*itP)->getDamage()<<std::endl;
                            mPlayerTank->damage((*itP)->getDamage());
                            (*itP)->destroy();
                            itP = mActiveProjectile.erase(itP);

                        }*/
                        else
                        {
                            ++itP;

                        }
                  }

                 if(temp==itT)
                    {
                        ++itT;
                    }
            }
            updateActiveEnemys();
}
void World::collisionTankXProjectile()
{
        for (auto itP = mActiveProjectile.begin(); itP != mActiveProjectile.end();++itP )
        {
            if((*itP)->getCategory()&Category::EnemyProjectile&&(*itP)->getBoundingRect().intersects(mPlayerTank->getBoundingRect())&&!(*itP)->isDestroyed())
            {
                mPlayerTank->damage((*itP)->getDamage());
                (*itP)->destroy();
            }
           for(auto itE = mActiveEnemies.begin(); itE != mActiveEnemies.end();++itE)
            {
                 if((*itP)->getCategory()&Category::AlliedProjectile&&(*itE)->getBoundingRect().intersects((*itP)->getBoundingRect())&&!(*itP)->isDestroyed()&&!(*itE)->isDestroyed())
                 {

                    (*itE)->damage((*itP)->getDamage());
                    (*itP)->destroy();
                 }

            }



        }
        updateActiveEnemys();
}
void World::collisionTankXPickups()
{
    if(!mActivePickups.empty())
        for (auto it = mActivePickups.begin(); it != mActivePickups.end();++it )
        {
            if(!(*it)->isDestroyed()&&(*it)->getBoundingRect().intersects(mPlayerTank->getBoundingRect()))
            {
                auto& player = static_cast<Tank&>(*mPlayerTank);
                (*it)->apply(player);
                (*it)->destroy();
                mPlayerTank->playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
            }

        }
}
void World::clearWorld()
{
   destroyTerrain();
   destroyProjectile();
   destroyPickups();
}
bool World::isEagleAlive() const
{
    return mEagleAlive;
}
void World::collision()
{
    CollectProjectile();
    CollectPickups();
	collisionEnemyXTerrain();
	collisionTankXProjectile();
	collisionTankXPickups();
    mActiveProjectile.clear();
    mActivePickups.clear();
}
sf::Vector2i World::adjustTerrainPosition(sf::Vector2i position)
{

    position.x=((position.x+18)/32)*32;
    position.y=((position.y+18)/32)*32;
    return position;
}
void World::saveTerrainToFile(const char *file_name)
{
    std::ofstream file;
    file.open (file_name, std::ios::out |  std::ios::binary | std::ios::trunc);
    if(!file.is_open())
        std::cerr << "nie udalo otworzyc pliku sie."<< std::endl;
    else
    {
         while (!mTerrainSpawnPoints.empty())
        {

            SpawnPointTerrain spawn = mTerrainSpawnPoints.back();

            file.write(reinterpret_cast<char*>(&spawn.type), sizeof(spawn.type));
            file.write(reinterpret_cast<char*>(&spawn.x), sizeof(spawn.x));
            file.write(reinterpret_cast<char*>(&spawn.y), sizeof(spawn.y));
            file.write(reinterpret_cast<char*>(&spawn.lvl), sizeof(spawn.lvl));
          //  std::cout<<"type = "<<spawn.type<<" x = "<<spawn.x<<" y = " <<spawn.y<<" lvl = "<<spawn.lvl<<std::endl;

            mTerrainSpawnPoints.pop_back();
        }
    }

    file.close();


}
int World::getCurrLvl() const
{
    return mCurrLvl;
}
