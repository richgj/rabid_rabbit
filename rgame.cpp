#include "rgame.h"
#include "creatures.h"
#include "wx/gdicmn.h"			//for wxPoint
#include "wx/msgdlg.h"			//for wxMessageBox
#include "wx/textfile.h"		//for text file i/o
#include "wx/textdlg.h"			//for text input
#include "wx/app.h"				//for wxExit
#include "wx/string.h"			// for ascii - wxString conversion

//default constructor
Game::Game ()
{
  myScore = 0;
  myLevel = 1;
  myState = WAITING;
  levelComplete = FALSE;
  rabbitKilled = FALSE;
  fireDirection = 5;			//none-bullet stays put
  fireNow = 1;					//increment each tick, to decide how often we fire
  try
  {
	pSheep = new rSheep[5];		//numbered 0-4!
	pBullet = new rBullet[NUMBULLETS];	//numbered 0-(NUMBULLETS-1)
	pRabbit = new rRabbit;
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }

  numBonuses = 0;
  numBadGuys = 0;
//pRabbit->SetPowerLevel(4);
  //don't set up the BadGuys/Bonuses here-they are created new for each level
  //set =NULL so don't point anywhere dangerous!
  for (int i = 0; i < MAXBADGUYS; i++)
	{
	  pBadGuys[i] = NULL;
	};
  for (int i = 0; i < MAXBONUSES; i++)
	{
	  pBonus[i] = NULL;
	};
};

//destructor
Game::~Game ()
{
  //remove all stuff created new
  delete[]pSheep;
  delete pRabbit;
  delete[]pBullet;
  //BadGuys/Bonus should be deleted before getting here-in DeleteLevel()
};

int
Game::GetLevel () const
{
  return myLevel;
};

void
Game::SetLevel (int level)
{
  myLevel = level;
};

void
Game::SetLevelComplete (bool i)
{
  levelComplete = i;
};

bool Game::GetLevelComplete ()
{
  return levelComplete;
};

gState Game::GetState () const
{
  return myState;
};

void
Game::SetState (gState state)
{
  myState = state;
};

int
Game::GetScore () const
{
  return myScore;
};

void
Game::SetScore (int i)
{
  myScore = i;
};

void
Game::AddToScore (int i)
{
  myScore += i;
};
int
Game::GetFireDir ()
{
  return fireDirection;
};
void
Game::SetFireDir (int dir)
{
  fireDirection = dir;
};

//stuff so the top level can draw the icons:
//Sheep:
wxPoint Game::SheepPos (int i)
{
  return wxPoint ((pSheep + i)->x, (pSheep + i)->y);
};

wxIcon *
Game::SheepIcon (int i)
{
  return (pSheep + i)->myIcon;
};

rState Game::SheepState (int i)
{
  return (pSheep + i)->GetState ();
};

//Bonuses
wxPoint Game::BonusPos (int i)
{
  return wxPoint (pBonus[i]->x, pBonus[i]->y);
};

wxIcon *
Game::BonusIcon (int i)
{
  return pBonus[i]->myIcon;
};

rState Game::BonusState (int i)
{
  return pBonus[i]->GetState ();
};

int
Game::GetNumBonuses ()
{
  return numBonuses;
};

rBonusType Game::BonusType (int i)
{
  return pBonus[i]->GetType ();
};

//BadGuy
wxPoint Game::BadGuyPos (int i)
{
  return wxPoint (pBadGuys[i]->x, pBadGuys[i]->y);
};

wxIcon *
Game::BadGuyIcon (int i)
{
  return pBadGuys[i]->myIcon;
};

rState Game::BadGuyState (int i)
{
  return pBadGuys[i]->GetState ();
};

int
Game::GetNumBadGuys ()
{
  return numBadGuys;
};

//bullet:
wxPoint Game::BulletPos (int i)
{
  return wxPoint ((pBullet + i)->x, (pBullet + i)->y);
};

wxIcon *
Game::BulletIcon (int i)
{
  return (pBullet + i)->myIcon;
};

rState Game::BulletState (int i)
{
  return (pBullet + i)->GetState ();
};

//rabbit
int
Game::GetRabbitLives ()
{
  return pRabbit->GetLives ();
};

wxPoint Game::RabbitPos ()
{
  return wxPoint (pRabbit->x, pRabbit->y);
};

wxIcon *
Game::RabbitIcon ()
{
  return pRabbit->myIcon;
};

rState Game::RabbitState ()
{
  return pRabbit->GetState ();
};

void
Game::SetRabbitMove (wxPoint point)
{
  pRabbit->xMove = point.x;
  pRabbit->yMove = point.y;
};

bool Game::GetRabbitKilled ()
{
  return rabbitKilled;
};

void
Game::SetRabbitKilled (bool i)
{
  rabbitKilled = i;
};

void
Game::SetRabbitSheepInTow (int n)
{
  pRabbit->SetSheepInTow (n);
};

int
Game::GetRabbitSheepInTow ()
{
  return pRabbit->GetSheepInTow ();
};

//now do the game play:
void
Game::NextMove ()
{
  int i;						//for loops

  //move all the sheep
  rSheep *ps;					//create a pointer to access the five sheep
  for (i = 0, ps = pSheep; i < 5; i++, ps++)
	{
	  ps->MoveMe ();			//move the sheep
	};

  //move the Bonuses
  for (i = 0; i < numBonuses; i++)
	{
	  pBonus[i]->MoveMe ();
	};

  //move the rabbit
  pRabbit->MoveMe ();

  //Move the bullets
  rBullet *pb;					//pointer to access the bullets
  for (i = 0, pb = pBullet; i < NUMBULLETS; i++, pb++)
	{
	  pb->MoveMe ();
	};

  //Fire new bullets
  fireNow++;
  if ((fireNow >= 10) && (fireDirection != 5))	//if =5, haven't fired yet
	{
	  fireNow = 1;
	  FireNewBullets ();		// a rather messy subroutine!
	};

  //move the bad guys
  for (int i = 0; i < numBadGuys; i++)
	{
	  pBadGuys[i]->MoveMe (pRabbit->x, pRabbit->y);	//the BGs know where the rabbit is...
	};
  //check for hits and change the states as needed
  CheckCollisions ();

  //check for bonus if all sheep caught
  if ((pRabbit->GetSheepInTow () == 5) && (pBonus[0]->GetState () == UNBORN))
	{
	  pBonus[0]->SetState (ALIVE);
	};

  //check to see if level is completed
  levelComplete = IsLevelFinished ();

};

bool Game::IsLevelFinished ()
{
//NOT if there is a sheep alive (not dead and not captured)
  rSheep *
	ps;
  ps = pSheep;
  for (int i = 0; i < 5; i++, ps++)
	{
	  if (ps->GetState () == ALIVE)
		{
		  return FALSE;
		};
	};
//NOT if any badguys are alive or dying
  for (int i = 0; i < numBadGuys; i++)
	{
	  if (pBadGuys[i]->GetState () != DEAD)
		{
		  return FALSE;
		};
	};
//NOT if any power ups are still available
  for (int i = 0; i < numBonuses; i++)
	{
	  if (pBonus[i]->GetState () == ALIVE)
		{
		  return FALSE;
		};
	};
//if it gets this far, everything is dead
  return TRUE;
};

bool
  Game::Overlap (int x1, int y1, wxIcon & icon1, int x2, int y2,
				 wxIcon & icon2)
{
//Check to see if the circle filling the icons are overlapping
//only works if the icons are square
//do the maths if you don't believe it
  int x, y, d, e;
  d = icon1.GetWidth ();
  e = icon2.GetWidth ();
  x = (x2 + e / 2) - (x1 + d / 2);
  y = (y2 + e / 2) - (y1 + d / 2);
  if (4 * (x * x + y * y) < ((d + e) * (d + e)))
	{
	  return TRUE;
	}
  else
	{
	  return FALSE;
	}
};

//this checks for Hits and changes the states as required
void
Game::CheckCollisions ()
{
//first see if the Rabbit has picked up any Sheep
  rSheep *pointer = pSheep;
  for (int i = 0; i < 5; i++, pointer++)
	{
	  if (pointer->GetState () == ALIVE)	//ie not already captured or killed!
		{
		  if (Overlap
			  (pRabbit->x, pRabbit->y, *(pRabbit->myIcon), pointer->x,
			   pointer->y, *(pointer->myIcon)))
			{
			  pRabbit->GotASheep ();
			  AddToScore (10);
			  pointer->SetState (CAPTURED);
			};
		};
	};
//see if the bullets hit anything
  rBullet *pb;					//to point to the bullets
  for (int i = 0; i < numBadGuys; i++)
	{
	  if (BadGuyState (i) == ALIVE)
		{
		  pb = pBullet;			//point to the first bullet
		  for (int j = 0; j < NUMBULLETS; j++, pb++)
			{
			  if (BulletState (j) == ALIVE)
				{
				  if (Overlap
					  (pBadGuys[i]->x, pBadGuys[i]->y, *(pBadGuys[i]->myIcon),
					   pb->x, pb->y, *(pb->myIcon)))
					{
					  //a live bullet has hit a live bad guy
					  pb->LoseALife ();
					  pBadGuys[i]->LoseALife ();
					  AddToScore (5);
					};
				};
			};
		};
	};
//see if the Rabbit gets a power up
  for (int i = 0; i < numBonuses; i++)
	{
	  if (BonusState (i) == ALIVE)
		{
		  if (Overlap
			  (pRabbit->x, pRabbit->y, *(pRabbit->myIcon), pBonus[i]->x,
			   pBonus[i]->y, *(pBonus[i]->myIcon)))
			{
			  //got one!
			  pBonus[i]->LoseALife ();
			  switch (BonusType (i))
				{
				case LIFE:
				  pRabbit->SetLives (pRabbit->GetLives () + 1);
				  break;
				case BOMB:
				  for (int i = 0; i < numBadGuys; i++)
					{
					  pBadGuys[i]->SetState (DEAD);
					};
				  AddToScore (200);
				  break;
				case BONUSSCORE:
				  AddToScore (30);
				  break;
				case UNKILLABLE:
				  pRabbit->SetState (INVULNERABLE);
				  break;
				case POWERUP:
				  pRabbit->SetPowerLevel (pRabbit->GetPowerLevel () + 1);
				  break;
				default:
				  break;
				};
			};
		};
	};
//see if the rabbit hits a bad guy
  if (pRabbit->GetState () == ALIVE)
	{
	  for (int i = 0; i < numBadGuys; i++)
		{
		  if (BadGuyState (i) == ALIVE)
			{
			  if (Overlap
				  (pRabbit->x, pRabbit->y, *(pRabbit->myIcon), pBadGuys[i]->x,
				   pBadGuys[i]->y, *(pBadGuys[i]->myIcon)))
				{				//Rabbit has hit a living bad guy
				  pRabbit->LoseALife ();
				  AddToScore (-50);	//lose some points
				  rabbitKilled = TRUE;
				  if (pRabbit->GetPowerLevel () > 1)
					{
					  pRabbit->SetPowerLevel (pRabbit->GetPowerLevel () - 1);	//drop a level
					};
				}
			};
		};
	};
};

//search for a bullet not in use
int
Game::FindBullet ()
{
  int bul = 0;
  while (((pBullet + bul)->GetState () != DEAD) && (bul < NUMBULLETS))
	{
	  bul++;
	};
  if (bul < NUMBULLETS)
	{
	  return bul;
	}
  else
	{
	  return -1;
	};
};

void
Game::PrintThis (int n)
{
  wxString c;

  c.Printf (wxT ("N= %i"), n);

  wxMessageBox (c, wxT ("Numeric Data:"));
};

void
Game::FireNewBullets ()
{
  int newBul;					//to create bullets as required
  int fd1, fd2, fd3;			//for additional bullets
  rBullet *pb;					//pointer to bullets

  switch (pRabbit->GetPowerLevel ())
	{
	case 1:					//bottom level bullet
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fireDirection);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[0];	//normal bullet
		  pb->SetLives (1);
		};
	  break;
	case 2:					//next level power
	  //first do as case 1(the middle bullet)
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fireDirection);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[0];	//normal bullet
		  pb->SetLives (1);
		};
	  //second, find the two other fire dirs(up & down)
	  switch (fireDirection)
		{
		case 1:
		  fd1 = 2;
		  fd2 = 4;
		  break;
		case 2:
		  fd1 = 1;
		  fd2 = 3;
		  break;
		case 3:
		  fd1 = 2;
		  fd2 = 6;
		  break;
		case 4:
		  fd1 = 1;
		  fd2 = 7;
		  break;
		case 5:				//shouldn't get here
		  break;
		case 6:
		  fd1 = 3;
		  fd2 = 9;
		  break;
		case 7:
		  fd1 = 4;
		  fd2 = 8;
		  break;
		case 8:
		  fd1 = 7;
		  fd2 = 9;
		  break;
		case 9:
		  fd1 = 6;
		  fd2 = 8;
		  break;
		};
	  //now create bullets as above for these two directions
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd1);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[0];	//normal bullet
		  pb->SetLives (1);
		};
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd2);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[0];	//normal bullet
		  pb->SetLives (1);
		};
	  break;
	case 3:					//as 2 but with move lives/bullet and new icon
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fireDirection);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[1];
		  pb->SetLives (2);
		};
	  //second, find the two other fire dirs(up & down)
	  switch (fireDirection)
		{
		case 1:
		  fd1 = 2;
		  fd2 = 4;
		  break;
		case 2:
		  fd1 = 1;
		  fd2 = 3;
		  break;
		case 3:
		  fd1 = 2;
		  fd2 = 6;
		  break;
		case 4:
		  fd1 = 1;
		  fd2 = 7;
		  break;
		case 5:				//shouldn't get here
		  break;
		case 6:
		  fd1 = 3;
		  fd2 = 9;
		  break;
		case 7:
		  fd1 = 4;
		  fd2 = 8;
		  break;
		case 8:
		  fd1 = 7;
		  fd2 = 9;
		  break;
		case 9:
		  fd1 = 6;
		  fd2 = 8;
		  break;
		};
	  //now create bullets as above for these two directions
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd1);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[1];	//normal bullet
		  pb->SetLives (2);
		};
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd2);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (70);	//how long do I last?
		  pb->myIcon = pb->iconList[1];	//normal bullet
		  pb->SetLives (2);
		};
	  break;
	case 0:
	  wxMessageBox (wxT ("FireNow"), wxT ("Problem in:Fire Bullet"));
	  //shouldn't ever get here
	  break;
	default:					//Any level >3
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fireDirection);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (50);	//how long do I last?
		  pb->myIcon = pb->iconList[2];
		  pb->SetLives (5);
		};
	  //second, find the two other fire dirs(up & down)
	  switch (fireDirection)
		{
		case 1:
		  fd1 = 2;
		  fd2 = 4;
		  break;
		case 2:
		  fd1 = 1;
		  fd2 = 3;
		  break;
		case 3:
		  fd1 = 2;
		  fd2 = 6;
		  break;
		case 4:
		  fd1 = 1;
		  fd2 = 7;
		  break;
		case 5:				//shouldn't get here
		  break;
		case 6:
		  fd1 = 3;
		  fd2 = 9;
		  break;
		case 7:
		  fd1 = 4;
		  fd2 = 8;
		  break;
		case 8:
		  fd1 = 7;
		  fd2 = 9;
		  break;
		case 9:
		  fd1 = 6;
		  fd2 = 8;
		  break;
		};
	  //now create bullets as above for these two directions
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd1);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (50);	//how long do I last?
		  pb->myIcon = pb->iconList[2];	//normal bullet
		  pb->SetLives (5);
		};
	  newBul = FindBullet ();
	  if (newBul >= 0)
		{
		  pb = (pBullet + newBul);	//point to the free bullet
		  pb->SetState (ALIVE);	//resurrect it
		  pb->SetMoveDir (fd2);	//set its direction
		  pb->x = pRabbit->x + 8;
		  pb->y = pRabbit->y + 8;	//middle of the rabbit
		  pb->SetTimeToLive (50);	//how long do I last?
		  pb->myIcon = pb->iconList[2];	//normal bullet
		  pb->SetLives (5);
		};
	  break;
	};
};

void
Game::CheckHighScores ()
{
  wxTextFile *fScores;
  wxTextEntryDialog *textIn;
  wxString tmp, theName[10];
  long place, theScore[10];

  //open the file
  fScores = new wxTextFile (wxT ("ScoreS"));
  if (NULL == fScores)
	{
	  wxMessageBox (wxT ("Error Allocating Memory"));
	  wxExit ();
	}
  fScores->Open ();
  //get the data
  theName[0] = fScores->GetFirstLine ();
  tmp = fScores->GetNextLine ();
  tmp.ToLong (&theScore[0]);
  for (int i = 1; i < 10; i++)
	{
	  theName[i] = fScores->GetNextLine ();
	  tmp = fScores->GetNextLine ();
	  tmp.ToLong (&theScore[i]);
	};
  //does it fit in?
  place = 0;
  while ((theScore[place] > myScore) && (place < 10))
	{
	  place++;
	};
  if (place < 10)				//it needs inserting
	{
	  for (int i = 9; i > place; i--)
		{
		  theScore[i] = theScore[i - 1];
		  theName[i] = theName[i - 1];
		};

	  //new name here!!!
	  try
	  {
		textIn = new wxTextEntryDialog (0,
										wxT ("Tell me your name"),
										wxT ("You got a high score!"));
	  }
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }

	  if (textIn->ShowModal () == wxID_OK)
		{
		  theName[place] = textIn->GetValue ();
		}
	  else						//stick in a default name
		{
		  theName[place] = wxT ("Rabbit");
		};
	  delete textIn;			//recover the memory
	  theScore[place] = myScore;
	  //put scores back into the text file
	  fScores->GoToLine (0);
	  for (int i = 0; i < 20; i++)
		{
		  fScores->RemoveLine (0);
		};
	  for (int i = 0; i < 10; i++)
		{
		  fScores->AddLine (theName[i]);
		  tmp.Printf (wxT ("%i"), theScore[i]);
		  fScores->AddLine (tmp);
		};
	  //write into file
	  fScores->Write ();
	};
  //close the file and return memory
  fScores->Close ();
  delete fScores;
};

void
Game::DeleteLevel ()
{
//this section recovers memory from the heap by
//deleting the bad guys from the current level
//wxMessageBox("Got to DeleteLevel","");
  for (int i = 0; i < MAXBADGUYS; i++)
	{
	  if (pBadGuys != NULL)
		{
		  delete pBadGuys[i];
		  pBadGuys[i] = NULL;
		};
	};
//wxMessageBox("BadGuys Deleted","");
//deleting the bonuses from the heap
  for (int i = 0; i < MAXBONUSES; i++)
	{
	  if (pBonus[i] != NULL)
		{
		  delete pBonus[i];
		  pBonus[i] = NULL;
		};
	};
//wxMessageBox("Bonuses deleted","");
};

void
Game::SetUpLevel ()
{
//this will be a HUGE method, as it needs a bit for each level played (in the switch)
//each level must set up the sheep, baddies, powerups and the rabbit
//Choose a position for the rabbit. Don't change anything else on the rabbit
//except its icon
  rSheep *ps;					//to point to sheep
  rRabbit *pr;					//to point to Rabbit
  rBullet *pb;					//to point to the Bullets

//make sure the starting point is the same each time a level is created
  srand (myLevel);

  //all bullets must be killed
  pb = pBullet;
  for (int i = 0; i < NUMBULLETS; i++, pb++)
	{
	  pb->SetState (DEAD);
	};

//all sheep must be reset
  ps = pSheep;
  for (int i = 0; i < 5; i++, ps++)
	{
	  ps->x = ps->RandomNo (MAXX);
	  ps->y = ps->RandomNo (MAXY);
	  ps->SetState (ALIVE);
	  ps->SetLives (1);
	  ps->moveCounter = 1;
	  ps->myIcon = *(ps->iconList);	//one of the alive positions!
	};

//reset the rabbit (do the position in the switch statement)
  pr = pRabbit;
  pr->myIcon = *(pr->iconList);	//point to first icon in list
  SetRabbitSheepInTow (0);
  SetRabbitMove (wxPoint (0, 0));

//stop firing bullets
  SetFireDir (5);

//reset the "end of level" messages
  rabbitKilled = FALSE;
  levelComplete = FALSE;

//now set up the bad guys and position the Rabbit
  switch (myLevel)
	{
	case 1:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 50;
	  numBadGuys = 40;			//in this level!
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg1;
		  };
		numBonuses = 2;
		pBonus[0] = new rBonus (POWERUP);	//only appears when sheep are caugt
		pBonus[1] = new rBonus (LIFE);
		pBonus[1]->SetState (ALIVE);	//so we can see it
		pBonus[1]->SetTimeToLive (800);	//ticks to last
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }

	  break;
	case 2:
	  pr = pRabbit;
	  pr->x = 5;
	  pr->y = 5;
	  numBadGuys = 50;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg2;
		  };
		numBonuses = 1;
		pBonus[0] = new rBonus (BONUSSCORE);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 3:
	  pr = pRabbit;
	  pr->x = 20;
	  pr->y = 20;
	  numBadGuys = 45;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg3;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (LIFE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (800);
		pBonus[1]->x = 50;
		pBonus[1]->y = 200;		//to give rabbit a chance to reach it if on low power
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 4:
	  pr = pRabbit;
	  pr->x = 200;
	  pr->y = 200;
	  numBadGuys = 50;
	  try
	  {
		for (int i = 0; i < 20; i++)
		  {
			pBadGuys[i] = new bg1;
		  };
		for (int i = 20; i < 30; i++)
		  {
			pBadGuys[i] = new bg2;
		  };
		for (int i = 30; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg3;
		  };
		numBonuses = 3;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (UNKILLABLE);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (800);
		pBonus[1]->x = 200;
		pBonus[1]->y = 220;		//to gve the rabbit a chance to get to it
		pBonus[2] = new rBonus (POWERUP);
		pBonus[2]->SetState (ALIVE);
		pBonus[2]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 5:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 380;
	  numBadGuys = 40;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg5;
		  };
		numBonuses = 3;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (POWERUP);
		pBonus[1] = new rBonus (LIFE);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (800);
		pBonus[2] = new rBonus (BONUSSCORE);
		pBonus[2]->SetState (ALIVE);
		pBonus[2]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 6:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 380;
	  numBadGuys = 40;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg6;
		  };
		numBonuses = 3;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[2] = new rBonus (LIFE);
		pBonus[2]->SetTimeToLive (1500);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 7:
	  pr = pRabbit;
	  pr->x = 20;
	  pr->y = 380;
	  numBadGuys = 60;
	  try
	  {
		for (int i = 0; i < 30; i++)
		  {
			pBadGuys[i] = new bg5;
		  };
		for (int i = 30; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg6;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (LIFE);
		pBonus[1] = new rBonus (BOMB);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (800);
		pBonus[1]->x = 50;
		pBonus[1]->y = 150;
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 8:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 200;
	  numBadGuys = 40;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg8;
		  };
		numBonuses = 4;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[2] = new rBonus (POWERUP);
		pBonus[2]->SetState (ALIVE);
		pBonus[2]->SetTimeToLive (1000);
		pBonus[2]->x = 250;
		pBonus[2]->y = 250;
		pBonus[3] = new rBonus (LIFE);
		pBonus[3]->SetState (ALIVE);
		pBonus[3]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 9:
	  pr = pRabbit;
	  pr->x = 10;
	  pr->y = 10;
	  numBadGuys = 35;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg9;
		  };
		numBonuses = 4;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (BOMB);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[2] = new rBonus (POWERUP);
		pBonus[2]->SetState (ALIVE);
		pBonus[2]->SetTimeToLive (1000);
		pBonus[2]->x = 150;
		pBonus[2]->y = 50;
		pBonus[3] = new rBonus (LIFE);
		pBonus[3]->SetState (ALIVE);
		pBonus[3]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 10:
	  pr = pRabbit;
	  pr->x = 10;
	  pr->y = 150;
	  numBadGuys = 40;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg10;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (POWERUP);
		pBonus[1] = new rBonus (UNKILLABLE);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[1]->x = 200;
		pBonus[1]->y = 200;
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 11:
	  pr = pRabbit;
	  pr->x = 200;
	  pr->y = 200;
	  numBadGuys = 40;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg11;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (UNKILLABLE);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[1]->x = 250;
		pBonus[1]->y = 250;
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 12:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 200;
	  numBadGuys = 20;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg12;
		  };
		numBonuses = 4;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[1]->x = 250;
		pBonus[1]->y = 300;
		pBonus[2] = new rBonus (BOMB);
		pBonus[2]->SetTimeToLive (1300);
		pBonus[2]->SetState (ALIVE);
		pBonus[3] = new rBonus (UNKILLABLE);
		pBonus[3]->SetTimeToLive (1100);
		pBonus[3]->SetState (ALIVE);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 13:
	  pr = pRabbit;
	  pr->x = 380;
	  pr->y = 200;
	  numBadGuys = 20;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg13;
		  };
		numBonuses = 3;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[1]->x = 250;
		pBonus[1]->y = 300;
		pBonus[2] = new rBonus (BOMB);
		pBonus[2]->SetTimeToLive (1400);
		pBonus[2]->SetState (ALIVE);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 14:
	  pr = pRabbit;
	  pr->x = 10;
	  pr->y = 10;
	  numBadGuys = 30;
	  try
	  {
		for (int i = 0; i < 15; i++)
		  {
			pBadGuys[i] = new bg12;
		  };
		for (int i = 15; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg13;
		  };
		numBonuses = 3;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
		pBonus[1]->x = 250;
		pBonus[1]->y = 300;
		pBonus[2] = new rBonus (BOMB);
		pBonus[2]->SetTimeToLive (2000);
		pBonus[2]->SetState (ALIVE);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 15:
	  pr = pRabbit;
	  pr->x = 10;
	  pr->y = 10;
	  numBadGuys = 45;
	  try
	  {
		for (int i = 0; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg15;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	case 16:
	  pr = pRabbit;
	  pr->x = 384;
	  pr->y = 384;
	  numBadGuys = 45;
	  try
	  {
		for (int i = 0; i < 10; i++)
		  {
			pBadGuys[i] = new bg9;
		  };
		for (int i = 10; i < 20; i++)
		  {
			pBadGuys[i] = new bg12;
		  };
		for (int i = 20; i < 30; i++)
		  {
			pBadGuys[i] = new bg13;
		  };
		for (int i = 30; i < numBadGuys; i++)
		  {
			pBadGuys[i] = new bg15;
		  };
		numBonuses = 2;			//pBonus[0] MUST exist as the bonus if all sheep are caught
		pBonus[0] = new rBonus (BONUSSCORE);
		pBonus[1] = new rBonus (POWERUP);
		pBonus[1]->SetState (ALIVE);
		pBonus[1]->SetTimeToLive (1000);
	  }							//end of try
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Error Allocating Memory"));
		wxExit ();
	  }
	  break;
	default:
	  //use this as a template to start
//              pr=pRabbit;
//              pr->x=200;
//              pr->y=20;
//              numBadGuys=0;
//              numBonuses=1;//pBonus[0] MUST exist as the bonus if all sheep are caught
//              pBonus[0]=new rBonus(BONUSSCORE);
	  break;
	};
};
