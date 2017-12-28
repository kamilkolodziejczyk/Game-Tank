#include "DataTables.hpp"
#include "Tank.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Terrain.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<TankData> initializeTankData()
{
	std::vector<TankData> data(Tank::TypeCount);

	data[Tank::Tank1].hitpoints = 100;
	data[Tank::Tank1].speed = 200.f;
	data[Tank::Tank1].fireInterval = sf::seconds(1);
	data[Tank::Tank1].texture = Textures::Tank1;
	data[Tank::Tank1].directions.push_back(Direction(0, 100));

	data[Tank::Tank2].hitpoints = 20;
	data[Tank::Tank2].speed = 80.f;
	data[Tank::Tank2].texture = Textures::Tank2;
    data[Tank::Tank2].directions.push_back(Direction(0, 200));
    data[Tank::Tank2].directions.push_back(Direction(90, 200));
    data[Tank::Tank2].directions.push_back(Direction(180, 200));
    data[Tank::Tank2].directions.push_back(Direction(270, 200));

	data[Tank::Tank2].fireInterval = sf::seconds(1.f);
/*
	data[Aircraft::Avenger].hitpoints = 40;
	data[Aircraft::Avenger].speed = 50.f;
	data[Aircraft::Avenger].texture = Textures::Avenger;
	data[Aircraft::Avenger].directions.push_back(Direction(+45.f,  50.f));
	data[Aircraft::Avenger].directions.push_back(Direction(  0.f,  50.f));
	data[Aircraft::Avenger].directions.push_back(Direction(-45.f, 100.f));
	data[Aircraft::Avenger].directions.push_back(Direction(  0.f,  50.f));
	data[Aircraft::Avenger].directions.push_back(Direction(+45.f,  50.f));
	data[Aircraft::Avenger].fireInterval = sf::seconds(2);*/

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
	data[Projectile::AlliedBullet].texture = Textures::Bullet;

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::Bullet;

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::Missile;

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::HealthRefill;
	data[Pickup::HealthRefill].action = [] (Tank& a) { a.repair(25); };

	data[Pickup::MissileRefill].texture = Textures::MissileRefill;
	data[Pickup::MissileRefill].action = std::bind(&Tank::collectMissiles, _1, 3);

	data[Pickup::FireSpread].texture = Textures::FireSpread;
	data[Pickup::FireSpread].action = std::bind(&Tank::increaseSpread, _1);

	data[Pickup::FireRate].texture = Textures::FireRate;
	data[Pickup::FireRate].action = std::bind(&Tank::increaseFireRate, _1);

	return data;
}
std::vector<TerrainData> initializeTerrainData()
{
    std::vector<TerrainData> data(Terrain::TypeCount);
    data[Terrain::Brick].texture = Textures::Brick;
    data[Terrain::Steel].texture = Textures::Steel;

    return data;

}
