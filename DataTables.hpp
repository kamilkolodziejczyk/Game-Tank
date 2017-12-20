#ifndef DATATABLES_HPP_INCLUDED
#define DATATABLES_HPP_INCLUDED


#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <functional>


class Tank;

struct Direction
{
	Direction(float angle, float distance)
	: angle(angle)
	, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct TankData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
	sf::Time						fireInterval;
	std::vector<Direction>			directions;
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


std::vector<TankData>	    initializeTankData();
std::vector<ProjectileData>	initializeProjectileData();
std::vector<PickupData>		initializePickupData();

#endif // DATATABLES_HPP_INCLUDED
