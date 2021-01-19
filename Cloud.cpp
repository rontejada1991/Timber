#include "Cloud.h"

Cloud::Cloud() {
	cloudTexture.loadFromFile("graphics/cloud.png");
	cloudSprite.setTexture(cloudTexture);
	cloudSprite.setPosition(0, 800);
	cloudSpeed = 0.0f;
	cloudActive = false;
}

void Cloud::activate() {
	cloudActive = true;
}

void Cloud::deactivate() {
	cloudActive = false;
}

void Cloud::setSpeed(float speed) {
	cloudSpeed = speed;
}

void Cloud::setPosition(float x, float y) {
	cloudSprite.setPosition(x, y);
}

Sprite Cloud::getSprite() {
	return cloudSprite;
}

bool Cloud::isActive() {
	return cloudActive;
}

float Cloud::getSpeed() {
	return cloudSpeed;
}

float Cloud::getPositionX() {
	return cloudSprite.getPosition().x;
}

float Cloud::getPositionY() {
	return cloudSprite.getPosition().y;
}