#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int b1x = 50;
int b1y = 200;
int b2x = 50;
int b2y = 350;



//if minion is on team 2, move it to 250 to attack
int t1minionRecieveAttackPosY = 255;

//if minion is on team 1, move it to 345
int t2minionRecieveAttackPosY = 295;

//should handle positioning of the minions

class Minion {

public:
	int power, health, xPosition, yPosition, team, recieveAttackLocX, recieveAttackLocY, index;
	Minion * target;
	string name;
	int height = 50;
	int weight = 50;
	int targetPos = 0;
	bool isAttacking = false;
	bool isRetreating = false;
	bool isDead = false;

	Minion() {

	};

	Minion(string n, int p, int h, int t, int idx)
	{
		name = n;
		power = p;
		health = h;
		index = idx;

		if (t == 1) {
			team = 1;
			xPosition = b1x + 100 * idx;
			yPosition = b1y;
			recieveAttackLocX = b1x + 100 * idx;
			recieveAttackLocY = t1minionRecieveAttackPosY;
		}

		if (t == 2) {
			team = 2;
			xPosition = b2x + 100 * idx;
			yPosition = b2y;
			recieveAttackLocX = b2x + 100 * idx;
			recieveAttackLocY = t2minionRecieveAttackPosY;
		}

	}

	void changeYpos(int yOffset) {
		yPosition = yPosition + yOffset;
		//cout << name + " moved To " + to_string(yPosition) << endl;
	}

	void changeXpos(int xOffset) {
		xPosition = xPosition + xOffset;
		//cout << name + " moved To " + to_string(yPosition) << endl;
	}

	void performAttack(Minion* t, int tPos) {
		this->target = t;
		this->targetPos = tPos;
		isAttacking = true;

		if (this->yPosition != target->recieveAttackLocY) {
			if (this->yPosition > target->recieveAttackLocY) {
				this->changeYpos(-1);
			}
			else {
				this->changeYpos(1);
			}
		}

		if (this->xPosition != target->recieveAttackLocX) {
			if (this->xPosition > target->recieveAttackLocX) {
				this->changeXpos(-1);
			}
			else {
				this->changeXpos(1);
			}
		}

		if (this->yPosition == target->recieveAttackLocY) {
			//cout << this->name + " attacks " + this->target->name + " for " + to_string(this->power) << endl;
			this->recieveAttack(target);


			target->recieveAttack(this);

			isAttacking = false;
			isRetreating = true;
		}




	}

	void performRetreat() {
		int originalY = this->team == 1 ? b1y : b2y;
		int originalX = this->team == 1 ? b1x + 100*this->index : b2x + 100 * this->index;

		if (this->yPosition != originalY) {
			if (this->yPosition > originalY) {
				this->changeYpos(-1);
			}
			else {
				this->changeYpos(1);
			}
		}

		if (this->xPosition != originalX) {
			if (this->xPosition > originalX) {
				this->changeXpos(-1);
			}
			else {
				this->changeXpos(1);
			}
		}

		if (this->yPosition == originalY) {
			isRetreating = false;
		}

	}

	void recieveAttack(Minion* t) {
		health = health - t->power;
		//cout << this->name + " recieves damage of " + to_string(t->power) + " from " + t->name << endl;
		if (health <= 0) {
			//cout << this->name + " has died!" << endl;
			isAttacking = false;
			isRetreating = false;
			isDead = true;
		}

	}
	//
	void drawSelf(olc::PixelGameEngine* pge) {
		if (isDead) {};
		if (isAttacking) this->performAttack(this->target, this->targetPos);
		if (isRetreating) this->performRetreat();
		pge->FillRect(xPosition, yPosition, weight, height);
		pge->DrawString(xPosition, yPosition, name, olc::BLACK);
		pge->DrawString(xPosition, yPosition + 15, "Y" + to_string(yPosition), olc::BLACK);
		pge->DrawString(xPosition, yPosition + height - 10, to_string(power), olc::BLACK);
		pge->DrawString(xPosition + weight - 10, yPosition+height-10, to_string(health), olc::BLACK);

	}



};

//make struct here for team1? Needs to have health
// could be hero also to store name property

struct Hero {
	string Name;
	int Health;
	list<Minion*> aliveMinions;
	//list<Minion*> deadMinions;
	int attackingMinionPos;
};

class Board {

	//minion that is currently attacking or retreating
	Minion* activeMinion;
	int numAttacks = 0;

	//choose first active minion, let it attack, after that the active minion has to be on the other team
	bool roundIsOver;
	bool team1Attacked;
	bool team2Attacked;
	Hero& activeTeam;
	Hero& inactiveTeam;
	Hero team1;
	Hero team2;


public:


	Board(Hero& h1, Hero& h2) : activeTeam(h1), inactiveTeam(h2) {
		bool t1first = (rand() % 2) != 0;
		team1 = h1;
		team2 = h2;
		//REMOVE THIS AFTER TESTING

		t1first = true;

		if (t1first) {
			activeTeam = team1;
			inactiveTeam = team2;
		}

		else {
			activeTeam = team2;
			inactiveTeam = team1;
		}

		roundIsOver = false;


	};



	void displayGameState(olc::PixelGameEngine* pge) {
		pge->DrawString(540, 510, "Game State:");
		pge->DrawString(540, 520, !roundIsOver ? "Fighting" : "Done");
		pge->DrawString(700, 10, team1.Name+" Health: " + to_string(team1.Health));
		pge->DrawString(700, 30, team2.Name+" Health: " + to_string(team2.Health));
		pge->DrawString(0, b1y, team1.Name);
		pge->FillRect(b1x, b1y, 500, 50, olc::CYAN);
		//b2
		pge->DrawString(0, b2y, team2.Name);
		pge->FillRect(b2x, b2y, 500, 50, olc::CYAN);

	}


	//determine the damage the player takes

	void handleRoundEnd() {
		roundIsOver = true;
		bool team1dead, team2dead, tie;
		tie = false;
		team1dead = false;
		team2dead = false;

		if (team1.aliveMinions.size() == 0) {
			team1dead = true;
		}

		if (team2.aliveMinions.size() == 0) {
			team2dead = true;
		}

		if (team1dead && team2dead) {
			tie = true;
		}

		if (!tie) {

			//this is not updating correctly

			if (team1dead) {
				team1.Health = team1.Health - 3;
			}
			if (team2dead) {
				team2.Health = team2.Health - 3;
			}
		}

	}



	//need to handle each unit attacking
	//this is only going to handle one frame

	//need logic like 1 move minion 1 frame (eventually complete attacking motion)
	//move unit back (eventually complete retreating motion)

	//METHOD GETS CALLED ON EACH FRAME
	void updateState() {
		Minion* target = nullptr;
		list<Minion*>::iterator it = activeTeam.aliveMinions.begin();
		list<Minion*>::iterator it2 = inactiveTeam.aliveMinions.begin();

		if (roundIsOver) return;

		// first time method is called, perform setup
		if (activeMinion == nullptr) {

			//cout << "activeTeam: " + activeTeam.Name << endl;
			//cout << "inactiveTeam: " + inactiveTeam.Name << endl;

			//cout << activeTeam.Name + " has attack minion Pos: " + to_string(activeTeam.attackingMinionPos) << endl;
			//cout << inactiveTeam.Name + " has attack minion Pos: " + to_string(inactiveTeam.attackingMinionPos) << endl;





			advance(it, activeTeam.attackingMinionPos);

			activeMinion = *it;


			int targetPos = (rand() % inactiveTeam.aliveMinions.size());
			advance(it2, targetPos);
			target = *it2;

			//cout << activeMinion->name + " is attacking " + target->name << endl;


			activeMinion->performAttack(target, targetPos);
			return;

		}


		//state updating, return;
		if (activeMinion != nullptr) {
			if (activeMinion->isAttacking || activeMinion->isRetreating) {
				return;
			}
		}


		if (activeTeam.aliveMinions.size() == 0 || inactiveTeam.aliveMinions.size() == 0) {
				handleRoundEnd();
				return;
		}



		else {

			//perform cleanup from previous activeMinion attack
			if (activeMinion->target->isDead) {
				//cout << target->name + " died" << endl;
				//cout << "removing " + (*it2)->name << endl;
				advance(it2, activeMinion->targetPos);
				it2 = inactiveTeam.aliveMinions.erase(it2);

			}


			if (activeMinion->isDead) {
				//cout << activeMinion->name + " died" << endl;
				//cout << "removing " + (*it)->name << endl;
				it = activeTeam.aliveMinions.erase(it);


			}

			if (activeTeam.attackingMinionPos == activeTeam.aliveMinions.size() - 1) {
				activeTeam.attackingMinionPos = 0;
			}

			else activeTeam.attackingMinionPos++;





			//cout << activeTeam.Name + " has attack minion Pos: " + to_string(activeTeam.attackingMinionPos) << endl;
			//cout << inactiveTeam.Name + " has attack minion Pos: " + to_string(inactiveTeam.attackingMinionPos) << endl;

			// end cleanup

			// check to see if next attack should start

			if (activeTeam.aliveMinions.size() == 0 || inactiveTeam.aliveMinions.size() == 0) {
				handleRoundEnd();
				return;
			}

			//start next attack
			Hero tempTeam = activeTeam;


			activeTeam = inactiveTeam;
			inactiveTeam = tempTeam;

			//cout << "activeTeam: " + activeTeam.Name << endl;
			//cout << "inactiveTeam: " + inactiveTeam.Name << endl;

			//cout << activeTeam.Name + " has attack minion Pos: " + to_string(activeTeam.attackingMinionPos) << endl;
			//cout << inactiveTeam.Name + " has attack minion Pos: " + to_string(inactiveTeam.attackingMinionPos) << endl;


			it = activeTeam.aliveMinions.begin();
			it2 = inactiveTeam.aliveMinions.begin();


			advance(it, activeTeam.attackingMinionPos);

			activeMinion = *it;


			int targetPos = (rand() % inactiveTeam.aliveMinions.size());
			advance(it2, targetPos);
			target = *it2;

			//cout << activeMinion->name + " is attacking " + target->name << endl;


			activeMinion->performAttack(target, targetPos);




		}

	}

};




class AutoChess : public olc::PixelGameEngine
{
public:
	AutoChess()
	{
		sAppName = "Blake Auto Chess";

	}

public:

	olc::Pixel _minionOneColor = olc::CYAN;
	olc::Pixel _minionTwoColor = olc::DARK_RED;

	//need to draw the board

	// b1,b2,b3

	// a1,a2,a3

	//make constant for minion height, and width

	//make constant for seperation between minions on each side of the board

	// make constant for t1 location vs. t2 location

	//make function like draw board one, draw board two



		Minion voidWalker = Minion("VdWlkr", 3, 1, 1, 0);
		Minion selflessHero = Minion("slfHro", 1, 2, 1, 1);
		Minion tideCaller = Minion("tdCllr", 3, 3, 2, 0);
		Minion tideHunter = Minion("tdHntr", 1, 2, 2, 1);

		list<Minion*> team1 = { &voidWalker,&selflessHero };
		list<Minion*> team2 = { &tideCaller,&tideHunter };

		Hero milHouse = { "Milhouse",40, team2, 0 };
		Hero rafam = { "Rafam",40, team1,  0 };



		Board currentBoard = Board(rafam, milHouse);



	bool OnUserCreate() override
	{

		srand((unsigned)time(0));

		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);


		currentBoard.updateState();
		currentBoard.displayGameState(this);
		FillRect(0, 0, 50, 50, olc::BLUE);
		//add string to move voidwalker down above the red button
		// then make it so that when you click the red button, the voidwalker moves down



		FillRect(550, 550, 50, 50, olc::RED);


		FillRect(550, 0, 50, 50, olc::YELLOW);

		//add string to move tide caller up above the green button
		// then make it so that when you click the green button, the tidecaller moves up
		//DrawString(0, 510, "Press W");
		//DrawString(0, 520, "Attack with Tidecaller");
		FillRect(0, 550, 50, 50, olc::GREEN);

		//b1





		voidWalker.drawSelf(this);
		tideCaller.drawSelf(this);


		selflessHero.drawSelf(this);
		tideHunter.drawSelf(this);



		return true;
	}
};


int main()
{
	AutoChess demo;
	if (demo.Construct(900, 900, 1, 1))
		demo.Start();

	return 0;
}
