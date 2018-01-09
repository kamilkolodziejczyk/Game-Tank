#include "NextLevelState.hpp"
#include "Utility.hpp"
#include "Player.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <string>


NextLevelState::NextLevelState(StateStack& stack, Context context)
: State(stack, context)
, mMusic(*context.music)
, mNextLevelText()
, mElapsedTime(sf::Time::Zero)
{
	sf::Font& font = context.fonts->get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	mNextLevelText.setFont(font);
	std::string currLvl = "Level "+ toString(context.player->getCurrLvl());
		mNextLevelText.setString(currLvl);


	mNextLevelText.setCharacterSize(70);
	centerOrigin(mNextLevelText);
	mNextLevelText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
	context.music->play(Music::NextLevelStateTheme);
}

void NextLevelState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color::Black);
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mNextLevelText);
}

bool NextLevelState::update(sf::Time dt)
{
	// Show state for 3 seconds, after return to menu
	mElapsedTime += dt;
	if (mElapsedTime > sf::seconds(3))
	{
		requestStackPop();
		//requestStackPush(States::Game);
	}
	return false;
}

bool NextLevelState::handleEvent(const sf::Event&)
{
	return false;
}
NextLevelState::~NextLevelState()
{
	mMusic.play(Music::MissionTheme);
}
