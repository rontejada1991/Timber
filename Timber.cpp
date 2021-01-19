#include "Bee.h"
#include "Cloud.h"
#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Make code easier to type with "using namespace"
using namespace sf;
// Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch? 
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main() {
	// Create a video mode object
	VideoMode vm(1920, 1080);

	// Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	// Create a sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Make a tree sprite array for the background trees
	// Make the trees smaller than the interactable trees but different in width
	// Set there position around the interactable tree
	Sprite spriteTreesBackground[7];
	srand(time(NULL));
	for (int i = 0; i < sizeof(spriteTreesBackground)/sizeof(Sprite); i++) {
		spriteTreesBackground[i].setTexture(textureTree);
		//Set scale from 0.4f to 0.8f of original scale, keep height the same
		spriteTreesBackground[i].setScale(
			(float(rand())/float((RAND_MAX)) * 0.4f) + 0.4f, 1.0f);
		// Set x position from 100 to 1580 (1920 - max tree background width - 100)
		// Set y position from -100 (farther than base tree) and -225 (max before it looks like its floating)
		spriteTreesBackground[i].setPosition(
			(rand() % 1480) + 100, 
			((rand() % 125) + 100) * -1
		);
	}

	// Prepare the bee
	Bee bee;

	// Prepare the clouds
	// Loads and sets the texture, sets active to false, and speed to 0.0f
	Cloud clouds[3];

	// Sets the height of each cloud differently
	for (int i = 0; i < sizeof(clouds)/sizeof(clouds[0]); i++) {
		clouds[i].setPosition(0, (i * 150));
	}

	// Variables to control time itself
	Clock clock;
	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Track whether the game is running
	bool paused = true;
	// Draw some text
	int score = 0;

	Text messageText;
	Text scoreText;

	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	// Make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

	scoreText.setPosition(20, 20);

	// Prepare 5 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	// Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		// Set the sprite's origin to dead center
		// We can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}
	
	// Load player textures
	Texture texturePlayerLeft;
	texturePlayerLeft.loadFromFile("graphics/playerLeft.png");
	Texture texturePlayerRight;
	texturePlayerRight.loadFromFile("graphics/playerRight.png");

	// Prepare the player
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayerLeft);
	spritePlayer.setPosition(580, 720);

	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);

	// hide the gravestone
	spriteRIP.setPosition(2000, 860);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);

	// hide the axe
	spriteAxe.setPosition(2000, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;
	
	// Control the player input
	bool acceptInput = false;

	// Prepare the sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sounds/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sounds/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sounds/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);
	
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for key presses again
				acceptInput = true;
			}
		}

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and the score
			score = 0;
			timeRemaining = 6;

			// Make all the branches disappear
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the axe into position
			spriteAxe.setPosition(AXE_POSITION_LEFT, 830);

			// Move the player into position
			spritePlayer.setPosition(580, 720);
			spritePlayer.setTexture(texturePlayerLeft);

			acceptInput = true;
		}

		// Wrap the player controls to
		// Make sure we are accepting input
		if (acceptInput) {
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Make sure the player is on the right
				playerSide = side::RIGHT;

				// Increase player score
				score++;

				// Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				// Set axe position to the right
				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				// Set player position to the right
				spritePlayer.setPosition(1200, 720);

				// Set player sprite to the right
				spritePlayer.setTexture(texturePlayerRight);

				// update the branches
				updateBranches(score);

				// set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

			// Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				// Make sure the player is on the left
				playerSide = side::LEFT;

				// Increase player score
				score++;

				// Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				// Set axe position to the right
				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);

				// Set player position to the left
				spritePlayer.setPosition(580, 720);

				// Set player sprite to the left
				spritePlayer.setTexture(texturePlayerLeft);

				// update the branches
				updateBranches(score);

				// set the log flying
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}
		}

		/*
		****************************************
		Update the scene
		****************************************
		*/
		if (!paused) {
			// Measure time
			Time dt = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!!");

				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the out of time sound
				outOfTime.play();
			}

			// Setup the bee
			if (!bee.isActive()) {
				// How fast is the bee
				srand((int)time(0) * 10);
				bee.setSpeed((rand() % 200) + 200);

				// How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				bee.setPosition(2000, height);
				bee.activate();
			} else {
				// Move the bee
				bee.setPosition(
					bee.getPositionX() -
					(bee.getSpeed() * dt.asSeconds()),
					bee.getPositionY());

				// Has the bee reached the right hand edge of the screen?
				if (bee.getPositionX() < -100) {
					// Set it up ready to be a whole new bee next frame
					bee.deactivate();
				}
			}

			// Manage the clouds
			for (int i = 0; i <= sizeof(clouds) / sizeof(clouds[0]); i++) {
				if (!clouds[i].isActive()) {
					// How fast is the cloud
					srand((int)time(0) * (i * 10) + 10);
					clouds[i].setSpeed(rand() % 200);

					// How high is the cloud
					float height = rand() % ((i * 150) + 150);
					clouds[i].setPosition(-200, height);
					clouds[i].activate();
				}
				else {
					clouds[i].setPosition(
						clouds[i].getPositionX() + (clouds[i].getSpeed() * dt.asSeconds()),
						clouds[i].getPositionY());

					// Has the cloud reached the right hand edge of the screen?
					if (clouds[i].getPositionX() > 1920) {
						clouds[i].deactivate();
					}
				}
			}

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {

				float height = i * 150;

				if (branchPositions[i] == side::LEFT) {
					// Move the sprite to the left side
					branches[i].setPosition(610, height);
					// Flip the sprite round the other way
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(180);
				} else if (branchPositions[i] == side::RIGHT) {
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(0);
				} else {
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// Handle a flying log				
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				// Has the insect reached the right hand edge of the screen?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000) {
					// Set it up ready to be a whole new cloud next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// Has the player been squished by a branch?
			if (branchPositions[5] == playerSide) {
				// death
				paused = true;
				acceptInput = false;

				// Draw the gravestone depending on which side player was squished
				if (playerSide == side::LEFT) {
					spriteRIP.setPosition(580, 760);
				}
				else {
					spriteRIP.setPosition(1200, 760);
				}

				// hide the player
				spritePlayer.setPosition(2000, 720);

				// Change the text of the message
				messageText.setString("SQUISHED!!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				// Play the death sound
				death.play();
			}
		} // End if(!paused)

		/*
		****************************************
		Draw the scene
		****************************************
		*/

		// Clear everything from the last frame
		window.clear();

		// Draw our game objects
		window.draw(spriteBackground);

		for (int i = 0; i < sizeof(spriteTreesBackground) / sizeof(Sprite); i++) {
			window.draw(spriteTreesBackground[i]);
		}

		for (int i = 0; i < sizeof(clouds) / sizeof(clouds[0]); i++) {
			window.draw(clouds[i].getSprite());
		}

		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		window.draw(spriteTree);
		window.draw(spritePlayer);
		window.draw(spriteAxe);
		window.draw(spriteLog);
		window.draw(spriteRIP);
		window.draw(bee.getSprite());
		window.draw(scoreText);
		window.draw(timeBar);

		if (paused) { 
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();
	}
	return 0;
}

// Function definition
void updateBranches(int seed) {
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;

	case 1:
		branchPositions[0] = side::RIGHT;
		break;

	default:
		branchPositions[0] = side::NONE;
		break;
	}
}



