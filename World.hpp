#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED


#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"
#include "Tank.hpp"
#include "Terrain.hpp"
#include "SoundPlayer.hpp"
#include "Pickup.hpp"
#include <iostream>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <queue>
#include <array>


// Forward declaration
namespace sf
{
	class RenderWindow;
}

class World : private sf::NonCopyable
{
	public:
		explicit							World(sf::RenderWindow& window, FontHolder &fonts, SoundPlayer& sounds);
		void								update(sf::Time dt);
		void                                edit(sf::Time dt);
		void								draw();

		CommandQueue&						getCommandQueue();

		bool 								hasAlivePlayer() const;
		bool 								hasPlayerReachedEnd() const;
		bool                                isNextLvl() ;
        bool                                isEagleAlive() const;

		void                                addTerrain(sf::Vector2i position,Terrain::Type type);
		void                                saveTerrainToFile(const char *file_name);
		void                                nextLvl();
		void                                prevLvl();
		int                                 getCurrLvl() const;




	private:

		void								loadTextures();
        void								handleCollisions();
		void                                destroyTerrain();
        void                                destroyProjectile();
        void                                destroyPickups();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();
		void                                adaptPlayerPosition(Tank* tank,Terrain *terr);
		void                                adaptPlayerPosition(Tank &tank);
        void                                updateSounds();
        void                                collision();
        void                                updateActiveEnemys();
        bool                                updateActiveTerrain(Terrain *terr);
		void								buildScene();
		void								addEnemies();
		void                                addTerrains(const char *file_name);
		void								addEnemy(Tank::Type type, int n);
		void                                addTerrain(Terrain::Type type, float relX, float relY,int lvl);
		void								spawnEnemies();
		void                                spawnTerrains();
		void								destroyEntitiesOutsideView();
		void								guideMissiles();
		void                                CollectEnemis();
		void                                CollectTerrain();
		void                                CollectProjectile();
		void                                CollectPickups();
		void                                destroyAllEnemis();
		void                                clearWorld();
		void                                collisionEnemyXTerrain();
        void                                collisionTankXProjectile();
        void                                collisionTankXPickups();
		void                                eraseDuplicates(Terrain* terrain);
		sf::FloatRect						getViewBounds() const;
		sf::FloatRect						getBattlefieldBounds() const;
		sf::Vector2i                        adjustTerrainPosition(sf::Vector2i position);

	private:
		enum Layer
		{
			Background,
			Ground,
			LayerCount
		};

        struct SpawnPoint
		{
			SpawnPoint(Tank::Type type, int n)
			: type(type)
			, n(n)
			{
			   y=50;
			   x=n*200 ;
			}

			Tank::Type type;
			int n;
			int x;
			int y;
		};
		class SpawnPointTerrain
		{
		    public:
			SpawnPointTerrain(Terrain::Type type, int x, int y, int lvl)
			: type(type)
			, x(x)
			, y(y)
			,lvl(lvl)
			{
			}

			Terrain::Type type;
			int x;
			int y;
			int lvl;
			inline bool operator==( const SpawnPointTerrain& spawn){
			    return this->x==spawn.x&&this->y==spawn.y&&lvl==spawn.lvl; };
		};

	private:
		sf::RenderWindow&					mWindow;
		sf::View							mWorldView;
		TextureHolder						mTextures;
		FontHolder&							mFonts;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;
		SoundPlayer&                        mSounds;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		sf::Vector2f                        mTerrainSpawnPosition;
		Tank*							    mPlayerTank;
		Terrain* 						    mTerrain;
		bool                                enemySpawned;
		bool                                mEagleAlive;
		int                                 mCurrLvl;
		int                                 mMaxLvl;

		std::vector<SpawnPoint>				mEnemySpawnPoints;
		std::vector<SpawnPointTerrain>      mTerrainSpawnPoints;
		std::vector<Tank*>				    mActiveEnemies;
		std::vector<Terrain*>               mActiveTerrain;
		std::vector<Projectile*>            mActiveProjectile;
		std::vector<Pickup*>                mActivePickups;
};


#endif // WORLD_HPP_INCLUDED
