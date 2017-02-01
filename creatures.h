#ifndef R_CREATURES
#define R_CREATURES

#include "wx/icon.h"			//to allow us to have icons in things
#include "wx/string.h"			//to allow us to convert ascii filenames to wxString

//to give random numbers between 0 and max-1
#include <stdlib.h>

#define MAXX 580
#define MAXY 580
#define CLIENTX (MAXX + 20)
#define CLIENTY (MAXY + 20)

//what states can the tokens be in?
enum rState
{
  DEAD = 1000,
  DYING,
  ALIVE,
  INVULNERABLE,
  CAPTURED,
  UNBORN,
};

//what types of bonus are there?
enum rBonusType
{
  LIFE = 100,
  POWERUP,
  BOMB,
  UNKILLABLE,
  BONUSSCORE,
};

//base class for all objects used in the game
class rToken
{
public:
  //constructors
  rToken ();
  virtual ~ rToken ();

  //data
  int x, y;						//position. Direct access for speed
  wxIcon *myIcon;				//the icon to print NOW
  int moveCounter;				//how often can I move?

  //accessors
  void SetState (rState state);
  rState GetState ();
  void SetLives (int i);
  int GetLives ();

  //methods
  virtual void MoveMe (int prx = 0, int pry = 0);	//MUST be Overridden in all derived classes
  //prx & pry are to give  position to move towards if required
  void LoseALife ();			//sets myState if changed
  int RandomNo (float max);

protected:
    rState myState;				//what condition am I in?
  int lives;					//how many times can I be hit
};

//define the Sheep
class rSheep:public rToken
{
public:
  rSheep ();
  ~rSheep ();

  //data
  wxIcon *iconList[3];			//three icons to be used as needed

  //methods
  void MoveMe (int prx = 0, int pry = 0);
};

//define the rabbit
class rRabbit:public rToken
{
public:
  rRabbit ();
  ~rRabbit ();

  //data
  int xMove, yMove;				//set by GAME accessors directly
  wxIcon *iconList[5];			//icons to be used

  //methods
  void LoseALife ();			//override default rToken behaviour
  void MoveMe (int prx = 0, int pry = 0);
  void SetPowerLevel (int i);
  int GetPowerLevel ();
  void GotASheep (int n = 1);
  int GetSheepInTow ();
  void SetSheepInTow (int n = 0);

protected:
  int powerLevel;				//what are my bullets like?
  int sheepInTow;				//how many have I picked up?
  int timeInvulnerable;			//how many ticks will I be unkillable

};

//the bullets for killing things:
class rBullet:public rToken
{
public:
  rBullet ();
  ~rBullet ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);
  void SetMoveDir (int dir);

  //data
  wxIcon *iconList[3];

  //accessors
  void SetTimeToLive (int time);
  int GetTimeToLive ();

private:
  int timeToLive;				//decremented every move:dies if <1
  int moveDir;					//based on numberpad
  int xmove, ymove;				//how to move-set by SetMoveDir
};

//define the bonuses
class rBonus:public rToken
{
//only one type, but it can have different icons-they all move the same
//so its pointless to create a class for each one
public:

  //constructors
  rBonus ();
  rBonus (rBonusType type = LIFE);
  ~rBonus ();

  //accessors
  void SetType (rBonusType type);
  rBonusType GetType ();
  void SetTimeToLive (int time);
  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[2];			//only ever two icons-set by constructor AND SetType

private:
  //methods
  void SetIcons (rBonusType type);
  //data
  rBonusType myType;
  int xMove, yMove;
  int timeToLive;				//how many clicks do I last?

};

//define the bad guys for level 1
class bg1:public rToken
{
public:
  //constructors
  bg1 ();
  ~bg1 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[3];

private:

};

class bg2:public rToken
{
public:
  //constructors
  bg2 ();
  ~bg2 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[2];

private:
};

class bg3:public rToken
{
public:
  //constructors
  bg3 ();
  ~bg3 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[5];

private:
    bool reborn;
};

//bg4 doesn't exist because level 4 is a combination of bg1-3

class bg5:public rToken
{
public:
  //constructors
  bg5 ();
  ~bg5 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[4];

private:
    bool reborn;
  int xM, yM;					//so the second stage can keep a consistent move
};

class bg6:public rToken
{
public:
  //constructors
  bg6 ();
  ~bg6 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[4];

private:
    bool reborn;
  int xM, yM;					//so the second stage can keep a consistent move
};

//No bg7 because level 7 made up with bg5 & bg6

//bg8
class bg8:public rToken
{
public:
  //constructors
  bg8 ();
  ~bg8 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[5];

private:
    bool reborn;
  int xM, yM;					//so the second stage can keep a consistent move
  int numTicksPerMove;			//so can move at different speeds
};
//bg9 - same icons as 8
class bg9:public rToken
{
public:
  //constructors
  bg9 ();
  ~bg9 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[5];

private:
    bool reborn;
  int xM, yM;					//so the second stage can keep a consistent move
  int numTicksPerMove;			//so can move at different speeds
};
//bg10
class bg10:public rToken
{
public:
  //constructors
  bg10 ();
  ~bg10 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[3];

private:
  int xM, yM;					//so can keep a consistent move
  int numTicksPerMove;			//so can move at different speeds
};
//bg11
//Explodeslike a nova
class bg11:public rToken
{
public:
  //constructors
  bg11 ();
  ~bg11 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[6];

private:
    bool reborn;
  int xM, yM;					//so the second stage can keep a consistent move
  int afterLife;				//how many moves after first death
};

//bg12 splits in two directions fires left or right
class bg12:public rToken
{
public:
  //constructors
  bg12 ();
  ~bg12 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[5];

private:
    bool reborn;
  int xM;
  int numTicksPerMove;			//so can move at different speeds
};

//bg13 same as bg12 but up/down
class bg13:public rToken
{
public:
  //constructors
  bg13 ();
  ~bg13 ();

  //methods
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[5];

private:
    bool reborn;
  int yM;
  int numTicksPerMove;			//so can move at different speeds
};

//bg15
//moves slowly in on stage towards rabbit.
//in other stage moves quickly in a random way
//swaps stages after n moves
class bg15:public rToken
{
public:
  //constructors
  bg15 ();
  ~bg15 ();

  //methoda
  void MoveMe (int prx = 0, int pry = 0);

  //data
  wxIcon *iconList[3];

private:
  int type;						//used for the state
  int typeCount;				//used to change types
  int xM, yM;
  int numTicksPerMove;
};
#endif
