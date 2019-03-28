#ifndef LOGINSTATE_H
#define LOGINSTATE_H

enum Layer
{
	Any = 1,
    Login = 2,
    CreateAccount = 4,
    Connecting = 8,
    CharacterSelect = 16,
    CreateCharacter = 32,
    EnteringWorld = 64,
    InGame = 128,
    Disconnected = 256
};

inline Layer operator|(Layer a, Layer b)
{
	return static_cast<Layer>(static_cast<int>(a) | static_cast<int>(b));
}

inline Layer operator&(Layer a, Layer b)
{
	return static_cast<Layer>(static_cast<int>(a) & static_cast<int>(b));
}

#endif