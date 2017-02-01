#ifndef RGAMES
#define RGAMES					//standard to stop multiple includes

#include "creatures.h"
#include "wx/gdicmn.h"			//for wxPoint
#include "wx/icon.h"			//for wxIcon

#define NUMBULLETS 50
#define MAXBADGUYS 60			//maximum number of enemies in a level
#define MAXBONUSES 5			//maximum number of bonuses in a level
#define LASTLEVEL 16			//change this when you add a level!

enum gState						//used to tell the game what it's doing
{
  SETUP = 100,					//ARBITRARY NUMBER
  WAITING,
  PAUSED,
  READYTOPLAY,
  PLAY,
  LEVELCOMPLETE,
  RABBITKILLED,
  GAMEOVER,
};
class Game
{
public:

//constructors-only ever one!
  Game ();
  ~Game ();						//must delete all "new" variables

//Game access methods
  gState GetState () const;
  void SetState (gState state);

  int GetLevel () const;
  void SetLevel (int level);
  bool GetLevelComplete ();
  void SetLevelComplete (bool i);
  bool GetRabbitKilled ();
  void SetRabbitKilled (bool i);

  int GetScore () const;
  void SetScore (int i);
  void AddToScore (int i);

//technically the rabbit chooses which direction to fire,
//but as the game does the work, we might as well leave it at this level
  int GetFireDir ();
  void SetFireDir (int dir);
  void CheckHighScores ();		//puts you in if you get there

  //Sheep Accessor
  wxPoint SheepPos (int i = 0);	//i to decide which one!
  wxIcon *SheepIcon (int i = 0);
  rState SheepState (int i = 0);

  //Bonus Accessor
  wxPoint BonusPos (int i = 0);
  wxIcon *BonusIcon (int i = 0);
  rState BonusState (int i = 0);
  rBonusType BonusType (int i = 0);
  int GetNumBonuses ();

  //BadGuy accessor
  wxPoint BadGuyPos (int i = 0);
  wxIcon *BadGuyIcon (int i = 0);
  rState BadGuyState (int i = 0);
  int GetNumBadGuys ();

  //Rabbit accessor
  int GetRabbitLives ();
  wxPoint RabbitPos ();
  wxIcon *RabbitIcon ();
  rState RabbitState ();
  void SetRabbitMove (wxPoint point);
  int GetRabbitSheepInTow ();
  void SetRabbitSheepInTow (int n = 0);

  //bullet accessors
  wxPoint BulletPos (int i = 0);
  wxIcon *BulletIcon (int i = 0);
  rState BulletState (int i = 0);

//Game methods to DO things
  void NextMove ();
  void SetUpLevel ();			//sets up myLevel ready to play, then sets state to play
  void DeleteLevel ();			//deletes all the created stuff to recover memory
  bool Overlap (int x1, int y1, wxIcon & icon1, int x2, int y2,
				wxIcon & icon2);
  //tests to see if icons overlap-ie for 'hits'
  void CheckCollisions ();
  //test to see if level has been completed
  bool IsLevelFinished ();
  //find a free bullet
  int FindBullet ();
  //create the new bullets as needed
  void FireNewBullets ();

  void PrintThis (int n);		//for debugging

private:

  int myScore;					//pretty obvious really
  int myLevel;					//how far have we got?
  int fireDirection;			//which way do I shoot?
  int fireNow;					//how often do I shoot
  bool rabbitKilled;			//to let MyApp know when the rabbit got hit
  bool levelComplete;			//to tell MyApp when level is completed
  gState myState;				//what am i doing at the moment?
  rSheep *pSheep;				//pointer to sheep
  rRabbit *pRabbit;				//points to the rabbit
  rBullet *pBullet;				//points to bullets
  rToken *pBadGuys[MAXBADGUYS];	//array of pointers to bad guys
  int numBadGuys;				//how many enemy are there this level?
  rBonus *pBonus[MAXBONUSES];	//array of pointers to bonuses
  int numBonuses;
};
#endif
