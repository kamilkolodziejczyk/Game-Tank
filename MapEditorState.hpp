#ifndef MAPEDITORSTATE_HPP_INCLUDED
#define MAPEDITORSTATE_HPP_INCLUDED


#include "State.hpp"
#include "World.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class MapEditorState : public State
{
	public:
							MapEditorState(StateStack& stack, Context context);

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);


	private:
	    void                changeTerrainType();
		World				mWorld;
		Terrain::Type       mTerrainType;
};

#endif // MAPEDITORSTATE_HPP_INCLUDED
