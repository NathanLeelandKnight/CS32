#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int id, int x, int y, GraphObject::Direction dir, int sub_level, StudentWorld& other)
		:GraphObject(id, x, y, dir, sub_level), m_studentWorldPtr(nullptr)
	{
		setStudentWorldPtr(other);
		setAlive(true);
	}

	virtual ~Actor()
	{}

	void setAlive(bool alive)
	{
		m_alive = alive;
	}

	bool checkIfAlive()
	{
		return m_alive;
	}

	virtual int doSomething() = 0;
	virtual int doDamage(int damage) = 0;

	void setStudentWorldPtr(StudentWorld& other);

	StudentWorld* getStudentWorldPtr()
	{
		return m_studentWorldPtr;
	}

private:
	bool m_alive;
	StudentWorld *m_studentWorldPtr;
};

//Used for Walls and Fake Walls
class Wall : public Actor
{
public:
	Wall(int id, int x, int y, int sub_level, StudentWorld& other)
		:Actor(id, x, y, GraphObject::Direction::none, sub_level, other)
	{}

	virtual ~Wall()
	{}

	virtual int doSomething()
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class Hostage : public Actor
{
public:
	Hostage(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_HOSTAGE, x, y, GraphObject::Direction::none, sub_level, other)
	{}

	virtual ~Hostage()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}

};

class Player : public Actor
{
public:
	Player(int x, int y, int subLevel, StudentWorld& other, int hitPoints, int ammo)
		:Actor(IID_PLAYER, x, y, GraphObject::Direction::right, subLevel, other), 
		 m_hitPoints(hitPoints), m_ammo(ammo), m_subLevel(subLevel), m_firedBullet(false)
	{}

	virtual ~Player()
	{}

	virtual int doSomething();

	void makeAMove(int keyPressed);

	void increaseAmmo(int ammo)
	{
		m_ammo += ammo;
	}

	void decAmmo()
	{
		m_ammo--;
	}

	int getHitPoints()
	{
		return m_hitPoints;
	}

	int getAmmo()
	{
		return m_ammo;
	}

	void setSubLevel(int sub_level)
	{
		m_subLevel = sub_level;
	}

	int getSubLevel()
	{
		return m_subLevel;
	}

	void setHitPoints(int hitPoints)
	{
		m_hitPoints = hitPoints;
	}

	virtual int doDamage(int damage);

	void setFiredBullet(bool firedBullet)
	{
		m_firedBullet = firedBullet;
	}

	bool getFiredBullet()
	{
		return m_firedBullet;
	}

private:
	int m_hitPoints;
	int m_ammo;
	int m_subLevel;
	bool m_firedBullet;
};

class Bullet : public Actor
{
public:
	Bullet(int x, int y, int sub_level, GraphObject::Direction dir, StudentWorld& other)
		:Actor(IID_BULLET, x, y, dir, sub_level, other)
	{}

	virtual ~Bullet()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	int checkIfPlayerHit(bool& hit);

	void makeAMove();

};

class AmmoGoodie : public Actor
{
public:
	AmmoGoodie(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_AMMO, x, y, Direction::none, sub_level, other)
	{}

	virtual ~AmmoGoodie()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class GoldGoodie : public Actor
{
public:
	GoldGoodie(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_GOLD, x, y, Direction::none, sub_level, other)
	{}

	virtual ~GoldGoodie()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class Gate : public Actor
{
public:
	Gate(int x, int y, int sub_level, StudentWorld& other, int transportLevel)
		:Actor(IID_GATE, x, y, Direction::none, sub_level, other), m_level(transportLevel)
	{}

	virtual ~Gate()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	int getLevelToTransportTo()
	{
		return m_level;
	}

private:
	int m_level;
};

class ExtraLifeGoodie : public Actor
{
public:
	ExtraLifeGoodie(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_EXTRA_LIFE, x, y, Direction::none, sub_level, other)
	{}

	virtual ~ExtraLifeGoodie()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class RestoreHealthGoodie : public Actor
{
public:
	RestoreHealthGoodie(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_RESTORE_HEALTH, x, y, Direction::none, sub_level, other)
	{}

	virtual ~RestoreHealthGoodie()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class Exit : public Actor
{
public:
	Exit(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_EXIT, x, y, Direction::none, sub_level, other)
	{}

	virtual ~Exit()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}
};

class Enemy : public Actor
{
public:
	Enemy(int id, int x, int y, int sub_level, StudentWorld& other, Direction dir, int hitPoints)
		: Actor(id, x, y, dir, sub_level, other), m_hitPoints(hitPoints)
	{
		setTicks();
	}

	virtual ~Enemy()
	{}

	void setTicks();

	void decTicks()
	{
		m_ticks--;
	}

	int getTicks()
	{
		return m_ticks;
	}

	void setHitPoints(int hitPoints)
	{
		m_hitPoints = hitPoints;
	}

	int getHitPoints()
	{
		return m_hitPoints;
	}

	virtual void makeAMove() = 0;
private:
	int m_hitPoints;
	int m_ticks;
};

class Gangster : public Enemy
{
public:
	Gangster(int x, int y, int sub_level, StudentWorld& other, Direction dir)
		:Enemy(IID_GANGSTER, x, y, sub_level, other, dir, 10)
	{}

	virtual ~Gangster()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage);

	virtual void makeAMove();
};

class Waterpool : public Enemy
{
public:
	Waterpool(int x, int y, int sub_level, StudentWorld& other)
		:Enemy(IID_WATERPOOL, x, y, sub_level, other, none, 30)
	{}

	virtual ~Waterpool()
	{}

	virtual int doSomething()
	{
		doDamage(1);
		return GWSTATUS_CONTINUE_GAME;
	}

	//Does 1 damage every tick
	virtual int doDamage(int damage)
	{
		setHitPoints(getHitPoints() - 1);

		if (getHitPoints() <= 0)
		{
			setAlive(false);
			setVisible(false);
		}
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void makeAMove()
	{
		//Do nothing
	}
};

class BullyNest : public Actor
{
public:
	BullyNest(int x, int y, int sub_level, StudentWorld& other)
		: Actor(IID_BULLY_NEST, x, y, none, sub_level, other)
	{}

	virtual ~BullyNest()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage)
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	bool checkForOtherBullies();

	bool createBully();
};

class Bully : public Enemy
{
public:
	Bully(int x, int y, int sub_level, StudentWorld& other)
		:Enemy(IID_BULLY, x, y, sub_level, other, right, 5), 
		 m_numStraightMoves(randomNumGen(1, 6)), 
		 m_numStraightMovesLeft(m_numStraightMoves)
	{}

	virtual ~Bully()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage);

	void makeAMove();

	int randomNumGen(int min, int max);

	void decNumStraightMovesLeft()
	{
		m_numStraightMovesLeft--;
	}

	void setNumStraightMovesLeft()
	{
		m_numStraightMovesLeft = getNumStraightMoves();
	}

	int getNumStraightMovesLeft()
	{
		return m_numStraightMovesLeft;
	}

	int getNumStraightMoves()
	{
		return m_numStraightMoves;
	}

	void makeAChangeInMovement();

	bool checkForPlayer();

	bool checkForNest(int x, int y);

	void addGoodie(int id);

	void dumpGoodies();

private:
	int m_numStraightMoves;
	int m_numStraightMovesLeft;
	std::vector<Actor*> m_bullyVec; //Holds graphObjects that the bully picked up
};

class RobotBoss : public Enemy
{
public:
	RobotBoss(int x, int y, int sub_level, StudentWorld& other)
		:Enemy(IID_ROBOT_BOSS, x, y, sub_level, other, right, 50)
	{}

	virtual ~RobotBoss()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage);

	virtual void makeAMove();
};

class FarplaneGun : public Actor
{
public:
	FarplaneGun(int x, int y, int sub_level, StudentWorld& other)
		:Actor(IID_FARPLANE_GUN, x, y, none, sub_level, other)
	{}

	virtual ~FarplaneGun()
	{}

	virtual int doSomething();

	virtual int doDamage(int damage);
};
#endif // ACTOR_H_
