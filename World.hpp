#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED


#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"
#include "Tank.hpp"

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
		explicit							World(sf::RenderWindow& window, FontHolder &fonts);
		void								update(sf::Time dt);
		void								draw();

		CommandQueue&						getCommandQueue();

		bool 								hasAlivePlayer() const;
		bool 								hasPlayerReachedEnd() const;



	private:

		void								loadTextures();
        void								handleCollisions();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();

		void								buildScene();
		void								addEnemies();
		void								addEnemy(Tank::Type type, float relX, float relY);
		void								spawnEnemies();
		void								destroyEntitiesOutsideView();
		void								guideMissiles();
		sf::FloatRect						getViewBounds() const;
		sf::FloatRect						getBattlefieldBounds() const;

	private:
		enum Layer
		{
			Background,
			Ground,
			LayerCount
		};

        struct SpawnPoint
		{
			SpawnPoint(Tank::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
			{
			}

			Tank::Type type;
			float x;
			float y;
		};

	private:
		sf::RenderWindow&					mWindow;
		sf::View							mWorldView;
		TextureHolder						mTextures;
		FontHolder&							mFonts;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		Tank*							    mPlayerTank;

		std::vector<SpawnPoint>				mEnemySpawnPoints;
		std::vector<Tank*>				    mActiveEnemies;
};


#endif // WORLD_HPP_INCLUDED