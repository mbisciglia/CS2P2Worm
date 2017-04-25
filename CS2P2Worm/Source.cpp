// Michael Bisciglia & Autumn Gnabasik
// Professor Perry Scott Kivolowitz
// 3/28/2017
// CS2 P2

#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#if defined(WIN32)
#include <Windows.h>
#include "curses.h"
#else
#include <unistd.h>
#include <curses.h>

#endif

using namespace std;


// struct POS allows for use of the entire terminal window by using
// the coordinates between LINES and COLS. POS() is specific to positioning
// food, POS(int,int) is specific to positioning the worm. the bool allows for
// the use of == within anything reguarding POS. int l and c are the LINES and 
// COLS, which are declared later in POS() and POS(int,int).
struct POS
{
	POS();
	POS(int l, int c);

	bool operator == (POS const & other)
	{
		return (l == other.l) && (c == other.c);
	}

	int l;
	int c;
};


// POS::POS() deterimes a random spot on the board
// bad spelling makes people think less of you (that is, if they can spell).
// This will be used to determine the placement of food on the board.
// to complete this thought, add "It is NOT used with the worm as its segments require exact control over coordinates."
POS::POS()
{
	l = rand() % (LINES - 2) + 1;
	c = rand() % (COLS - 2) + 1;
}


// POS::POS() takes the arguments int, int as they are non specific and
// are defined later
// POS() is specific to the worm and will only be used to determine the
// worm's position.
// POS() returns nothing
POS::POS(int nl, int nc)
{
l = nl;
c = nc;
}


// struct Food keeps track of everything related to the food for
// the game. This includes, how many points the food is worth, where
// the food is being rendered, and the position of food.
struct Food
{
	Food();
	POS p;
	int v;
	void Render();
};


// Food::Food() houses the variables v and p. 
// v determines the value of the food (5 through 9)
// p determines the position of the food on the board.
Food::Food()
{
	v = rand() % 5 + 5;
	p = POS();
}


// void Food::Render() renders the food on the board in whatever
// space that was determined that it would appear in.
// The comment above is sufficient to describe the whole function. That's good.
// However, the use of stringstream may not be clear as to why you chose that
// method (there are many of ways). For example, this way is more flexible than
// addch(v + '0');
void Food::Render()
{
	move(p.l, p.c);
	stringstream ss;
	ss << v;
	// addch was sufficient here.
	addstr(ss.str().c_str());
}


//	Enumerates numbers starting at 2, going to 5
enum DIR
{
	DOWN = 2,
	UP,
	LEFT,
	RIGHT,
	NODIR
};


// struct Worm groups together everything related to the worm // this is gratuitous
// Instead lead directly into the next line which takes the place
// of comments inside the class declaration. HOWEVER, putting all
// the explanation up here (for members) makes it more likely that
// new members will be forgotten (to document).
//
// this includes the position of the worm on the board, the worm
// itself as a vector, where it is being initialized to, if the
// worm has run into something else on the board, the movement of
// the worm, and how long the worm is.
struct Worm
{
	POS p;
	DIR dir = RIGHT;
	// explanation of this
	vector<POS> segments;
	// blank line if what comes next is a "different thought or concept"
	void Render();
	void Initialize();
	bool Lands_On_Me(const POS & p);
	bool Move(DIR, Food & f, int & score);
	// keep the member functions together and the member variables together - so move this up to segments.
	const int initial_length = 6;
	// And, when you do that, put a blank line between consts, statics (you don't know this yet) and ordinary variables.
	int additional_segments_needed = 0;
};



// bool Worm::Move 
// moves each segment of the worm to the position of the previous segment
// in the case of the head piece, the head is moved one spot in the direction
// selected by the player (in ProcessKeyboard)
// returns true if the player hits a wall or self (isDead)
// otherwise returns false
bool Worm::Move(DIR d, Food & f, int & score)
{
	bool isDead = false;
	//DIR dir;
	// For loop goes backwards and puts the next value at the next part of the vector
	// within segments.at(i) too allow for shifting of values through the vector
	for (size_t i = segments.size() - 1; i > 0; i--)
	{
		segments.at(i) = segments.at(i - 1);
	}


	// use a temporary POS and then add to segments.at(0) after the switch like we did in class.
	POS temp(0, 0);

	/*switch (d)
	{
	case UP:
		temp.l--;
		break;

	case DOWN:
		temp.l++;
		break;
	case LEFT:
		temp.c--;
		break;
		
	case RIGHT:
		temp.c++;
		break;
	}*/
	switch (d)
	{
	case UP:
	case DOWN:
	case LEFT:
	case RIGHT:
		dir = d;		
	}

	switch (dir)
	{
	case UP:
		temp.l--;
		break;

	case DOWN:
		temp.l++;
		break;
	case LEFT:
		temp.c--;
		break;

	case RIGHT:
		temp.c++;
		break;
	}

	POS operator+(const POS & other);
	{
		segments.at(0).l += temp.l;
		segments.at(0).c += temp.c;
	}

	// Perhaps write little itty bitty bool functions for checking against verticals and horizontals
	// if (HitTopBottom(segments.at(0)) || HitSides(segments.at(0)))
	if (segments.at(0).l == LINES - 1 || segments.at(0).c == COLS - 1 ||
		segments.at(0).l == 0 || segments.at(0).c == 0)
	{
		isDead = true;
	}

	// no harm here but this could be an else if
	else if (Lands_On_Me(POS(segments.at(0))) == true)
	{

		isDead = true;
	}

	// if (f.p == segments.at(0))
	if (f.p == POS(segments.at(0)))
	{
		// Not correct functioning. It is supposed to add one segment per later move.
		for (int i = 0; f.v > i; i++)
		{
			segments.push_back(POS(segments.at(1)));
			f = Food();
			score++;
		}
	}
	return isDead;
}

// bool Worm::Lands_On_Me() determines if something else, such as another
// part of the worm, a wall, or a piece of food is present in the space
// that the worm is instructed to move to.
// Lands_On_Me() takes the argument const POS p referenced as it states the 
// number of LINES and COLS present on the boards, minus the outside edges.
bool Worm::Lands_On_Me(const POS & p)
{

	bool isLandedOn = false;
	for (size_t i = 1; i < segments.size(); i++)
	{
		if (segments.at(i) == p)
		{
			isLandedOn = true;
			break;
		}
	}
	return isLandedOn;
}

// void Worm::Render() prints out the vector segments at the designated positions
// for each part, which are stored in segments.
void Worm::Render()
{
	for (size_t i = 0; i < segments.size(); i++)
	{
		move(segments.at(i).l, segments.at(i).c);
		
		// This code can be replaced with one line using the ternary operator assuming 
		// this first else if isn't truely needed by your code.
		addch((i == 0) ? '@' : '0');
	}
}


// void Worm::Initialize() initializes the  vector segments,
// which houses the position of the worm.
void Worm::Initialize()
{
	POS p;
	for (int i = 0; i < initial_length; i++)
	{
		// Here is your unnecessary code - if POS(int, int) were done correctly.
		p.l = LINES / 2;
		p.c = (COLS / 2) - i;
		segments.push_back(POS(p));
	}
}


// bool ProcessKeyboard determines if key selection is valid to make an action happen 
// Food f must be referenced to determine the position of the food
// Worm w must be referenced to determine where worm is
// int score must be referenced to be able to read the score in if it needs to be incremented
// True for rv is returned if the player choosed q to quit the game
// Otherwise false is returned if an acceptable movement key is selected
bool ProcessKeyboard(Food & f, Worm & w, int & score)
{
	int i = 0;
	string FraudRV;
	bool rv = false;
	time_t start;
	start = time(nullptr);
	char c = getch();

		switch (c)
		{
		case 'q':
			rv = true;
			break;

		case 'd':
			rv = w.Move(RIGHT, f, score);
			break;

		case 'w':
			rv = w.Move(UP, f, score);
			break;

		case 'a':
			rv = w.Move(LEFT, f, score);
			break;

		case 's':
			rv = w.Move(DOWN, f, score);
			break;
		case ERR:
				rv = w.Move(NODIR, f, score);
				break;
		}

		/*if (start <= 10)
		{
			rv = w.Move(RIGHT, f, score);
			start = 0;
		}*/
	
	// Checks head to see if touching body or wall
	//rv = w.Move(RIGHT, f, score);
	return rv;
}

// void DrawBorder takes the parameteres a referenced int score and the time_t start_time
// prints out a border around the window, including a timer and a score section.
// returns nothing
void DrawBorder(int & score, time_t st)
{
	border(0, 0, 0, 0, 0, 0, 0, 0);
	stringstream ss;
	ss << " Score: " << setw(4) << score << " ";
	move(0, 2);
	addstr(ss.str().c_str());

	int dT = static_cast<int>(difftime(time(nullptr), st));
	int seconds = dT % 60;
	int minutes = dT / 60 % 60;
	int hours = dT / 3600 % 24;

	ss = stringstream();
	ss << " Time: " << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << ":" << setw(2) << seconds << " ";
	move(0, COLS - 20);
	addstr(ss.str().c_str());
}

// void Die() takes in the int score to print out
// this returns nothing
// Die() prints out a ASCII art "GAME OVER" screen and
// the final score of the player
void Die(int score)
{
	//char die_chars[] = { '/', '-' , '\\', '|', 0 };
	erase();
	while (getch() == ERR)
	{
		stringstream ss;
		int mx = LINES;
		int my = COLS;


		refresh();

		//will center game over vertically
		int numOfEndls = (LINES - 22) / 2;
		//askii art game over
		// make this flash 2 times then hold on screen longer before quitting
		//use a for loop and an if statement to keep it on screen the last time 
		//for a longer time using (2000) or sleep until q is pressed
		for (int i = 0; i < numOfEndls; i++)
		{
			ss << endl;
		}
		ss << '\t' << "     GG	       AA      M     M    EE" << endl;
		ss << '\t' << "  GGGG        AAA      M    MM  EEEE" << endl;
		ss << '\t' << " GG  GG      A  A     MM   MM  EE  E" << endl;
		ss << '\t' << " G  GG	    A   A    MMM  MM   E  E" << endl;
		ss << '\t' << " G	   A   A     MM  MMM   E" << endl;
		ss << '\t' << "GG        AA   A    MMMMMMMM   EE" << endl;
		ss << '\t' << "GG    G AAAAA AA    M MMM M   EE" << endl;
		ss << '\t' << "GG GGG   AA  AA    M  MM  M  E    E" << endl;
		ss << '\t' << " GGGG    A    A    M  M  M   E   E" << endl;
		ss << '\t' << "    G   A     A   M   M  M    EEE" << endl;
		ss << '\t' << "   G" << endl;
		ss << '\t' << setw(60) << right << "Score: " << score << endl;
		ss << left;
		ss << '\t' << "          OOO     VV        EE    RR" << endl;
		ss << '\t' << "         O       VVV   V  EEEE   RRRR" << endl;
		ss << '\t' << "        O  OO   V  V   V EE  E   R  R" << endl;
		ss << '\t' << "       O  O  O V  V   V  E  E   R   R" << endl;
		ss << '\t' << "      O  O   O V  V   V  E      R   R" << endl;
		ss << '\t' << "      O     O    V   V   EE     R  R" << endl;
		ss << '\t' << "     OO    OO    V   V  EE     R RR" << endl;
		ss << '\t' << "      O    O    V   V  E    E  R  R" << endl;
		ss << '\t' << "      OO  O     VVVV   E   E   R  RR" << endl;
		ss << '\t' << "       OOO       VV     EEE        RR" << endl << endl;
		//waste_time();

		addstr(ss.str().c_str());
		refresh();
		Sleep(5000);
	}
}

//void AutoMove(DIR default, Food & f, Worm & w, int & score)
//{
//	w.Move(RIGHT, f, score);
//	Sleep(1000);
//}

// void Instructions prints out the instructions of what keys to use 
// to move the snake and begin the game
void Instructions()
{
	stringstream ss;
	move((LINES / 2) - 3, (COLS / 2) - 15);
	ss << "USE THE W, A, S, D KEYS TO MOVE" << endl << endl;
	ss << setw((COLS / 2) + 8) << "PRESS E TO BEGIN";
	ss << endl << endl << endl << endl << endl << endl << endl << endl;
	ss << setw((COLS / 2) + 12) << "PRESS Q TO QUIT THE GAME";
	addstr(ss.str().c_str());
}


int main()
{
	srand((unsigned int)time(nullptr));

	int score = 0;

	initscr();
	//	Use for testing
	//  resize_term(10, 40);
	//  with this here, you can capture the arrow keys with 2, 3, 4 and 5.
	keypad(stdscr, 1);
	cbreak();
	noecho();
	nodelay(stdscr, 1);
	curs_set(0);

	while (true)
	{
		char c = getch();
		Instructions();
		refresh();
		if ('e' == c)
		{
			break;
		}
		if ('q' == c)
		{
			Die(score);
		}
	}

	// start_time must be declaired after the instructions screen, or the timer
	// starts timing before the player starts the game
	time_t start_time;
	start_time = time(nullptr);
	Food f; // = Food();
	Worm w;
	w.Initialize();
	while (true)
	{
		//AutoMove(default, f, w, score);
		if (ProcessKeyboard(f, w, score) == true)
		{
			break;
		}
		clear();
		DrawBorder(score, start_time);
		w.Render();
		while (true)
		{
			if (w.Lands_On_Me(POS(f.p)) == true)
			{
				f.p = POS();
			}
			else
			{
				f.Render();
				break;
			}
		}
		refresh();
		Sleep(100);
	}
	Die(score);
	endwin();
}