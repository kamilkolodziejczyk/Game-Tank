#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED


#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Player.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>


class Application
{
	public:
								Application();
		void					run();


	private:
		void					processInput();
		void					update(sf::Time dt);
		void					render();

		void					updateStatistics(sf::Time dt);
		void					registerStates();


	private:
		static const sf::Time	TimePerFrame;

		sf::RenderWindow		mWindow;
		TextureHolder			mTextures;
	  	FontHolder				mFonts;
		Player					mPlayer;


        MusicPlayer             mMusic;
		StateStack				mStateStack;
		SoundPlayer             mSounds;

		sf::Text				mStatisticsText;
		sf::Time				mStatisticsUpdateTime;
		std::size_t				mStatisticsNumFrames;
};

#endif // APPLICATION_HPP_INCLUDED
