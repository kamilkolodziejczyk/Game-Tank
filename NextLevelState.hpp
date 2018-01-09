#ifndef NEXTLEVELSTATE_HPP_INCLUDED
#define NEXTLEVELSTATE_HPP_INCLUDED

#include "State.hpp"
#include "Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class NextLevelState : public State
{
	public:
							NextLevelState(StateStack& stack, Context context);
							~NextLevelState();

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);


	private:
		sf::Text			mNextLevelText;
		sf::Time			mElapsedTime;
		MusicPlayer&        mMusic;
};

#endif // NEXTLEVELSTATE_HPP_INCLUDED
