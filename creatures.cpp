#include "wx/icon.h"
#include "wx/string.h"
#include "creatures.h"
#include "wx/msgdlg.h"

//default constructor for base class
rToken::rToken ()
{
  //set all variables
  x = 0;
  y = 0;
  myState = ALIVE;
  myIcon = 0;
  lives = 1;
};

//destructor for base class (virtual)
rToken::~rToken ()
{
  //nothing to do
};

//function only created to be hidden (virtual)
void
rToken::MoveMe (int prx, int pry)
{
//don't bother to do anything
};

//Accessors
void
rToken::SetState (rState state)
{
  myState = state;
};

rState rToken::GetState ()
{
  return myState;
};

void
rToken::SetLives (int i)
{
  lives = i;
};

int
rToken::GetLives ()
{
  return lives;
};

//The token has been hit.
void
rToken::LoseALife ()
{
  lives -= 1;
  if (lives == 0)
	{
	  myState = DYING;
	};
  if (lives < -1)				//0 and -1 are Dying, so it gets drawn once
	{
	  myState = DEAD;
	};
};

int
rToken::RandomNo (float max)
{
  return (int) (max * (float) rand () / (float) RAND_MAX);
};


//the Sheep class
//constructor
rSheep::rSheep ()
{
  x = RandomNo (MAXX);
  y = RandomNo (MAXY);
  iconList[0] = new wxIcon (wxT ("icons/rsheep.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lsheep.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/dsheep.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = 1;				//incremented every tick
};

//sheep destructor
rSheep::~rSheep ()
{
  //get rid of all stuff created new
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
};

//Sheep move
void
rSheep::MoveMe (int prx, int pry)
{
  //sheep move randomly
  if (moveCounter >= 5)			//every fifth tick
	{
	  switch (myState)
		{
		case ALIVE:
		  x += RandomNo (5) - 2;
		  y += RandomNo (5) - 2;
		  if (x > MAXX)
			{
			  x = MAXX;
			};
		  if (x < 0)
			{
			  x = 0;
			};
		  if (y > MAXY)
			{
			  y = MAXY;
			};
		  if (y < 0)
			{
			  y = 0;
			};
		  moveCounter = 1;
		  //change sheep direction?
		  if (RandomNo (10) > 4)
			{
			  myIcon = *iconList;
			}
		  else
			{
			  myIcon = *(iconList + 1);
			};
		  break;
		case CAPTURED:
		  //do nothing for now
		  break;
		case DYING:
		  myIcon = *(iconList + 2);
		  LoseALife ();
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//the Rabbit class
// Rabbit constructor
rRabbit::rRabbit ()
{
  x = RandomNo (MAXX);
  y = RandomNo (MAXY);
  iconList[0] = new wxIcon (wxT ("icons/rrabbit.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lrabbit.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/drabbit.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/urrabbit.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/ulrabbit.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = 1;				//incremented every tick
  lives = 3;
  powerLevel = 1;				//start at the bottom
  sheepInTow = 0;				//cos they all need capturing at this point
  xMove = 0;
  yMove = 0;					//start off by standing still!
  timeInvulnerable = 0;			//set when made invulnreable
};

//rabbit destructor
rRabbit::~rRabbit ()
{
  //remove all the "new" stuff
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};

//rabbit accessors
int
rRabbit::GetPowerLevel ()
{
  return powerLevel;
};
void
rRabbit::SetPowerLevel (int i)
{
  powerLevel = i;
};

//rabbit move
void
rRabbit::MoveMe (int prx, int pry)
{
  switch (myState)
	{
	case ALIVE:
	  x += xMove;
	  y += yMove;
	  if (x > MAXX)
		{
		  x = MAXX;
		  xMove = 0;
		  yMove = 0;
		};
	  if (x < 0)
		{
		  x = 0;
		  xMove = 0;
		  yMove = 0;
		};
	  if (y > MAXY)
		{
		  y = MAXY;
		  xMove = 0;
		  yMove = 0;
		};
	  if (y < 0)
		{
		  y = 0;
		  xMove = 0;
		  yMove = 0;
		};
	  //choose icon-leave alone if moving up and down
	  if (xMove > 0)
		{
		  myIcon = iconList[0];	//right facing
		}
	  else if (xMove < 0)
		{
		  myIcon = iconList[1];	//left facing
		};
	  break;
	case INVULNERABLE:
	  if (timeInvulnerable < 1)	//first time INVULNERABLE
		{
		  timeInvulnerable = 300;	//how long it lasts
		}
	  else
		{
		  timeInvulnerable--;
		  if (timeInvulnerable < 1)
			{
			  myState = ALIVE;
			};
		};
	  //the next bit is same as ALIVE but with different icons!
	  x += xMove;
	  y += yMove;
	  if (x > MAXX)
		{
		  x = MAXX;
		  xMove = 0;
		  yMove = 0;
		};
	  if (x < 0)
		{
		  x = 0;
		  xMove = 0;
		  yMove = 0;
		};
	  if (y > MAXY)
		{
		  y = MAXY;
		  xMove = 0;
		  yMove = 0;
		};
	  if (y < 0)
		{
		  y = 0;
		  xMove = 0;
		  yMove = 0;
		};
	  //choose icon-leave alone if moving up and down
	  if (xMove > 0)
		{
		  myIcon = iconList[3];	//right facing unkillable
		}
	  else if (xMove < 0)
		{
		  myIcon = iconList[4];	//left facing unkillable
		};
	  break;
	case DYING:
	  myIcon = iconList[2];		//dying rabbit
	default:
	  break;
	};
};

//overWrite the LoseALife default
void
rRabbit::LoseALife ()
{
  myIcon = iconList[2];			//dying rabbit
  lives -= 1;
  if (lives <= 0)
	{
	  myState = DYING;
	};
};

void
rRabbit::GotASheep (int n)
{
  sheepInTow += n;
};

int
rRabbit::GetSheepInTow ()
{
  return sheepInTow;
};

void
rRabbit::SetSheepInTow (int n)
{
  sheepInTow = n;
};

//Bullets get to do things too...
//Bullet Constructor
rBullet::rBullet ()
{
  x = 0;
  y = 0;						//just so it has a position
  myState = DEAD;				//set when made active
  moveDir = 5;					//see numpad for direction, 5=none
  lives = 1;
  iconList[0] = new wxIcon (wxT ("icons/bullet.xpm"), wxBITMAP_TYPE_XPM);	//normal bullet
  iconList[1] = new wxIcon (wxT ("icons/bulletplus.xpm"), wxBITMAP_TYPE_XPM);	//powered bullet
  iconList[2] = new wxIcon (wxT ("icons/bulletplus2.xpm"), wxBITMAP_TYPE_XPM);	//mega power bullet
  myIcon = *iconList;			//points to normal bullet
  timeToLive = 70;				//will move ?? pixels before dying(default)
};

//Bullet destructor
rBullet::~rBullet ()
{
//delete the icons
  delete *iconList;
  delete *(iconList + 1);
};

//Bullet accessors
void
rBullet::SetTimeToLive (int time)
{
  timeToLive = time;
};
int
rBullet::GetTimeToLive ()
{
  return timeToLive;
};

//Bullet methods
//Bullet moving:
void
rBullet::MoveMe (int prx, int pry)
{
  if (myState != DEAD)
	{
	  timeToLive -= 1;
	  if (timeToLive < 1)
		{
		  myState = DEAD;
		};
	  x += xmove;
	  y += ymove;
	  if ((x > 400) || (x < 0) || (y > 400) | (y < 0))	//it's moved out of the window
		{
		  myState = DEAD;
		};
	};
  if (myState == DYING)			//due to hitting something
	{
	  LoseALife ();
	};
};

//Bullet SetMoveDir
void
rBullet::SetMoveDir (int dir)
{
//based on standard number pad, with 5= don't move
  int m = 3;					//move in each direction
  switch (dir)
	{
	case 1:
	  xmove = -m;
	  ymove = m;
	  break;
	case 2:
	  xmove = 0;
	  ymove = m;
	  break;
	case 3:
	  xmove = m;
	  ymove = m;
	  break;
	case 4:
	  xmove = -m;
	  ymove = 0;
	  break;
	case 5:
	  xmove = 0;
	  ymove = 0;
	  break;
	case 6:
	  xmove = m;
	  ymove = 0;
	  break;
	case 7:
	  xmove = -m;
	  ymove = -m;
	  break;
	case 8:
	  xmove = 0;
	  ymove = -m;
	  break;
	case 9:
	  xmove = m;
	  ymove = -m;
	  break;
	default:
	  break;
	};
};

//Bonuses
rBonus::rBonus ()
{
//default constructor calls constructor with default type
  rBonus (rBonusType (BONUSSCORE));
};

rBonus::rBonus (rBonusType type)
{
  x = RandomNo (MAXX - 16);
  y = RandomNo (MAXY - 16);
  xMove = RandomNo (4) - 2;
  yMove = RandomNo (4) - 2;
  timeToLive = 32767;			//by default they last for ages
  myState = UNBORN;				//by default they don't show
  moveCounter = 1;
  myType = type;
  lives = 1;
  iconList[0] = new wxIcon;
  iconList[1] = new wxIcon;
  SetIcons (type);
};

rBonus::~rBonus ()
{
  delete *iconList;
  delete *(iconList + 1);
};

void
rBonus::SetTimeToLive (int time)
{
  timeToLive = time;
};

void
rBonus::SetIcons (rBonusType type)
{
  switch (type)
	{
	case LIFE:
	  *iconList[0] = wxIcon (wxT ("icons/heart1.xpm"), wxBITMAP_TYPE_XPM);
	  *iconList[1] = wxIcon (wxT ("icons/heart2.xpm"), wxBITMAP_TYPE_XPM);
	  break;
	case BOMB:
	  *iconList[0] = wxIcon (wxT ("icons/bomb1.xpm"), wxBITMAP_TYPE_XPM);
	  *iconList[1] = wxIcon (wxT ("icons/bomb2.xpm"), wxBITMAP_TYPE_XPM);
	  break;
	case POWERUP:
	  *iconList[0] = wxIcon (wxT ("icons/power1.xpm"), wxBITMAP_TYPE_XPM);
	  *iconList[1] = wxIcon (wxT ("icons/power2.xpm"), wxBITMAP_TYPE_XPM);
	  break;
	case UNKILLABLE:
	  *iconList[0] = wxIcon (wxT ("icons/unkill1.xpm"), wxBITMAP_TYPE_XPM);
	  *iconList[1] = wxIcon (wxT ("icons/unkill2.xpm"), wxBITMAP_TYPE_XPM);
	  break;
	case BONUSSCORE:
	  *iconList[0] = wxIcon (wxT ("icons/bonus1.xpm"), wxBITMAP_TYPE_XPM);
	  *iconList[1] = wxIcon (wxT ("icons/bonus2.xpm"), wxBITMAP_TYPE_XPM);
	  break;
	default:
	  break;
	};
  myIcon = *iconList;
};

rBonusType rBonus::GetType ()
{
  return myType;
};

void
rBonus::MoveMe (int prx, int pry)
{
  moveCounter++;
  if (moveCounter == 5)
	{
	  //switch icons
	  if (myIcon == *iconList)
		{
		  myIcon = *(iconList + 1);
		}
	  else
		{
		  myIcon = *iconList;
		}
	};
  //if it drifts off the edge, bounce back
  if (moveCounter >= 10)
	{
	  moveCounter = 1;
	  x += xMove;
	  if ((x < 0) || (x > MAXX - 16))
		{
		  xMove *= -1;
		};
	  y += yMove;
	  if ((y < 0) || (y > MAXY - 16))
		{
		  yMove *= -1;
		};
	};
  if (myState == DYING)
	{
	  LoseALife ();
	};
  if (myState == ALIVE)
	{
	  timeToLive--;
	  if (timeToLive <= 0)
		{
		  myState = DYING;
		};
	};
};

//BadGuys:
//bg1 stuff
bg1::bg1 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg1.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg1.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/dbg1.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
};

bg1::~bg1 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
}

void
bg1::MoveMe (int prx, int pry)
{
  int xM, yM;					//for the move
  // move randomly
  if (moveCounter >= 5)			//every fifth tick
	{
	  switch (myState)
		{
		case ALIVE:
		  //move toward the rabbit...
		  if (x < prx)
			{
			  xM = 1;
			}
		  else
			{
			  xM = -1;
			};
		  if (y < pry)
			{
			  yM = 1;
			}
		  else
			{
			  yM = -1;
			};
		  x += xM;
		  y += yM;
		  //don't need to check for out of bounds
		  //because moves towards rabbit
		  moveCounter = 1;
		  //change icon:
		  if (xM > 0)
			{
			  myIcon = *iconList;
			}
		  else
			{
			  myIcon = *(iconList + 1);
			};
		  break;
		case DYING:
		  myIcon = *(iconList + 2);
		  LoseALife ();
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg2:
bg2::bg2 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/bg2.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/dbg2.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  lives = 2;
};

bg2::~bg2 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
};

void
bg2::MoveMe (int prx, int pry)
{
  int xM, yM;					//for the move
  // move randomly
  if (moveCounter >= 5)			//every fifth tick
	{
	  switch (myState)
		{
		case ALIVE:
		  //move toward the rabbit...
		  if (x < prx)
			{
			  xM = 2;
			}
		  else
			{
			  xM = -2;
			};
		  if (y < pry)
			{
			  yM = 2;
			}
		  else
			{
			  yM = -2;
			};
		  x += xM;
		  y += yM;
		  //don't need to check for out of bounds
		  //because moves towards rabbit
		  moveCounter = 1;
		  //icon doesn't need changing
		  break;
		case DYING:
		  myIcon = *(iconList + 1);
		  LoseALife ();
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg3:
bg3::bg3 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg3a.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg3a.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/rbg3b.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/lbg3b.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/dbg3.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg3::~bg3 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};

void
bg3::MoveMe (int prx, int pry)
{
  int xM, yM;					//for the move
  // move randomly
  if (moveCounter >= 4)			//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  //move toward the rabbit...
		  if (x < prx)
			{
			  xM = 2;
			}
		  else
			{
			  xM = -2;
			};
		  if (y < pry)
			{
			  yM = 2;
			}
		  else
			{
			  yM = -2;
			};
		  x += xM;
		  y += yM;
		  //don't need to check for out of bounds
		  //because moves towards rabbit
		  moveCounter = 1;
		  if (reborn)
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList + 2);
				}
			  else
				{
				  myIcon = *(iconList + 3);
				};
			}
		  else
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList);
				}
			  else
				{
				  myIcon = *(iconList + 1);
				};
			};
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 4);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  x = RandomNo (MAXX - 300) + 150;
			  y = RandomNo (MAXY - 300) + 150;
			  lives = 2;
			};
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg5:
//don't delete the bg5 icons-bg6 uses them too
bg5::bg5 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg5.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg5.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/upbg5.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/dbg5.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg5::~bg5 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
};

void
bg5::MoveMe (int prx, int pry)
{
  if (moveCounter >= 4)			//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  xM = 2;
				}
			  else
				{
				  xM = -2;
				};
			  if (y < pry)
				{
				  yM = 2;
				}
			  else
				{
				  yM = -2;
				};
			  x += xM;
			  y += yM;
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves up or down until out of the picture
			  y += yM;			//set by the re-creation bit
			  if ((y < 0) || (y > MAXY))
				{
				  myState = DEAD;
				}
			};
		  //which icon should I use?
		  if (reborn)
			{
			  myIcon = *(iconList + 2);
			}
		  else
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList);
				}
			  else
				{
				  myIcon = *(iconList + 1);
				};
			};
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 3);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 4;
			  //move up
			  yM = -2;
			};
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg6
//same as bg5 except that the reborn move is different-don't delete bg5 icons!
bg6::bg6 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg5.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg5.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/downbg6.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/dbg5.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg6::~bg6 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
};
void
bg6::MoveMe (int prx, int pry)
{
  if (moveCounter >= 4)			//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  xM = 2;
				}
			  else
				{
				  xM = -2;
				};
			  if (y < pry)
				{
				  yM = 2;
				}
			  else
				{
				  yM = -2;
				};
			  x += xM;
			  y += yM;
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves up or down until out of the picture
			  y += yM;			//set by the re-creation bit
			  if ((y < 0) || (y > MAXY))
				{
				  myState = DEAD;
				}
			};
		  //which icon should I use?
		  if (reborn)
			{
			  myIcon = *(iconList + 2);
			}
		  else
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList);
				}
			  else
				{
				  myIcon = *(iconList + 1);
				};
			};
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 3);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 4;
			  //move down
			  yM = 2;
			};
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//no bg7-level made from bg5&6

//bg8
bg8::bg8 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg8.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg8.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/bg8r1.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/bg8r2.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/dbg5.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  numTicksPerMove = 5;
  lives = 3;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg8::~bg8 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};
void
bg8::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)	//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  xM = 2;
				}
			  else
				{
				  xM = -2;
				};
			  if (y < pry)
				{
				  yM = 2;
				}
			  else
				{
				  yM = -2;
				};
			  x += xM;
			  y += yM;
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves up or down and bounces off the edges
			  y += yM;			//set by the re-creation bit
			  x += xM;
			  if ((y < 0) || (y > MAXY))
				{
				  yM *= -1;
				};
			  if ((x < 0) || (x > MAXX))
				{
				  xM *= -1;
				};
			};
		  //which icon should I use?
		  if (reborn)
			{
			  if (myIcon = *(iconList + 2))
				{
				  myIcon = *(iconList + 3);
				}
			  else
				{
				  myIcon = *(iconList + 2);
				}
			}
		  else
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList);
				}
			  else
				{
				  myIcon = *(iconList + 1);
				};
			};
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 4);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 4;
			  //move
			  yM = -2;
			  xM = 2;
			  numTicksPerMove = 3;	//ie gets faster
			};
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg9
bg9::bg9 ()						//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg8.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg8.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/bg8r1.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/bg8r2.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/dbg5.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  numTicksPerMove = 5;
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg9::~bg9 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};
void
bg9::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)	//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  xM = 2;
				}
			  else
				{
				  xM = -2;
				};
			  if (y < pry)
				{
				  yM = 2;
				}
			  else
				{
				  yM = -2;
				};
			  x += xM;
			  y += yM;
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves up or down and bounces off the edges
			  y += yM;			//set by the re-creation bit
			  x += xM;
			  if ((y < 0) || (y > MAXY))
				{
				  yM *= -1;
				};
			  if ((x < 0) || (x > MAXX))
				{
				  xM *= -1;
				};
			};
		  //which icon should I use?
		  if (reborn)
			{
			  if (myIcon == *(iconList + 2))
				{
				  myIcon = *(iconList + 3);
				}
			  else
				{
				  myIcon = *(iconList + 2);
				}
			}
		  else
			{
			  if (xM > 0)
				{
				  myIcon = *(iconList);
				}
			  else
				{
				  myIcon = *(iconList + 1);
				};
			};
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 4);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 3;
			  //move
			  switch (RandomNo (4))
				{
				case 0:
				  xM = 2;
				  yM = 2;
				  break;
				case 1:
				  xM = -2;
				  yM = 2;
				  break;
				case 2:
				  xM = -2;
				  yM = -2;
				  break;
				case 4:
				  xM = 2;
				  yM = -2;
				  break;
				default:
				  xM = 2;
				  yM = 2;
				  break;
				};
			  numTicksPerMove = 3;	//ie gets faster
			};
		  break;
		default:				//do nothing
		  break;
		}
	}
  else
	{
	  moveCounter++;
	};
};

//bg10
bg10::bg10 ()
{
  x = RandomNo (MAXX - 30) + 30;
  y = RandomNo (MAXY - 30) + 30;
  iconList[0] = new wxIcon (wxT ("icons/lrbg10.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/udbg10.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/dbg10.xpm"), wxBITMAP_TYPE_XPM);
  myState = ALIVE;
  moveCounter = RandomNo (4);
  numTicksPerMove = 3;
  lives = 20;
  //choose which direction to move
  switch (RandomNo (4))
	{
	case 0:
	  xM = 3;
	  yM = 0;
	  myIcon = *iconList;
	  break;
	case 1:
	  xM = -3;
	  yM = 0;
	  myIcon = *iconList;
	  break;
	case 2:
	  xM = 0;
	  yM = 3;
	  myIcon = *(iconList + 1);
	  break;
	case 3:
	  xM = 0;
	  yM = -3;
	  myIcon = *(iconList + 1);
	  break;
	default:
	  xM = 3;
	  yM = 0;
	  myIcon = *(iconList);
	  break;
	};
};

bg10::~bg10 ()
{
  //delete the icons
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
};
void
bg10::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)
	{
	  switch (myState)
		{
		case ALIVE:
		  x += xM;
		  y += yM;
		  if ((x < 0) || (x > MAXX))
			{
			  xM *= -1;
			}
		  if ((y < 0) || (y > MAXY))
			{
			  yM *= -1;
			}
		  break;
		case DYING:
		  LoseALife ();
		  myIcon = *(iconList + 2);
		  break;
		default:
		  break;
		};
	  moveCounter = 1;
	}
  else
	{
	  moveCounter++;
	}
};

//bg11
bg11::bg11 ()					//constructor
{
  bool positionOK = FALSE;
  do
	{
	  x = 30 + RandomNo (MAXX - 60);
	  y = 30 + RandomNo (MAXY - 60);
	  if (((x > 100) && (x < MAXX - 100)) && ((y > 100) && (y < MAXY - 100)))
		{
		  positionOK = FALSE;
		}
	  else
		{
		  positionOK = TRUE;
		};
	}
  while (!positionOK);
  iconList[0] = new wxIcon (wxT ("icons/bg11a.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/bg11b.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/bg11c.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/bg11d.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/bg11e.xpm"), wxBITMAP_TYPE_XPM);
  iconList[5] = new wxIcon (wxT ("icons/bg11f.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *(iconList);
  myState = ALIVE;
  moveCounter = RandomNo (5);
  lives = 3;
  reborn = FALSE;				//when first one dies, next one gets reborn...
};

bg11::~bg11 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
  delete *(iconList + 5);
};
void
bg11::MoveMe (int prx, int pry)
{
  if (moveCounter >= 4)
	{
	  moveCounter = 1;
	  switch (myState)
		{
		case ALIVE:
		  if (reborn)
			{
			  switch (afterLife)
				{
				case 1:
				  x -= 2;
				  y -= 2;
				  myIcon = *(iconList + 1);
				  break;
				case 2:
				  x -= 2;
				  y -= 2;
				  myIcon = *(iconList + 2);
				  break;
				case 3:
				  x -= 3;
				  y -= 3;
				  myIcon = *(iconList + 3);
				  break;
				case 4:
				  x -= 5;
				  y -= 5;
				  myIcon = *(iconList + 4);
				  break;
				case 5:
				  x -= 9;
				  y -= 9;
				  myIcon = *(iconList + 5);
				case 6:
				  //Don't do anything-this allows the last icon to be shown
				  break;
				default:
				  myState = DEAD;	//star is burnt out
				  break;
				};
			  afterLife++;
			}
		  else
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  xM = 2;
				}
			  else
				{
				  xM = -2;
				};
			  if (y < pry)
				{
				  yM = 2;
				}
			  else
				{
				  yM = -2;
				};
			  x += xM;
			  y += yM;
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			};
		  break;
		case DYING:
		  lives = 20;			//so don't actually get killed by bullets if hit
		  afterLife = 1;
		  reborn = TRUE;
		  myState = ALIVE;
		  break;
		default:
		  break;
		};
	}
  else
	{
	  moveCounter++;
	}
};

//bg11
bg12::bg12 ()					//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg12.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg12.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/bg12r.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/bg12l.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/dbg12.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  numTicksPerMove = 5;
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn.
};

bg12::~bg12 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};

void
bg12::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)	//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  x += 2;
				  myIcon = *(iconList);
				}
			  else
				{
				  x += -2;
				  myIcon = *(iconList + 1);
				};
			  if (y < pry)
				{
				  y += 2;
				}
			  else
				{
				  y += -2;
				};
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves left or right!
			  x += xM;
			  //make them wrap round into the screen until they are killed
			  while (x < 0)
				{
				  x += MAXX;
				};
			  while (x > MAXX)
				{
				  x -= MAXX;
				};
			}
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 4);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 3;
			  //move
			  if (RandomNo (10) > 5)
				{
				  xM = 3;
				  myIcon = *(iconList + 2);
				}
			  else
				{
				  xM = -3;
				  myIcon = *(iconList + 3);
				};
			  numTicksPerMove = 2;	//ie gets faster
			};
		};
	}
  else
	{
	  moveCounter++;
	};
};

//bg13-similar to bg12
bg13::bg13 ()					//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/rbg12.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/lbg12.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/bg13u.xpm"), wxBITMAP_TYPE_XPM);
  iconList[3] = new wxIcon (wxT ("icons/bg13d.xpm"), wxBITMAP_TYPE_XPM);
  iconList[4] = new wxIcon (wxT ("icons/dbg12.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  numTicksPerMove = 5;
  lives = 2;
  reborn = FALSE;				//when first one dies, next one gets reborn.
};

bg13::~bg13 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
  delete *(iconList + 3);
  delete *(iconList + 4);
};

void
bg13::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)	//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (!reborn)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  x += 2;
				  myIcon = *(iconList);
				}
			  else
				{
				  x += -2;
				  myIcon = *(iconList + 1);
				};
			  if (y < pry)
				{
				  y += 2;
				}
			  else
				{
				  y += -2;
				};
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			}
		  else
			{
			  //it moves up or down
			  y += yM;
			  //make them wrap round into the screen until they are killed
			  while (y < 0)
				{
				  y += MAXY;
				};
			  while (y > MAXY)
				{
				  y -= MAXY;
				};
			}
		  break;
		case DYING:
		  if (reborn)
			{
			  myIcon = *(iconList + 4);
			  LoseALife ();
			}
		  else
			{
			  myState = ALIVE;
			  reborn = TRUE;
			  lives = 3;
			  //move
			  if (RandomNo (10) > 5)
				{
				  yM = 3;
				  myIcon = *(iconList + 3);
				}
			  else
				{
				  yM = -3;
				  myIcon = *(iconList + 2);
				};
			  numTicksPerMove = 2;	//ie gets faster
			};
		};
	}
  else
	{
	  moveCounter++;
	};
};

//bg15
bg15::bg15 ()					//constructor
{
  x = 30 + RandomNo (MAXX - 60);
  y = 30 + RandomNo (MAXY - 60);	//start in middle portion
  iconList[0] = new wxIcon (wxT ("icons/bg15s1.xpm"), wxBITMAP_TYPE_XPM);
  iconList[1] = new wxIcon (wxT ("icons/bg15s2.xpm"), wxBITMAP_TYPE_XPM);
  iconList[2] = new wxIcon (wxT ("icons/dbg15.xpm"), wxBITMAP_TYPE_XPM);
  myIcon = *iconList;
  myState = ALIVE;
  moveCounter = RandomNo (5);
  numTicksPerMove = 5;
  lives = 2;
  type = 1;
  typeCount = RandomNo (20);
};

bg15::~bg15 ()					//destructor
{
  delete *iconList;
  delete *(iconList + 1);
  delete *(iconList + 2);
};

void
bg15::MoveMe (int prx, int pry)
{
  if (moveCounter >= numTicksPerMove)	//every n ticks
	{
	  switch (myState)
		{
		case ALIVE:
		  if (type == 1)
			{
			  //move toward the rabbit...
			  if (x < prx)
				{
				  x += 2;
				}
			  else
				{
				  x += -2;
				};
			  if (y < pry)
				{
				  y += 2;
				}
			  else
				{
				  y += -2;
				};
			  //don't need to check for out of bounds
			  //because moves towards rabbit
			  moveCounter = 1;
			  typeCount++;
			  if (typeCount > 20)
				{
				  type = 2;
				  typeCount = 1;
				  numTicksPerMove = 1;
				  //choose a random move direction
				  xM = 3 - RandomNo (7);
				  yM = 3 - RandomNo (7);
				};
			  myIcon = *(iconList);
			}
		  else					//(type 2)
			{
			  //it moves up or down in the direction chosen earlier
			  y += yM;
			  x += xM;
			  //make them wrap round into the screen until they are killed
			  while (y < 0)
				{
				  y += MAXY;
				};
			  while (y > MAXY)
				{
				  y -= MAXY;
				};
			  while (x < 0)
				{
				  x += MAXX;
				};
			  while (x > MAXX)
				{
				  x -= MAXX;
				};
			  moveCounter = 1;
			  typeCount++;
			  if (typeCount > 20)
				{
				  type = 1;
				  typeCount = 1;
				  numTicksPerMove = 5;
				};
			  myIcon = *(iconList + 1);
			}
		  break;
		case DYING:
		  myIcon = *(iconList + 2);
		  LoseALife ();
		  break;
		};
	}
  else
	{
	  moveCounter++;
	};
}
