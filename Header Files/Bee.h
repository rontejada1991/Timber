#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bee {

private:
	Texture beeTexture;
	Sprite beeSprite;
	float beeSpeed;
	bool beeActive;

public:
	Bee();
	void activate();
	void deactivate();
	void setSpeed(float speed);
	void setPosition(float x, float y);
	Sprite getSprite();
	bool isActive();
	float getSpeed();
	float getPositionX();
	float getPositionY();
};

