#ifndef AUDIO_HPP_INCLUDED
#define AUDIO_HPP_INCLUDED

#include <SFML/Audio.hpp>

using namespace sf;

class Audio
{
public:
    Audio(bool load=false, bool menu=false);
    ~Audio(void);

    enum SoundType{T_STRZAL, T_MENU, T_CLICK, T_COUNT};

    bool loadMusic(bool menu);
    bool loadSounds();

    void playMusic();
    void stopMusic();

    void playSound(SoundType sound);
    void setVolume(bool mute);
private:
    Music music;

    Sound sound;

    SoundBuffer buffer[T_COUNT];
};

#endif // AUDIO_HPP_INCLUDED
