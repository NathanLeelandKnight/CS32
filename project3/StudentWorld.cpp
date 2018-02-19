#include "StudentWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

int StudentWorld::loadLevel()
{
	//Convert to string to load level
	ostringstream convert;
	convert << "level";

	if (GameWorld::getLevel() < 10)
		convert << "0";

	convert << GameWorld::getLevel();
	convert << ".dat";

	//Load level
	string currentLevel = convert.str();
	Level *tempLev;
	tempLev = new Level(assetDirectory());
	Level::LoadResult result = tempLev->loadLevel(currentLevel);

	//Check result 
	if (result == Level::load_success)
	{
		//If successful add to level vector
		m_lev.push_back(tempLev);
		m_levelSize++;
		if (loadSubLevels() == GWSTATUS_LEVEL_ERROR)
		{
			return GWSTATUS_LEVEL_ERROR;
		}
		return GWSTATUS_CONTINUE_GAME;
	}
	else if (result == Level::load_fail_bad_format)
	{
		return GWSTATUS_LEVEL_ERROR;
	}
	else if (result == Level::load_fail_file_not_found)
	{
		return GWSTATUS_PLAYER_WON;
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::loadSubLevels()
{
	//Convert to string for subLevel
	ostringstream convert;
	string currentSubLevel;
	unsigned int subLevel = 1;
	Level *tempLev = new Level(assetDirectory());

	convert << "level";

	if (GameWorld::getLevel() < 10)
		convert << "0";

	convert << GameWorld::getLevel() << "_" << subLevel << ".dat";

	currentSubLevel = convert.str();

	//Load level
	Level::LoadResult result = tempLev->loadLevel(currentSubLevel, subLevel);

	while (subLevel < MAX_SUB_LEVEL && result == Level::load_sublevel_success)
	{
		//Add all subLevels
		m_lev.push_back(tempLev);
		m_levelSize++;
		subLevel++;
		tempLev = new Level(assetDirectory());
		convert.str("");
		convert.clear();
		convert << "level";

		if (GameWorld::getLevel() < 10)
			convert << "0";

		convert << GameWorld::getLevel() << "_" << subLevel << ".dat";

		currentSubLevel = convert.str();

		result = tempLev->loadLevel(currentSubLevel, subLevel);
	}

	//Check results
	if (result == Level::load_sublevel_fail_file_not_found)
	{
		delete tempLev;
	}
	else if (result == Level::load_sublevel_fail_bad_format)
	{
		return GWSTATUS_LEVEL_ERROR;
	}

	tempLev = nullptr;

	//Construct Objects 
	StudentWorld::constructObjects();
	return GWSTATUS_CONTINUE_GAME;

}

void StudentWorld::constructObjects()
{
	int playerCount = 0;
	Actor *tempPtr = nullptr;
	std::vector<Actor*> tempVec;

	//Run through level vector constructing objects and placing them in the correct subLevels
	for (unsigned int i = 0; i < m_levelSize; i++)
	{
		for (unsigned int j = 0; j < VIEW_WIDTH; j++)
		{
			for (unsigned int k = 0; k < VIEW_HEIGHT; k++)
			{
				switch (m_lev[i]->getContentsOf(j, k, i))
				{
				case Level::MazeEntry::wall:
					tempPtr = new Wall(IID_WALL, j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::fake_wall:
					tempPtr = new Wall(IID_FAKE_WALL, j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::hostage:
					tempPtr = new Hostage(j, k, i, *this);
					tempVec.push_back(tempPtr);
					incNumOfHostage();
					break;
				case Level::MazeEntry::player:
					if (playerCount != 1)
					{
						m_playerPtr = new Player(j, k, 0, *this, 20, 0);
						m_playerPtr->setVisible(true);
						tempVec.push_back(m_playerPtr);
						playerCount++;
					}
					break;
				case Level::MazeEntry::ammo:
					tempPtr = new AmmoGoodie(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gold:
					tempPtr = new GoldGoodie(j, k, i, *this);
					tempVec.push_back(tempPtr);
					incNumOfGold();
					break;
				case Level::MazeEntry::gate0:
					tempPtr = new Gate(j, k, i, *this, 0);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gate1:
					tempPtr = new Gate(j, k, i, *this, 1);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gate2:
					tempPtr = new Gate(j, k, i, *this, 2);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gate3:
					tempPtr = new Gate(j, k, i, *this, 3);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gate4:
					tempPtr = new Gate(j, k, i, *this, 4);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::gate5:
					tempPtr = new Gate(j, k, i, *this, 5);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::extra_life:
					tempPtr = new ExtraLifeGoodie(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::restore_health:
					tempPtr = new RestoreHealthGoodie(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::exit:
					tempPtr = new Exit(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::vert_gangster:
					tempPtr = new Gangster(j, k, i, *this, GraphObject::Direction::down);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::horiz_gangster:
					tempPtr = new Gangster(j, k, i, *this, GraphObject::Direction::right);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::bully_nest:
					tempPtr = new BullyNest(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				case Level::MazeEntry::robot_boss:
					tempPtr = new RobotBoss(j, k, i, *this);
					tempVec.push_back(tempPtr);
					incNumOfRobotBosses();
					break;
				case Level::MazeEntry::farplane_gun:
					tempPtr = new FarplaneGun(j, k, i, *this);
					tempVec.push_back(tempPtr);
					break;
				}
				if (m_lev[i]->getContentsOf(j, k, i) != Level::MazeEntry::exit)
				{
					tempPtr->setVisible(true);
				}
			}
		}

		//Add objects to graph Vector
		m_graphVec.push_back(tempVec);
		tempVec.clear();
	}
}

Actor* StudentWorld::findAndReturnGraphObjectAtLocation(int x, int y)
{
	Actor *tempPtr = nullptr;

	//Find and return solid objects that a player or enemy cannot move through
	for (unsigned int index = 0; index < m_graphVec[m_subLevel].size(); index++)
	{
		if (m_graphVec[m_subLevel][index]->getX()  == x				   &&
			m_graphVec[m_subLevel][index]->getY()  == y				   &&
			m_graphVec[m_subLevel][index]->getID() != IID_FAKE_WALL    &&
			m_graphVec[m_subLevel][index]->getID() != IID_AMMO		   &&
			m_graphVec[m_subLevel][index]->getID() != IID_BULLET	   &&
			m_graphVec[m_subLevel][index]->getID() != IID_EXIT		   &&
			m_graphVec[m_subLevel][index]->getID() != IID_EXTRA_LIFE   &&
			m_graphVec[m_subLevel][index]->getID() != IID_FARPLANE_GUN &&
			m_graphVec[m_subLevel][index]->getID() != IID_GATE		   &&
			m_graphVec[m_subLevel][index]->getID() != IID_GOLD		   &&
			m_graphVec[m_subLevel][index]->getID() != IID_HOSTAGE	   &&
			m_graphVec[m_subLevel][index]->getID() != IID_PLAYER	   &&
			m_graphVec[m_subLevel][index]->getID() != IID_BULLY_NEST   &&
			m_graphVec[m_subLevel][index]->getID() != IID_RESTORE_HEALTH)
		{
			tempPtr = m_graphVec[m_subLevel][index];
			return tempPtr;
		}
	}
	return tempPtr;
}

int StudentWorld::move()
{
	setDisplayText();
	decTimeLimit();

	//If time expires
	if (getTimeLimit() == 0)
	{
		//decrease lives by one and reset to play again
		decLives();
		setTimeLimit(3000);
		setNumOfGold(0);
		setNumOfHostage(0);
		setNumOfRobotBosses(0);
		playSound(SOUND_PLAYER_DIE);
		return GWSTATUS_PLAYER_DIED;
	}

	//Run through all graph objects and tell them to do something
	int subLevel = getCurrentSubLevel();
	int size = m_graphVec[subLevel].size();
	for (int i = 0; i < size; i++)
	{
		int status = m_graphVec[subLevel][i]->doSomething();
		if (status == GWSTATUS_PLAYER_DIED)
		{
			//If player dies reset
			decLives();
			setTimeLimit(3000);
			setNumOfGold(0);
			setNumOfHostage(0);
			setNumOfRobotBosses(0);
			playSound(SOUND_PLAYER_DIE);
			return GWSTATUS_PLAYER_DIED;
		}
		else if (status == GWSTATUS_FINISHED_LEVEL &&
				 getNumOfGold()        == 0        &&
				 getNumOfHostage()     == 0        &&
				 getNumOfRobotBosses() == 0        &&
				 m_graphVec[subLevel][i]->getID() != IID_GATE)
		{
			//Can only win when all gold, hostages, and robotbosses are collected/killed
			cleanUpDeadGraphObjects();
			return GWSTATUS_FINISHED_LEVEL;
		}
		else if (status == GWSTATUS_FINISHED_LEVEL)
		{
			cleanUpDeadGraphObjects();
			return GWSTATUS_CONTINUE_GAME;
		}
	}
	
	//deallocate all memory
	cleanUpDeadGraphObjects();

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setDisplayText()
{
	//convert game text to a string and call display function for it to appear
	int score     = getScore();
	int level     = getLevel();
	int subLevel  = getCurrentSubLevel();
	int timeLimit = getTimeLimit();
	int livesLeft = getLives();
	int health    = m_playerPtr->getHitPoints();
	int ammo      = m_playerPtr->getAmmo();

	ostringstream convert;
	convert << "Score: ";

	if (score < 10)
	{
		convert << "000000";
	}
	else if (score < 100)
	{
		convert << "00000";
	}
	else if (score < 1000)
	{
		convert << "0000";
	}
	else if (score < 10000)
	{
		convert << "000";
	}
	else if (score < 100000)
	{
		convert << "00";
	}
	else if (score < 1000000)
	{
		convert << "0";
	}

	convert << score << "  ";
	convert << "Level: ";

	if (level < 10)
	{
		convert << "0";
	}

	convert << level << "_0" << subLevel << "  ";
	convert << "Lives: " << livesLeft << "  ";
	convert << "Health: ";

	switch (health)
	{
	case 20: convert << "100";
		break;
	case 19: convert << "95";
		break;
	case 18: convert << "90";
		break;
	case 17: convert << "85";
		break;
	case 16: convert << "80";
		break;
	case 15: convert << "75";
		break;
	case 14: convert << "70";
		break;
	case 13: convert << "65";
		break;
	case 12: convert << "60";
		break;
	case 11: convert << "55";
		break;
	case 10: convert << "50";
		break;
	case 9: convert << "45";
		break;
	case 8: convert << "40";
		break;
	case 7: convert << "35";
		break;
	case 6: convert << "30";
		break;
	case 5: convert << "25";
		break;
	case 4: convert << "20";
		break;
	case 3: convert << "15";
		break;
	case 2: convert << "10";
		break;
	case 1: convert << "5";
		break;
	case 0: convert << "0";
		break;
	}

	convert << "%  ";
	convert << "Ammo: " << ammo << "  ";
	convert << "TimeLimit: " << timeLimit;

	setGameStatText(convert.str());
}

void StudentWorld::cleanUpDeadGraphObjects()
{
	//Run through vector and deallocate memory
	vector<Actor*>::iterator it = m_graphVec[m_subLevel].begin();

	while (it != m_graphVec[m_subLevel].end())
	{
		if ((*it)->checkIfAlive() == false)
		{
			delete *it;
			it = m_graphVec[m_subLevel].erase(it);
		}
		else
		{
			it++;
		}
	}
}

void StudentWorld::removePlayerFromCurrentSubLevelAndPlaceInOtherSubLevel(int subLevel)
{
	//This function will remove the player from the current subLevel and transport the player to the new subLevel
	Player *tempPtr = m_playerPtr;

	int currentSubLevel = m_subLevel;

	std::vector<Actor*>::iterator it = m_graphVec[currentSubLevel].begin();

	//Remove player
	while (it != m_graphVec[currentSubLevel].end())
	{
		if (*it == tempPtr)
		{
			it = m_graphVec[currentSubLevel].erase(it);
		}
		else
		{
			it++;
		}
	}

	//Remember where the player went through the gate
	//This will be the location the player will be at when being transported back
	if (currentSubLevel == 0)
	{
		setLastX(m_playerPtr->getX());
		setLastY(m_playerPtr->getY());
		int x, y;
		getStartPointOfPlayer(x, y, subLevel);
		tempPtr = new Player(x,
							 y,
							 subLevel,
							 *this,
							 m_playerPtr->getHitPoints(),
							 m_playerPtr->getAmmo());
	}
	else
	{
		//Get last position will store the subLevel 0 position
		tempPtr = new Player(getLastX(),
							 getLastY(),
							 subLevel,
							 *this,
							 m_playerPtr->getHitPoints(),
							 m_playerPtr->getAmmo());
	}

	//set player visible in new sublevel and delete old player
	tempPtr->setVisible(true);
	delete m_playerPtr;
	m_playerPtr = tempPtr;
	Actor *tempPtr1 = m_playerPtr;
	m_graphVec[subLevel].push_back(tempPtr1);
}

Actor* StudentWorld::findAndReturnNest(int x, int y)
{
	Actor *tempPtr = nullptr;

	for (unsigned int index = 0; index < m_graphVec[m_subLevel].size(); index++)
	{
		if (m_graphVec[m_subLevel][index]->getX()  == x &&
			m_graphVec[m_subLevel][index]->getY()  == y &&
			m_graphVec[m_subLevel][index]->getID() == IID_BULLY_NEST)
		{
			tempPtr = m_graphVec[m_subLevel][index];
			return tempPtr;
		}
	}
	return tempPtr;
}

Actor* StudentWorld::findAndReturnFakeWall(int x, int y)
{
	//Find and return the object fake wall at location specified
	Actor *tempPtr = nullptr;

	for (unsigned int index = 0; index < m_graphVec[m_subLevel].size(); index++)
	{
		if (m_graphVec[m_subLevel][index]->getX()  == x &&
			m_graphVec[m_subLevel][index]->getY()  == y &&
			m_graphVec[m_subLevel][index]->getID() == IID_FAKE_WALL)
		{
			tempPtr = m_graphVec[m_subLevel][index];
			return tempPtr;
		}
	}
	return tempPtr;
}

void StudentWorld::cleanUp()
{
	// Deallocate memory
	for (unsigned int i = 0; i < m_levelSize; i++)
	{
		std::vector<Actor*>::iterator it = m_graphVec[i].begin();
		while (it != m_graphVec[i].end())
		{
			delete *it;
			it = m_graphVec[i].erase(it);
		}
	}
	std::vector< std::vector<Actor*> >::iterator it = m_graphVec.begin();

	while (it != m_graphVec.end())
	{
		it = m_graphVec.erase(it);
	}

	std::vector<Level*>::iterator it1 = m_lev.begin();

	while (it1 != m_lev.end())
	{
		delete *it1;
		it1 = m_lev.erase(it1);
	}
	// Set all values back to default
	m_levelSize = 0;
	m_subLevel = 0;
	m_numOfGold = 0;
	m_numOfHostage = 0;
	m_timeLimit = 3000;
}

void StudentWorld::getStartPointOfPlayer(int& x, int& y, int sub_level)
{
	//Locate the start point of the player for each subLevel
	for (unsigned int j = 0; j < VIEW_WIDTH; j++)
	{
		for (unsigned int k = 0; k < VIEW_HEIGHT; k++)
		{
			if (m_lev[sub_level]->getContentsOf(j, k, sub_level) == Level::MazeEntry::player)
			{
				x = j;
				y = k;
				return;
			}
		}
	}
}

//This function will check if the player is located in a particular column and is also visible to the object looking for the player
//Meaning that nothing is blocking the player and the player is not hiding behind a fakewall
bool StudentWorld::checkColumnForPlayer(int x, int y, GraphObject::Direction dir)
{
	if (m_playerPtr->getX() == x)
	{
		//Check above
		if (m_playerPtr->getY() > y && dir == GraphObject::Direction::up)
		{
			for (y; y < m_playerPtr->getY(); y++)
			{
				Actor *tempPtr = findAndReturnGraphObjectAtLocation(x, y + 1);

				if (tempPtr != nullptr)
				{
					return false;
				}

				Actor *tempPtr1 = findAndReturnFakeWall(x, y + 1);

				if (tempPtr1 != nullptr)
				{
					return false;
				}
			}
			return true;
		}

		//Check below
		if (m_playerPtr->getY() < y && dir == GraphObject::Direction::down)
		{
			for (y; y > m_playerPtr->getY(); y--)
			{
				Actor *tempPtr = findAndReturnGraphObjectAtLocation(x, y - 1);

				if (tempPtr != nullptr)
				{
					return false;
				}

				Actor *tempPtr1 = findAndReturnFakeWall(x, y - 1);

				if (tempPtr1 != nullptr)
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

//This function will check if the player is located in a particular row and is also visible to the object looking for the player
//Meaning that nothing is blocking the player and the player is not hiding behind a fakewall
bool StudentWorld::checkRowForPlayer(int x, int y, GraphObject::Direction dir)
{
	if (m_playerPtr->getY() == y)
	{
		//Check right
		if (m_playerPtr->getX() > x && dir == GraphObject::Direction::right)
		{
			for (x; x < m_playerPtr->getX(); x++)
			{
				Actor *tempPtr = findAndReturnGraphObjectAtLocation(x + 1, y);

				if (tempPtr != nullptr)
				{
					return false;
				}

				Actor *tempPtr1 = findAndReturnFakeWall(x + 1, y);

				if (tempPtr1 != nullptr)
				{
					return false;
				}
			}
			return true;
		}

		//Check left
		if (m_playerPtr->getX() < x && dir == GraphObject::Direction::left)
		{
			for (x; x > m_playerPtr->getX(); x--)
			{
				Actor *tempPtr = findAndReturnGraphObjectAtLocation(x - 1, y);

				if (tempPtr != nullptr)
				{
					return false;
				}

				Actor *tempPtr1 = findAndReturnFakeWall(x - 1, y);

				if (tempPtr1 != nullptr)
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}