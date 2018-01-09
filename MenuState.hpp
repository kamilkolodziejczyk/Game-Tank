#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

#include "State.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class MenuState : public State
{
	public:
								MenuState(StateStack& stack, Context context);

		virtual void			draw();
		virtual bool			update(sf::Time dt);
		virtual bool			handleEvent(const sf::Event& event);

		void					updateOptionText();


	private:
		enum OptionNames
		{
			Play,
			MapEditor,
			Exit,
		};


	private:
		sf::Sprite				mBackgroundSprite;

		std::vector<sf::Text>	mOptions;
		std::size_t				mOptionIndex;
};


#endif // MENU_HPP_INCLUDED
