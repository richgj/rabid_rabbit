#include "wx/wx.h"				//for the Windows stuff
#include "wx/textfile.h"		//for the high scores stuff
#include "wx/font.h"			//for non-boring fonts
#include "wx/string.h"			// To convert ascii - wxString
#include "wx/wxhtml.h"			//for the help page
#include "wx/dialog.h"
#include "rgame.h"				//the game part

#define CLIENTX 400
#define CLIENTY 400
#define GAMESPEED 30			//milliseconds per move

//declare a global variable to let us know if the timer loop overruns
static bool iRanOutOfTime;

//declare a class so it can be used in the App
class MyTimer;
class MyFrame;

class MyApp:public wxApp
{
public:
  virtual bool OnInit ();		//overridden
  int OnExit ();				//overridden

  //additional methods
  void DoTheNextThing ();		//this does all the game control
  void DrawPage ();				//to show us the game
  void DrawSetupPage (int level);	//while setting up the level
  void SetGameMoveOnClick (wxPoint point, int whichButton);	//sets the rabbit move
  void DrawHighScoreWindow ();	//when waiting for game to be started

  //additional data
  MyTimer *myTimer;
  MyFrame *myFrame;
  wxMemoryDC *myDrawingDC;
  wxBitmap *myBitmap;
  wxClientDC *myDC;
  Game *myGame;
private:

};
DECLARE_APP (MyApp);			//so I can get at the public method(s) in MyTimer

class MyTimer:public wxTimer
{
  //override this member only
  void Notify ();
};

class MyFrame:public wxFrame
{
public:

  //constructor(s)
  MyFrame (const wxString & title, const wxPoint & pos, const wxSize & size);
  //destructor
   ~MyFrame ()
  {
	//delete all heap stuff
	//wxFrame gets rid of the menu bar
  }
  //member functions
  void OnStart (wxCommandEvent & event);
  void OnQuit (wxCommandEvent & event);
  void OnAbout (wxCommandEvent & event);
  void OnHelp (wxCommandEvent & event);
  //process mouse clicks
  void MyMouseClick (wxMouseEvent & event);


private:
  wxHtmlWindow * html;
  wxDialog *htmlDialog;
  wxMenu *menuFile;
  wxMenu *menuHelp;
  wxMenuBar *menuBar;
  DECLARE_EVENT_TABLE ();
};

//create arbitrary numbers to use for event handling
enum
{
  ID_QUIT = 1,
  ID_ABOUT,
  ID_START,
  ID_HELP,
};

//link an event with a specific subroutine to run
BEGIN_EVENT_TABLE (MyFrame, wxFrame)
EVT_MENU (ID_QUIT, MyFrame::OnQuit)
EVT_MENU (ID_ABOUT, MyFrame::OnAbout)
EVT_MENU (ID_START, MyFrame::OnStart)
EVT_MENU (ID_HELP, MyFrame::OnHelp)
EVT_MOUSE_EVENTS (MyFrame::MyMouseClick) END_EVENT_TABLE ()
//This sets up the application before it is implemented
	 bool
	 MyApp::OnInit ()
{
  //set the working directory
  wxSetWorkingDirectory (wxT ("/usr/games/rrabbit"));

  //Create a window to control the app
  try
  {
	myFrame = new MyFrame (wxT ("Rabid Rabbit"),
						   wxPoint (50, 50), wxSize (100, 100));
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }
  //set the client window area
  myFrame->SetClientSize (CLIENTX, CLIENTY);
  myFrame->Show (TRUE);
  SetTopWindow (myFrame);

  //set up a drawing window
  try
  {
	myDrawingDC = new wxMemoryDC ();
  myBitmap = new wxBitmap (CLIENTX, CLIENTY);
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }
  myDrawingDC->SelectObject (*myBitmap);
  myDrawingDC->SetBackground (*wxBLACK_BRUSH);
  myDrawingDC->SetTextBackground (*wxBLACK);
  myDrawingDC->SetTextForeground (*wxCYAN);
  myDrawingDC->Clear ();

  //set up a visible window
  try
  {
	myDC = new wxClientDC (GetTopWindow ());
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }

  //create the game
  try
  {
	myGame = new Game ();
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }

  //set the global timer variable
  iRanOutOfTime = FALSE;

  //start off the timer
  try
  {
	myTimer = new MyTimer;
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }
  myTimer->Start (GAMESPEED);	//every xxx milli seconds

  return TRUE;
};

//set the rabbit moving in the right direction
void
MyApp::SetGameMoveOnClick (wxPoint point, int whichButton)
{
//left mouse button changes direction of rabbit
//right mouse button changes direction of rabbit and fire direction
//middle button stops mouse, but continues firing
  float xDiff, yDiff, tanA;
  int xM, yM;					//temporary move variables
  int fDir;						//temporary fire direction
  wxPoint rPoint;

  rPoint = myGame->RabbitPos ();

//the yucky maths stuff works out which octant the click is in
//relevant to the Rabbit
//it also sets a fire direction if it's not a left click
  xDiff = point.x - (rPoint.x + 8);
  yDiff = point.y - (rPoint.y + 8);	//the 8 is to put it into the middle of the icon
  if (xDiff > 0)
	{
	  tanA = yDiff / xDiff;
	  if (tanA <= -2.414)
		{
		  xM = 0;
		  yM = -1;
		  fDir = 8;
		}
	  else if ((tanA > -2.414) && (tanA <= -0.4142))
		{
		  xM = 1;
		  yM = -1;
		  fDir = 9;
		}
	  else if ((tanA > -0.4142) && (tanA <= 0.4142))
		{
		  xM = 1;
		  yM = 0;
		  fDir = 6;
		}
	  else if ((tanA > 0.4142) && (tanA <= 2.414))
		{
		  xM = 1;
		  yM = 1;
		  fDir = 3;
		}
	  else						//tanA > 2.414
		{
		  xM = 0;
		  yM = 1;
		  fDir = 2;
		}
	}
  else if (xDiff < 0)
	{
	  tanA = yDiff / xDiff;
	  if (tanA <= -2.414)
		{
		  xM = 0;
		  yM = 1;
		  fDir = 2;
		}
	  else if ((tanA > -2.414) && (tanA <= -0.4142))
		{
		  xM = -1;
		  yM = 1;
		  fDir = 1;
		}
	  else if ((tanA > -0.4142) && (tanA <= 0.4142))
		{
		  xM = -1;
		  yM = 0;
		  fDir = 4;
		}
	  else if ((tanA > 0.4142) && (tanA <= 2.414))
		{
		  xM = -1;
		  yM = -1;
		  fDir = 7;
		}
	  else						//tanA > 2.414
		{
		  xM = 0;
		  yM = -1;
		  fDir = 8;
		}
	}
  else							//xdiff == 0
	{
	  if (yDiff > 0)
		{
		  xM = 0;
		  yM = 1;
		  fDir = 2;
		}
	  else
		{
		  xM = 0;
		  yM = -1;
		  fDir = 8;
		}
	};
  if (whichButton == 2)
	{
	  xM = 0;
	  yM = 0;
	};
//now set the move:
  myGame->SetRabbitMove (wxPoint (xM, yM));
//now change the fire direction
  if (whichButton == 3)			//right click
	{
	  myGame->SetFireDir (fDir);
	};
};

//What to do when the application finished
int
MyApp::OnExit ()
{
  //delete all the things created to stop memory leaks
  delete myDrawingDC;
  delete myDC;
  delete myTimer;
  delete myGame;
  //delete myFrame; is done by the system
  return 0;
}

//This controls the whole rGame
void
MyApp::DoTheNextThing ()
{
//this be the control based on the state of the Game
  switch (myGame->GetState ())
	{
	case PLAY:
	  myGame->NextMove ();		//do a move of the game
	  DrawPage ();				//show us what happened
	  if (myGame->GetRabbitKilled ())
		{
		  myGame->SetState (RABBITKILLED);
		};
	  if (myGame->GetLevelComplete () == TRUE)
		{
		  myGame->SetState (LEVELCOMPLETE);
		};
	  break;
	case SETUP:
	  myTimer->Stop ();
	  DrawSetupPage (myGame->GetLevel ());	//probably won't get seen for long...
	  myGame->SetUpLevel ();
	  myGame->SetState (READYTOPLAY);
	  DrawPage ();				//let the player see the setup for...
	  myTimer->Start (2000);	//pause to let the player see the page!
	  break;
	case READYTOPLAY:
	  myTimer->Stop ();
	  myGame->SetState (PLAY);
	  myTimer->Start (GAMESPEED);
	  break;
	case RABBITKILLED:
	  myTimer->Stop ();
	  myGame->SetRabbitKilled (FALSE);
	  if (myGame->RabbitState () == ALIVE)
		{
		  myGame->DeleteLevel ();	//to recover the memory
		  myGame->SetState (SETUP);
		}
	  else
		{
		  myGame->DeleteLevel ();	//to recover the memory
		  myGame->SetState (GAMEOVER);
		};
	  myTimer->Start (2000);
	  break;
	case LEVELCOMPLETE:
	  myTimer->Stop ();
	  //recover the memory used
	  myGame->DeleteLevel ();
	  myGame->SetLevelComplete (FALSE);
	  if (myGame->GetLevel () == LASTLEVEL)
		{
		  myGame->SetState (GAMEOVER);
		}
	  else
		{
		  myGame->SetLevel (myGame->GetLevel () + 1);
		  myGame->SetState (SETUP);
		};
	  myTimer->Start (2000);	//Wait 2s after finishing level before setting up next level
	  break;
	case GAMEOVER:
	  myTimer->Stop ();
	  wxMessageBox (wxT ("Game Over"), wxT ("That's all Folks!"));
	  myGame->CheckHighScores ();
	  //reset the whole game
	  delete myGame;
	  try
	  {
		myGame = new Game;
	  }
	  catch (void *theError)
	  {
		wxMessageBox (wxT ("Memory Allocation Error"));
		wxExit ();
	  }

	  myGame->SetState (WAITING);
	  myTimer->Start (500);
	  break;
	case WAITING:				//Here at start and end of Game
	  myTimer->Stop ();
	  DrawHighScoreWindow ();
	  myTimer->Start (500);
	  break;
	default:
	  wxMessageBox (wxT ("Should never reach here"),
					wxT ("Error in DoTheNextThing"));
	  myTimer->Stop ();
	  break;
	};
};

//this is the bit we override to make sure that things happen that we want
//it checks to make sure the PC keeps up, then calls DoTheNextThing
void
MyTimer::Notify ()
{
  if (iRanOutOfTime)
	{
	  //stop the timer
	  Stop ();
	  //tell us the problem
	  wxMessageBox (wxT ("Fatal Error"),
					wxT
					("Timer has been called before\n the previous loop finished."),
					wxOK);
	}
  else
	{
	  iRanOutOfTime = TRUE;
	  wxGetApp ().DoTheNextThing ();	//call the main function for the program
	  iRanOutOfTime = FALSE;
	}
}

//constructor for the frame (calls the wxFrame constructor directly)
MyFrame::MyFrame (const wxString & title, const wxPoint & pos,
				  const wxSize & size):
wxFrame ((wxFrame *) NULL, -1, title, pos, size,
		 ~wxRESIZE_BORDER & (wxSYSTEM_MENU | wxCAPTION))
{
  html = NULL;
  htmlDialog = NULL;
  try
  {
	menuFile = new wxMenu;
  }								//deleted by wxFrame
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }

  menuFile->Append (ID_START, wxT ("&Start"));
  menuFile->AppendSeparator ();
  menuFile->Append (ID_QUIT, wxT ("E&xit"));

  try
  {
	menuHelp = new wxMenu;
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }

  menuHelp->Append (ID_HELP, wxT ("&Help"));
  menuHelp->AppendSeparator ();
  menuHelp->Append (ID_ABOUT, wxT ("&About"));

  try
  {
	menuBar = new wxMenuBar;
  }								//deleted by wxFrame
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }
  menuBar->Append (menuFile, wxT ("&File"));
  menuBar->Append (menuHelp, wxT ("&Help"));

  SetMenuBar (menuBar);

  CreateStatusBar (3);			//status bar has three sections
}

void
MyFrame::OnStart (wxCommandEvent & event)
{
  //tell the game to get set up if it's WAITING
  if (wxGetApp ().myGame->GetState () == WAITING)
	{
	  wxGetApp ().myGame->SetState (SETUP);
	};
}

void
MyFrame::OnQuit (wxCommandEvent & WXUNUSED (event))
{
  Close (TRUE);
}

void
MyFrame::OnAbout (wxCommandEvent & WXUNUSED (event))
{
  wxMessageBox (wxT
				("Rabid Rabbit v1.3\nDate: Feb 2017\nAuthor: Richard Jones"),
				wxT ("Information"), wxOK | wxICON_INFORMATION);
}

void
MyFrame::OnHelp (wxCommandEvent & WXUNUSED (event))
{
  if (NULL == htmlDialog)
	{
	  htmlDialog = new wxDialog (this, -1, wxT ("Rabid Rabbit Help"),
								 wxDefaultPosition, wxSize (600, 400),
								 wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX
								 | wxMAXIMIZE_BOX);
	}
  if (NULL == html)
	{
	  html = new wxHtmlWindow (htmlDialog);
	}


  html->SetRelatedFrame ((wxFrame *) htmlDialog,
						 wxT ("Help for Rabid Rabbit"));

  if (FALSE == html->LoadPage (wxT ("docs/RRabbit.htm")))
	{
	  wxMessageBox (wxT ("Failed to load Help File. \n"
						 "Please view <dir>/docs/RRabbit.htm using a browser"),
					wxT ("Warning"));
	}

  htmlDialog->Centre ();
  htmlDialog->Show (TRUE);
};


void
MyFrame::MyMouseClick (wxMouseEvent & event)
{
//send the position of a click to the App
//so it can work out which way to move the rabbit
//last parameter sent is True if it was a left button
  int theButton;				//to send to the movement control

  if (event.ButtonDown ())
	{
	  if (event.ButtonDown (1) == TRUE)
		{
		  theButton = 1;
		};
	  if (event.ButtonDown (2) == TRUE)
		{
		  theButton = 2;
		};
	  if (event.ButtonDown (3) == TRUE)
		{
		  theButton = 3;
		};
	  wxGetApp ().SetGameMoveOnClick (wxPoint (event.GetX (), event.GetY ()),
									  theButton);
	}
  else
	{
	  event.Skip ();
	};

}

void
MyApp::DrawPage ()
{
  //draw to the invisible window
  myDrawingDC->Clear ();

  //BadGuys
  for (int i = 0; i < myGame->GetNumBadGuys (); i++)
	{
	  if ((myGame->BadGuyState (i) == ALIVE)
		  || (myGame->BadGuyState (i) == DYING))
		{
		  myDrawingDC->DrawIcon (*myGame->BadGuyIcon (i),
								 myGame->BadGuyPos (i));
		};
	};
  //Sheep:
  for (int i = 0; i < 5; i++)
	{
	  //Draw the sheep if alive caught or dying
	  if ((myGame->SheepState (i) == ALIVE)
		  || (myGame->SheepState (i) == DYING))
		{
		  myDrawingDC->DrawIcon (*myGame->SheepIcon (i),
								 myGame->SheepPos (i));
		};
	};
  //Draw the Rabbit
  if ((myGame->RabbitState () == ALIVE) || (myGame->RabbitState () == DYING)
	  || (myGame->RabbitState () == INVULNERABLE))
	{
	  myDrawingDC->DrawIcon (*myGame->RabbitIcon (), myGame->RabbitPos ());
	};
  //draw the bullets
  for (int i = 0; i < NUMBULLETS; i++)
	{
	  //if not dead, draw the bullets
	  if (myGame->BulletState (i) != DEAD)
		{
		  myDrawingDC->DrawIcon (*myGame->BulletIcon (i),
								 myGame->BulletPos (i));
		};
	};
  //draw the bonuses
  for (int i = 0; i < myGame->GetNumBonuses (); i++)
	{
	  //only draw if ALIVE
	  if (myGame->BonusState (i) == ALIVE)
		{
		  myDrawingDC->DrawIcon (*myGame->BonusIcon (i),
								 myGame->BonusPos (i));
		};
	};

  //copy the drawing DC to the visible one
  myDC->Blit (0, 0, CLIENTX, CLIENTY, myDrawingDC, 0, 0);

  //Set up the status bar
  wxString myStatus;

  myStatus.Printf (wxT ("Score: %i"), myGame->GetScore ());
  myFrame->SetStatusText (myStatus, 0);	//first pos on Bar

  myStatus.Printf (wxT ("Lives: %i"), myGame->GetRabbitLives ());
  myFrame->SetStatusText (myStatus, 1);	//second position on bar

  myStatus.Printf (wxT ("Lives: %i"), myGame->GetRabbitSheepInTow ());
  myFrame->SetStatusText (myStatus, 2);	//Third position
}

void
MyApp::DrawSetupPage (int level)
{
  myDC->Clear ();
  wxString textToUse;

  textToUse.Printf (wxT ("Setting up Level: %d"), level);
  myDC->DrawText (textToUse, 40, 40);

};

void
MyApp::DrawHighScoreWindow ()
{
//to be consistent, I should get the scores from the game, rather than
//the file, but I can't be
//bothered to write all the extra routines.
//Besides, this is quicker.
  wxTextFile *textIn;
  wxString tmp, theName[10], theScore[10];
  wxFont myFont;

  myFont = wxFont (*wxSWISS_FONT);

  try
  {
	textIn = new wxTextFile;
  }
  catch (void *theError)
  {
	wxMessageBox (wxT ("Memory Allocation Error"));
	wxExit ();
  }
  if (wxFileExists (wxT ("ScoreS")))
	{
	  textIn->Open (wxT ("ScoreS"));
	  //there are 20 lines
	  //0,2,4... are the names,
	  //1,3,7... are the scores
	  //put them into strings
	  for (int i = 0; i < 10; i++)
		{
		  tmp = textIn->GetLine (i * 2);
		  theName[i] = tmp.Pad (21 - tmp.Len (), '.');
		  theScore[i] = textIn->GetLine ((i * 2) + 1);
		};
	  myDrawingDC->Clear ();
	  //Write the title
	  myFont.SetPointSize (30);
	  myFont.SetStyle (wxSLANT);
	  myFont.SetWeight (wxBOLD);
	  myFont.SetFamily (wxSCRIPT);
	  myDrawingDC->SetTextForeground (*wxLIGHT_GREY);
	  myDrawingDC->SetFont (myFont);
	  myDrawingDC->DrawText (wxT ("Rabid Rabbit"), 10, 10);

	  //Write the high scores title
	  myFont.SetPointSize (20);
	  myFont.SetStyle (wxITALIC);
	  myFont.SetWeight (wxBOLD);
	  myFont.SetFamily (wxROMAN);
	  myDrawingDC->SetTextForeground (*wxRED);
	  myDrawingDC->SetFont (myFont);
	  myDrawingDC->DrawText (wxT ("High Scores"), 80, 65);

	  //Put in the Scores
	  myDrawingDC->SetFont (*wxNORMAL_FONT);	//should default to normal one

	  myDrawingDC->SetTextForeground (*wxCYAN);
	  for (int i = 0; i < 10; i++)
		{
		  myDrawingDC->DrawText (theName[i], 100, 105 + i * 20);
		};
	  myDrawingDC->SetTextForeground (*wxGREEN);
	  for (int i = 0; i < 10; i++)
		{
		  myDrawingDC->DrawText (theScore[i], 250, 105 + i * 20);
		};

	  //close the file and retrieve its memory block
	  textIn->Close ();
	  delete textIn;
	}
  else
	{
	  wxMessageBox (wxT ("ScoreS file not found"), wxT (""));
	  myTimer->Stop ();
	  myFrame->Close ();		//close down gracefully
	};

  //print a rabbit somewhere
  int x, y;
  x = (int) ((CLIENTX - 16) * (float) rand () / (float) RAND_MAX);
  y = (int) ((CLIENTY - 16) * (float) rand () / (float) RAND_MAX);
  myDrawingDC->DrawIcon (*myGame->RabbitIcon (), x, y);
  //copy the drawing DC to the visible one
  myDC->Blit (0, 0, CLIENTX, CLIENTY, myDrawingDC, 0, 0);

};

//The Main() function:
IMPLEMENT_APP (MyApp);
