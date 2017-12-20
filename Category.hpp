#ifndef CATEGORY_HPP_INCLUDED
#define CATEGORY_HPP_INCLUDED

// Entity/scene node category, used to dispatch commands
namespace Category
{
	enum Type
	{
		None				    = 0,
		SceneGroundLayer		= 1 << 0,
		PlayerTank		        = 1 << 1,
		AlliedTank		        = 1 << 2,
		EnemyTank	            = 1 << 3,
		Pickup				    = 1 << 4,
		AlliedProjectile	    = 1 << 5,
		EnemyProjectile		    = 1 << 6,

		Tank= PlayerTank | AlliedTank | EnemyTank,
		Projectile = AlliedProjectile | EnemyProjectile,
	};
}

#endif // CATEGORY_HPP_INCLUDED
