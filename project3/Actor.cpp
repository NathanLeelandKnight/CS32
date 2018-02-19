#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Gives every Actor access to StudentWorld object
void Actor::setStudentWorldPtr(StudentWorld& other)
{
	m_studentWorldPtr = &other;
}

int Player::doSomething()
{
	int keyPressed;

	if (checkIfAlive() == false)
	{
		return GWSTATUS_PLAYER_DIED;
	}

	//Check for key pressed
	if (getStudentWorldPtr()->getKey(keyPressed))
	{
		//Keeps player from getting hit by own bullet
		if (getFiredBullet() && keyPressed != KEY_PRESS_SPACE)
		{
			if ((keyPressed == KEY_PRESS_RIGHT && getDirection() == right) ||
				(keyPressed == KEY_PRESS_LEFT  && getDirection() == left)  ||
				(keyPressed == KEY_PRESS_UP    && getDirection() == up)    ||
				(keyPressed == KEY_PRESS_DOWN  && getDirection() == down))
			{
				setFiredBullet(false);
				return GWSTATUS_CONTINUE_GAME;
			}
			setFiredBullet(false);
		}

		//Takes away 1 life and starts level over
		if (keyPressed == KEY_PRESS_ESCAPE)
		{
			getStudentWorldPtr()->decLives();

			return GWSTATUS_PLAYER_DIED;
		}

		//Move
		makeAMove(keyPressed);
	}
	return GWSTATUS_CONTINUE_GAME;
}

void Player::makeAMove(int keyPressed)
{
	Actor     *tempPtr;
	int       x   = getStudentWorldPtr()->getPlayerPtrFromSW()->getX();
	int       y   = getStudentWorldPtr()->getPlayerPtrFromSW()->getY();
	Direction dir = getStudentWorldPtr()->getPlayerPtrFromSW()->getDirection();

	switch (keyPressed)
	{
	//Fire bullet if player has ammo
	case KEY_PRESS_SPACE:
		if (m_ammo == 0)
		{
			break;
		}
		switch (dir)
		{
		case    up: y++;
			break;
		case  down: y--;
			break;
		case right: x++;
			break;
		case  left: x--;
			break;
		}
		//Add bullet to graphObject vector and dec ammo
		tempPtr = new Bullet(x, y, m_subLevel, dir, *getStudentWorldPtr());
		getStudentWorldPtr()->addToGraphVec(tempPtr, m_subLevel);
		getStudentWorldPtr()->playSound(SOUND_PLAYER_FIRE);
		tempPtr->setVisible(true);
		setFiredBullet(true);
 		decAmmo();
		break;
	//Change Direction and check (x, y) coord before moving
	case KEY_PRESS_UP:
		y++;
		dir = Direction::up;
		break;
	case KEY_PRESS_DOWN:
		y--;
		dir = Direction::down;
		break;
	case KEY_PRESS_RIGHT:
		x++;
		dir = Direction::right;
		break;
	case KEY_PRESS_LEFT:
		x--;
		dir = Direction::left;
		break;
	}

	//Check if player can actually move to the space specified
	if (keyPressed != KEY_PRESS_ESCAPE && keyPressed != KEY_PRESS_SPACE)
	{
		tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(x, y);

		getStudentWorldPtr()->getPlayerPtrFromSW()->setDirection(dir);

		if (tempPtr == nullptr)
		{
			getStudentWorldPtr()->getPlayerPtrFromSW()->moveTo(x, y);
		}
		else if (tempPtr->getID() == IID_FAKE_WALL    ||
				 tempPtr->getID() == IID_AMMO		  ||
				 tempPtr->getID() == IID_BULLET		  ||
				 tempPtr->getID() == IID_EXIT		  ||
				 tempPtr->getID() == IID_EXTRA_LIFE   ||
				 tempPtr->getID() == IID_FARPLANE_GUN ||
				 tempPtr->getID() == IID_GATE		  ||
				 tempPtr->getID() == IID_GOLD		  ||
				 tempPtr->getID() == IID_HOSTAGE	  ||
				 tempPtr->getID() == IID_RESTORE_HEALTH)
		{
			getStudentWorldPtr()->getPlayerPtrFromSW()->moveTo(x, y);
		}
	}
}

int Player::doDamage(int damage)
{
	//subtract hitPoints
	m_hitPoints -= damage;
	getStudentWorldPtr()->playSound(SOUND_PLAYER_IMPACT);

	//check if alive
	if (m_hitPoints <= 0)
	{
		setAlive(false);
		return GWSTATUS_PLAYER_DIED;
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Bullet::doSomething()
{
	//Used for making sure the bullet collides with other objects
	bool enemyHit  = false;
	bool playerHit = false;
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (tempPtr == nullptr)
	{
		//Do nothing
	}
	else if (tempPtr->getID() == IID_WALL      ||
			 tempPtr->getID() == IID_GANGSTER  ||
			 tempPtr->getID() == IID_WATERPOOL ||
			 tempPtr->getID() == IID_BULLY     ||
			 tempPtr->getID() == IID_ROBOT_BOSS)
	{
		//Damage enemy/Wall
		enemyHit = true;
		tempPtr->doDamage(2);
		setAlive(false);
		setVisible(false);
	}

	//Check if player hit
	if (checkIfPlayerHit(playerHit) == GWSTATUS_PLAYER_DIED)
	{
		return GWSTATUS_PLAYER_DIED;
	}

	makeAMove();

	//Double check that enemy or player is hit after bullet moves
	//This will ensure the bullet does not pass the player or enemy
	if (tempPtr == nullptr && !enemyHit)
	{
		//Do nothing
	}
	else if ((tempPtr->getID() == IID_WALL        ||
		      tempPtr->getID() == IID_GANGSTER    ||
			  tempPtr->getID() == IID_WATERPOOL   ||
			  tempPtr->getID() == IID_BULLY       ||
			  tempPtr->getID() == IID_ROBOT_BOSS) &&
		      !enemyHit)
	{
		tempPtr->doDamage(2);
		setAlive(false);
		setVisible(false);
	}

	if (checkIfPlayerHit(playerHit) == GWSTATUS_PLAYER_DIED &&
		!playerHit)
	{
		return GWSTATUS_PLAYER_DIED;
	}

	return GWSTATUS_CONTINUE_GAME;
}

int Bullet::checkIfPlayerHit(bool& hit)
{
	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		if (getStudentWorldPtr()->getPlayerPtrFromSW()->doDamage(2) == GWSTATUS_PLAYER_DIED)
		{
			return GWSTATUS_PLAYER_DIED;
		}
		hit = true;
		setAlive(false);
		setVisible(false);
	}

	return GWSTATUS_CONTINUE_GAME;
}

void Bullet::makeAMove()
{
	int x = getX();
	int y = getY();

	//Move based off of direction
	switch (getDirection())
	{
	case    up: y++;
		break;
	case  down: y--;
		break;
	case right: x++;
		break;
	case  left: x--;
		break;
	}

	Actor *tempPtr  = getStudentWorldPtr()->findAndReturnNest(x, y);

	if (tempPtr != nullptr)
	{
		Actor *tempPtr1 = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(x, y);
		
		//Check to see if it hit an object
		if (tempPtr1 == nullptr)
		{
			setAlive(false);
			setVisible(false);
		}
	}
	//move
	moveTo(x, y);
}

int AmmoGoodie::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (tempPtr == nullptr)
	{
		//Do nothing
	}
	else if (tempPtr->getID() == IID_BULLY)
	{
		//Pick up goodie and add to bullyVec
		Bully *tempPtr1 = nullptr;
		tempPtr1 = static_cast<Bully*>(tempPtr);
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_BULLY_MUNCH);
		tempPtr1->addGoodie(IID_AMMO);
		return GWSTATUS_CONTINUE_GAME;
	}

	//Pick up if player is on same coord
	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		getStudentWorldPtr()->getPlayerPtrFromSW()->increaseAmmo(25);
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_GOODIE);
		getStudentWorldPtr()->increaseScore(150);
	}
	return GWSTATUS_CONTINUE_GAME;
}

int GoldGoodie::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	//Pick up Goodie if player is on same coord
	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		getStudentWorldPtr()->decNumOfGold();
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_GOLD);
		getStudentWorldPtr()->increaseScore(100);
	}
	return GWSTATUS_CONTINUE_GAME;
}

int ExtraLifeGoodie::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (tempPtr == nullptr)
	{
		//Do nothing
	}
	else if (tempPtr->getID() == IID_BULLY)
	{
		//Bully picks up Goodie if on same coord
		Bully *tempPtr1 = nullptr;
		tempPtr1 = static_cast<Bully*>(tempPtr);
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_BULLY_MUNCH);
		tempPtr1->addGoodie(IID_EXTRA_LIFE);
		return GWSTATUS_CONTINUE_GAME;
	}

	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		//Player picks up Goodie if on same coord
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_GOODIE);
		getStudentWorldPtr()->increaseScore(150);
		getStudentWorldPtr()->incLives();
	}
	return GWSTATUS_CONTINUE_GAME;
}

int RestoreHealthGoodie::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());
	
	if (tempPtr == nullptr)
	{
		//Do nothing
	}
	else if (tempPtr->getID() == IID_BULLY)
	{
		//Bully picks up Goodie if on same coord
		Bully *tempPtr1 = nullptr;
		tempPtr1 = static_cast<Bully*>(tempPtr);
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_BULLY_MUNCH);
		tempPtr1->addGoodie(IID_RESTORE_HEALTH);
		return GWSTATUS_CONTINUE_GAME;
	}

	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		//Player picks up Goodie if on same coord
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_GOODIE);
		getStudentWorldPtr()->increaseScore(250);
		getStudentWorldPtr()->getPlayerPtrFromSW()->setHitPoints(20);
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Gate::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		//Transport Player if on same coord
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->getPlayerPtrFromSW()->setSubLevel(getLevelToTransportTo());
		getStudentWorldPtr()->removePlayerFromCurrentSubLevelAndPlaceInOtherSubLevel(getLevelToTransportTo());
		getStudentWorldPtr()->setLevelToTransportTo(getLevelToTransportTo());
		return GWSTATUS_FINISHED_LEVEL;
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Hostage::doSomething()
{
	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		//Pick up Hostage if player is on same coord
		getStudentWorldPtr()->decNumOfHostage();
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_GOODIE);
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Exit::doSomething()
{
	//Only exit if all Hostages, Gold, and RobotBosses are picked up or killed
	if (getStudentWorldPtr()->getNumOfGold() == 0 &&
		getStudentWorldPtr()->getNumOfHostage() == 0 &&
		getStudentWorldPtr()->getNumOfRobotBosses() == 0)
	{
		if (!isVisible())
		{
			//Reveal Exit
			getStudentWorldPtr()->playSound(SOUND_REVEAL_EXIT);
			setVisible(true);
		}
		
		Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

		if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
			getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
		{
			//Exit if player is on same coord
			setAlive(false);
			setVisible(false);
			getStudentWorldPtr()->playSound(SOUND_FINISHED_LEVEL);
			getStudentWorldPtr()->increaseScore(1500);
			getStudentWorldPtr()->increaseScore(getStudentWorldPtr()->getTimeLimit());
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

void Enemy::setTicks()
{
	//Limits the amount of moves an enemy can make
	int ticks = (28 - getStudentWorldPtr()->getLevel()) / 4;

	if (ticks < 3)
	{
		m_ticks = 3;
	}
	else
	{
		m_ticks = ticks;
	}
}

int Gangster::doSomething()
{
	//Limits the amount of move an enemy can make
	if (getTicks() != 1)
	{
		decTicks();
		return GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		setTicks();
	}

	Actor *tempPtr = nullptr;

	//Check for player and fire bullet if player is in sight
	if (getStudentWorldPtr()->checkColumnForPlayer(getX(), getY(), getDirection()))
	{
		if (getDirection() == Direction::up && getStudentWorldPtr()->getPlayerPtrFromSW()->getY() > getY())
		{
			tempPtr = new Bullet(getX(), getY() + 1, getStudentWorldPtr()->getCurrentSubLevel(), Direction::up, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}

		if (getDirection() == Direction::down && getStudentWorldPtr()->getPlayerPtrFromSW()->getY() < getY())
		{
			tempPtr = new Bullet(getX(), getY() - 1, getStudentWorldPtr()->getCurrentSubLevel(), Direction::down, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}
	}
	else if (getStudentWorldPtr()->checkRowForPlayer(getX(), getY(), getDirection()))
	{
		if (getDirection() == Direction::right && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() > getX())
		{
			tempPtr = new Bullet(getX() + 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::right, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}

		if (getDirection() == Direction::left && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() < getX())
		{
			tempPtr = new Bullet(getX() - 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::left, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}
	}

	//Move if player is nowhere to be found
	if (tempPtr == nullptr)
	{
		makeAMove();
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Gangster::doDamage(int damage)
{
	setHitPoints(getHitPoints() - damage);

	getStudentWorldPtr()->playSound(SOUND_ENEMY_IMPACT);

	//Check if dead
	if (getHitPoints() <= 0)
	{
		Actor *tempPtr = nullptr;
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_ENEMY_DIE);
		getStudentWorldPtr()->increaseScore(100);
		tempPtr = new Waterpool(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
		tempPtr->setVisible(true);
	}
	return GWSTATUS_CONTINUE_GAME;
}

void Gangster::makeAMove()
{
	int x = getX();
	int y = getY();

	switch (getDirection())
	{
	case right: x++;
		break;
	case  left: x--;
		break;
	case    up: y++;
		break;
	case  down: y--;
		break;
	}

	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(x, y);

	if (tempPtr != nullptr || (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == x &&
							   getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == y))
	{
		//Change Direction
		Direction dir = getDirection();

		switch (dir)
		{
		case right: dir = left;
			break;
		case  left: dir = right;
			break;
		case    up: dir = down;
			break;
		case  down: dir = up;
			break;
		}
		setDirection(dir);
	}
	else
	{
		//Move
		moveTo(x, y);
	}
}

int BullyNest::doSomething()
{
	if (checkForOtherBullies() && createBully())
	{
		//Create bullies if conditions are both true
		Actor *tempPtr = new Bully(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
		tempPtr->setVisible(true);
		getStudentWorldPtr()->playSound(SOUND_BULLY_BORN);
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool BullyNest::checkForOtherBullies()
{
	int count = 0;

	int xLowerLimit = getX() - 3;
	int yLowerLimit = getY() - 3;
	int xUpperLimit = getX() + 3;
	int yUpperLimit = getY() + 3;

	if (xLowerLimit < 0)
	{
		xLowerLimit = 0;
	}

	if (yLowerLimit < 0)
	{
		yLowerLimit = 0;
	}

	if (xUpperLimit > 14)
	{
		xUpperLimit = 14;
	}

	if (yUpperLimit > 14)
	{
		yUpperLimit = 14;
	}

	//Run through space surrounding bully nest and count number of bullies
	for (xLowerLimit; xLowerLimit <= xUpperLimit; xLowerLimit++)
	{
		for (int j = yLowerLimit; j <= yUpperLimit; j++)
		{
			Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(xLowerLimit, j);
			
			if (tempPtr == nullptr)
			{
				//Do nothing
			}
			else if (tempPtr->getID() == IID_BULLY)
			{
				count++;
			}
		}
	}

	Actor *tempPtr;

	tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY());

	if (tempPtr == nullptr)
	{
		//Do nothing
	}
	else if(tempPtr->getID() == IID_BULLY)
	{
		count = 3;
	}

	if (count < 3)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return true if count is under 3 and a bully is not on the bully nest
	//return false otherwise
}

bool BullyNest::createBully()
{
	//randomly return true to create a bully
	static const double fraction1 = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);

	if (static_cast<int>(rand() * fraction1 * 50 + 1) == 4)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Bully::doSomething()
{
	//Limit number of moves made
	if (getTicks() != 1)
	{
		decTicks();
		return GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		setTicks();
	}

	Actor *tempPtr = nullptr;

	//Check for player and fire bullet player in sight
	if (getStudentWorldPtr()->checkColumnForPlayer(getX(), getY(), getDirection()))
	{
		if (getDirection() == Direction::up && getStudentWorldPtr()->getPlayerPtrFromSW()->getY() > getY())
		{
			tempPtr = new Bullet(getX(), getY() + 1, getStudentWorldPtr()->getCurrentSubLevel(), Direction::up, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}

		if (getDirection() == Direction::down && getStudentWorldPtr()->getPlayerPtrFromSW()->getY() < getY())
		{
			tempPtr = new Bullet(getX(), getY() - 1, getStudentWorldPtr()->getCurrentSubLevel(), Direction::down, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}
	}
	else if (getStudentWorldPtr()->checkRowForPlayer(getX(), getY(), getDirection()))
	{
		if (getDirection() == Direction::right && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() > getX())
		{
			tempPtr = new Bullet(getX() + 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::right, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}

		if (getDirection() == Direction::left && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() < getX())
		{
			tempPtr = new Bullet(getX() - 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::left, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}
	}

	//Make a move
	if (tempPtr == nullptr)
	{
		makeAMove();
	}
	return GWSTATUS_CONTINUE_GAME;
}

int Bully::doDamage(int damage)
{
	setHitPoints(getHitPoints() - damage);
	getStudentWorldPtr()->playSound(SOUND_ENEMY_IMPACT);

	if (getHitPoints() <= 0)
	{
		//kill bully
		dumpGoodies();
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_ENEMY_DIE);
		getStudentWorldPtr()->increaseScore(10);
	}
	return GWSTATUS_CONTINUE_GAME;
}

void Bully::makeAMove()
{
	int x = getX();
	int y = getY();

	switch (getDirection())
	{
	case right: x++;
		break;
	case  left: x--;
		break;
	case    up: y++;
		break;
	case  down: y--;
		break;
	}

	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(x, y);
	
	//Make a move if straightline moves is 0 and not going to run into nest
	if (getNumStraightMovesLeft() != 0 && tempPtr == nullptr && !checkForNest(x, y))
	{
		if (!checkForPlayer()) //Will also change direction towards play if next to player
		{
			//move
			moveTo(x, y);
		}
		
		decNumStraightMovesLeft();
	}
	else
	{
		makeAChangeInMovement();
		setNumStraightMovesLeft();
	}
}

bool Bully::checkForNest(int x, int y)
{
	//returns true if location contains a nest
	Actor *tempPtr = nullptr;

	tempPtr = getStudentWorldPtr()->findAndReturnNest(x, y);

	if (tempPtr == nullptr)
	{
		return false;
	}
	else if (tempPtr->getID() == IID_BULLY_NEST)
	{
		return true;
	}

	return false;
}

int Bully::randomNumGen(int min, int max)
{
	//create random number
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);

	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

bool Bully::checkForPlayer()
{
	//turn towards player if nearby
	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() + 1 &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		setDirection(right);
		return true;
	}
	else if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() - 1 &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		setDirection(left);
		return true;
	}
	else if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY() + 1)
	{
		setDirection(up);
		return true;
	}
	else if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY() - 1)
	{
		setDirection(down);
		return true;
	}
	else
	{
		return false;
	}
}

//This function will check to see what options the bully has to make a move and randomly pick an open option
void Bully::makeAChangeInMovement()
{
	bool upward;
	bool downward;
	bool rightward;
	bool leftward;
	bool playerNotThere;
	int  choice = randomNumGen(1, 2);

	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX() + 1, getY());

	if (checkForPlayer())
	{
		return;
	}

	if (tempPtr == nullptr)
	{
		rightward = true;
	}
	else
	{
		rightward = false;
	}

	tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX() - 1, getY());

	if (tempPtr == nullptr)
	{
		leftward = true;
	}
	else
	{
		leftward = false;
	}

	tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY() + 1);

	if (tempPtr == nullptr)
	{
		upward = true;
	}
	else
	{
		upward = false;
	}

	tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(getX(), getY() - 1);

	if (tempPtr == nullptr)
	{
		downward = true;
	}
	else
	{
		downward = false;
	}

	switch (getDirection())
	{
	case right: 
		if (upward && downward)
		{
			if (choice == 1)
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
			}
			else
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
			}
		}
		else if (upward)
		{
			setDirection(up);
			moveTo(getX(), getY() + 1);
		}
		else if (downward)
		{
			setDirection(down);
			moveTo(getX(), getY() - 1);
		}
		else
		{
			setDirection(left);
		}
		break;
	case left:
		if (upward && downward)
		{
			if (choice == 1)
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
			}
			else
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
			}
		}
		else if (upward)
		{
			setDirection(up);
			moveTo(getX(), getY() + 1);
		}
		else if (downward)
		{
			setDirection(down);
			moveTo(getX(), getY() - 1);
		}
		else
		{
			setDirection(right);
		}
		break;
	case up:
		if (leftward && rightward)
		{
			if (choice == 1)
			{
				setDirection(right);
				moveTo(getX() + 1, getY());
			}
			else
			{
				setDirection(left);
				moveTo(getX() - 1, getY());
			}
		}
		else if (leftward)
		{
			setDirection(left);
			moveTo(getX() - 1, getY());
		}
		else if (rightward)
		{
			setDirection(right);
			moveTo(getX() + 1, getY());
		}
		else
		{
			setDirection(down);
		}
		break;
	case down:
		if (leftward && rightward)
		{
			if (choice == 1)
			{
				setDirection(right);
				moveTo(getX() + 1, getY());
			}
			else
			{
				setDirection(left);
				moveTo(getX() - 1, getY());
			}
		}
		else if (leftward)
		{
			setDirection(left);
			moveTo(getX() - 1, getY());
		}
		else if (rightward)
		{
			setDirection(right);
			moveTo(getX() + 1, getY());
		}
		else
		{
			setDirection(up);
		}
		break;
	}
}

void Bully::dumpGoodies()
{
	Actor *tempPtr = nullptr;

	//Create Goodies and add them to the studentworld vector
	for (int index = 0; index < m_bullyVec.size(); index++)
	{
		tempPtr = m_bullyVec[index];
		Actor *tempPtr1 = nullptr;

		switch (tempPtr->getID())
		{
		case IID_AMMO:
			tempPtr1 = new AmmoGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
			break;
		case IID_EXTRA_LIFE:
			tempPtr1 = new ExtraLifeGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
			break;
		case IID_RESTORE_HEALTH:
			tempPtr1 = new RestoreHealthGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
			break;
		}
		getStudentWorldPtr()->addToGraphVec(tempPtr1, getStudentWorldPtr()->getCurrentSubLevel());
		tempPtr1->setVisible(true);
		//free memory in bully
		delete tempPtr;
	}
}

//Add Goodies to bully vector
void Bully::addGoodie(int id)
{
	Actor *tempPtr = nullptr;

	switch (id)
	{
	case IID_AMMO:
		tempPtr = new AmmoGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		break;
	case IID_EXTRA_LIFE:
		tempPtr = new ExtraLifeGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		break;
	case IID_RESTORE_HEALTH:
		tempPtr = new RestoreHealthGoodie(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		break;
	}
	m_bullyVec.push_back(tempPtr);
}

void RobotBoss::makeAMove()
{
	int x = getX();
	int y = getY();

	//Set Direction
	switch (getDirection())
	{
	case right: x++;
		break;
	case  left: x--;
		break;
	}

	Actor *tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(x, y);

	//Check for player or other object to not walk into them
	if (tempPtr != nullptr || (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == x &&
							   getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == y))
	{
		Direction dir = getDirection();

		switch (dir)
		{
		case right: dir = left;
			break;
		case  left: dir = right;
			break;
		}
		setDirection(dir);
	}
	else
	{
		//Move
		moveTo(x, y);
	}
}

int RobotBoss::doDamage(int damage)
{
	setHitPoints(getHitPoints() - damage);
	getStudentWorldPtr()->playSound(SOUND_ENEMY_IMPACT);

	if (getHitPoints() <= 0)
	{
		//Kill RobotBoss and create hostage
		Actor *tempPtr = nullptr;
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_ENEMY_DIE);
		tempPtr = new Hostage(getX(), getY(), getStudentWorldPtr()->getCurrentSubLevel(), *getStudentWorldPtr());
		getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
		getStudentWorldPtr()->incNumOfHostage();
		getStudentWorldPtr()->decNumOfRobotBosses();
		getStudentWorldPtr()->increaseScore(100);
		tempPtr->setVisible(true);
	}

	return GWSTATUS_CONTINUE_GAME;
}

int RobotBoss::doSomething()
{
	//Limit number of moves
	if (getTicks() != 1)
	{
		decTicks();
		return GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		setTicks();
	}

	Actor *tempPtr = nullptr;

	//Check for player and fire bullet if player is in sight
	if (getStudentWorldPtr()->checkRowForPlayer(getX(), getY(), getDirection()))
	{
		if (getDirection() == Direction::right && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() > getX())
		{
			tempPtr = new Bullet(getX() + 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::right, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}

		if (getDirection() == Direction::left && getStudentWorldPtr()->getPlayerPtrFromSW()->getX() < getX())
		{
			tempPtr = new Bullet(getX() - 1, getY(), getStudentWorldPtr()->getCurrentSubLevel(), Direction::left, *getStudentWorldPtr());
			getStudentWorldPtr()->addToGraphVec(tempPtr, getStudentWorldPtr()->getCurrentSubLevel());
			getStudentWorldPtr()->playSound(SOUND_ENEMY_FIRE);
			tempPtr->setVisible(true);
		}
	}

	if (tempPtr == nullptr)
	{
		//Move
		makeAMove();
	}
	return GWSTATUS_CONTINUE_GAME;
}

int FarplaneGun::doSomething()
{
	//Check for player at location
	if (getStudentWorldPtr()->getPlayerPtrFromSW()->getX() == getX() &&
		getStudentWorldPtr()->getPlayerPtrFromSW()->getY() == getY())
	{
		//Scan area and destroy bullies and Gangsters in area
		int xLowerLimit = getX() - 4;
		int xUpperLimit = getX() + 4;
		int yLowerLimit = getY() - 4;
		int yUpperLimit = getY() + 4;

		if (xLowerLimit < 0)
		{
			xLowerLimit = 0;
		}

		if (xUpperLimit > 14)
		{
			xUpperLimit = 14;
		}

		if (yLowerLimit < 0)
		{
			yLowerLimit = 0;
		}

		if (yUpperLimit > 14)
		{
			yUpperLimit = 14;
		}

		Actor *tempPtr = nullptr;
		for (xLowerLimit; xLowerLimit <= xUpperLimit; xLowerLimit++)
		{
			for (int j = yLowerLimit; j <= yUpperLimit; j++)
			{
				tempPtr = getStudentWorldPtr()->findAndReturnGraphObjectAtLocation(xLowerLimit, j);

				if (tempPtr == nullptr)
				{
					//do nothing
				}
				else if (tempPtr->getID() == IID_GANGSTER ||
						 tempPtr->getID() == IID_BULLY)
				{
					tempPtr->doDamage(10);
				}
			}
		}

		//Damage player
		if (getStudentWorldPtr()->getPlayerPtrFromSW()->doDamage(10) == GWSTATUS_PLAYER_DIED)
		{
			return GWSTATUS_PLAYER_DIED;
		}
		setAlive(false);
		setVisible(false);
		getStudentWorldPtr()->playSound(SOUND_GOT_FARPLANE_GUN);
	}
	return GWSTATUS_CONTINUE_GAME;
}

int FarplaneGun::doDamage(int damage)
{
	return GWSTATUS_CONTINUE_GAME;
}
