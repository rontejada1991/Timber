#include "Bee.h"

Bee::Bee() {
	beeTexture.loadFromFile("graphics/bee.png");
	beeSprite.setTexture(beeTexture);
	beeSprite.setPosition(0, 800);
	beeSpeed = 0.0f;
	beeActive = false;
}

void Bee::activate() {
	beeActive = true;
}

void Bee::deactivate() {
	beeActive = false;
}

void Bee::setSpeed(float speed) {
	beeSpeed = speed;
}

void Bee::setPosition(float x, float y) {
	beeSprite.setPosition(x, y);
}

Sprite Bee::getSprite() {
	return beeSprite;
}

bool Bee::isActive() {
	return beeActive;
}

float Bee::getSpeed() {
	return beeSpeed;
}

float Bee::getPositionX() {
	return beeSprite.getPosition().x;
}

float Bee::getPositionY() {
	return beeSprite.getPosition().y;
}