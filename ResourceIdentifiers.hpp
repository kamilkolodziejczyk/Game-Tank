#ifndef RESOURCEIDENTIFIERS_HPP_INCLUDED
#define RESOURCEIDENTIFIERS_HPP_INCLUDED

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class SoundBuffer;
}

namespace Textures
{
	enum ID
	{
		Tank1,
		Tank2,
		Bullet,
		Missile,
		Desert,
		Brick,
		Steel,
		HealthRefill,
		MissileRefill,
		FireRate,
		FireSpread,
		TitleScreen,
		Eagle,
		Spawn

	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}
namespace Music
{
	enum ID
	{
		MenuTheme,
		MissionTheme,
		NextLevelStateTheme,
	};
}
namespace SoundEffect
{
	enum ID
	{
		AlliedGunfire,
		EnemyGunfire,
		Explosion1,
		Explosion2,
		LaunchMissile,
		CollectPickup,
		Button,
		Ride,
	};
}
// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>			FontHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect::ID>	SoundBufferHolder;



#endif // RESOURCEIDENTIFIERS_HPP_INCLUDED
