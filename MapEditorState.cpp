#include "MapEditorState.hpp"


MapEditorState::MapEditorState(StateStack& stack, Context context)
: State(stack, context)
, mWorld(*context.window, *context.fonts, *context.sounds)
,mTerrainType(Terrain::Steel)
{

	context.music->play(Music::MissionTheme);
}

void MapEditorState::draw()
{
	mWorld.draw();
}

bool MapEditorState::update(sf::Time dt)
{
	mWorld.edit(dt);

	return true;
}

bool MapEditorState::handleEvent(const sf::Event& event)
{
		if (event.type == sf::Event::MouseButtonPressed)
        {
            if(event.mouseButton.button == sf::Mouse::Left)
                    mWorld.addTerrain(sf::Mouse::getPosition(),mTerrainType);
            else if(event.mouseButton.button == sf::Mouse::Right)
                    changeTerrainType();

        }

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        mWorld.saveTerrainToFile("map.bin");
        requestStateClear();
        requestStackPush(States::Menu);
    }
    if (event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::Right)
        {
           mWorld.nextLvl();
        }

    }
    if (event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::Left)
        {
            mWorld.prevLvl();
        }

    }


	return true;
}
void MapEditorState::changeTerrainType()
{
    mTerrainType=static_cast<Terrain::Type>((mTerrainType+1)%(int)Terrain::TypeCount);
}

