#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include "Level.h"
#include <list>
#include <vector>
#include <string>
#include <algorithm>

//Remove this
#include <iostream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Player;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_levelSize(0), m_subLevel(0), m_playerPtr(nullptr), m_timeLimit(3000), m_numOfGold(0), m_numOfHostage(0), m_numOfRobotBosses(0)
	{}

	virtual ~StudentWorld()
	{}

	virtual int init()
	{
		return loadLevel();
	}

	virtual int move();

	virtual void cleanUp();

	void cleanUpDeadGraphObjects();

	virtual int getCurrentSubLevel()
	{
		return m_subLevel;
	}

	int loadLevel();
	int loadSubLevels();
	void constructObjects();

	void decTimeLimit()
	{
		m_timeLimit--;
	}

	void advanceToNextSubLevel()
	{
		++m_subLevel;
	}

	void advanceToPrevSubLevel()
	{
		--m_subLevel;
	}

	void setLevelToTransportTo(int level)
	{
		m_subLevel = level;
	}

	Player* getPlayerPtrFromSW()
	{
		return m_playerPtr;
	}

	Actor* findAndReturnGraphObjectAtLocation(int x, int y);

	Actor* findAndReturnNest(int x, int y);

	Actor* findAndReturnFakeWall(int x, int y);

	void setDisplayText();

	void setTimeLimit(int timeLimit)
	{
		m_timeLimit = timeLimit;
	}

	int getTimeLimit()
	{
		return m_timeLimit;
	}

	//Add object to graphVector
	void addToGraphVec(Actor *tempPtr, int sub_level)
	{
		m_graphVec[sub_level].push_back(tempPtr);
	}

	void removePlayerFromCurrentSubLevelAndPlaceInOtherSubLevel(int subLevel);

	void setLastX(int x)
	{
		m_lastX = x;
	}

	void setLastY(int y)
	{
		m_lastY = y;
	}

	int getLastX()
	{
		return m_lastX;
	}

	int getLastY()
	{
		return m_lastY;
	}

	void setNumOfGold(int num)
	{
		m_numOfGold = num;
	}

	void setNumOfHostage(int num)
	{
		m_numOfHostage = num;
	}

	void setNumOfRobotBosses(int num)
	{
		m_numOfRobotBosses = num;
	}

	void incNumOfGold()
	{
		m_numOfGold++;
	}

	void incNumOfHostage()
	{
		m_numOfHostage++;
	}

	void incNumOfRobotBosses()
	{
		m_numOfRobotBosses++;
	}

	void decNumOfGold()
	{
		m_numOfGold--;
	}

	void decNumOfHostage()
	{
		m_numOfHostage--;
	}

	void decNumOfRobotBosses()
	{
		m_numOfRobotBosses--;
	}

	int getNumOfGold()
	{
		return m_numOfGold;
	}

	int getNumOfHostage()
	{
		return m_numOfHostage;
	}

	int getNumOfRobotBosses()
	{
		return m_numOfRobotBosses;
	}

	void getStartPointOfPlayer(int& x, int& y, int sub_level);

	bool checkColumnForPlayer(int x, int y, GraphObject::Direction dir);

	bool checkRowForPlayer(int x, int y, GraphObject::Direction dir);

private:
	std::vector<Level*> m_lev;
	unsigned int m_levelSize;
	unsigned int m_subLevel;
	int m_timeLimit;
	int m_lastX;
	int m_lastY;
	int m_numOfGold;
	int m_numOfHostage;
	int m_numOfRobotBosses;
	std::vector< std::vector<Actor*> > m_graphVec;
	Player *m_playerPtr;
};

#endif // STUDENTWORLD_H_
