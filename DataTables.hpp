#ifndef DATATABLES_HPP_INCLUDED
#define DATATABLES_HPP_INCLUDED


#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <functional>


class Tank;

enum direction{
        Up,
        Down,
        Right,
        Left,
        };


struct TankData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
	sf::Time						fireInterval;
	float                           distance;
};

struct ProjectileData
{
	int								damage;
	float							speed;
	Textures::ID					texture;
};

struct PickupData
{
	std::function<void(Tank&)>	    action;
	Textures::ID					texture;
};
struct TerrainData
{
	Textures::ID					texture;
};

std::vector<TankData>	    initializeTankData();
std::vector<ProjectileData>	initializeProjectileData();
std::vector<PickupData>		initializePickupData();
std::vector<TerrainData>    initializeTerrainData();

#endif // DATATABLES_HPP_INCLUDED
