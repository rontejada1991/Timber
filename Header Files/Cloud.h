#pragma once
#include <SFML\Graphics.hpp>

using namespace sf;

class Cloud {

private:
	Texture cloudTexture;
	Sprite cloudSprite;
	float cloudSpeed;
	bool cloudActive;

public:
	Cloud();
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
