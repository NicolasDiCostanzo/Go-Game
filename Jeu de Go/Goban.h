#pragma once
#include "Header.h"
class Case;

using namespace std;
class Goban
{
protected:
	int size;
	bool noirDoitJouer = true;
	int yellowPoints = 0; //Noir
	float pinkPoints = 6.5;		 //Blanc

	Case* boxNotAvaiable;
	bool unavaibleTurn = false;
	bool turnedPass = false;
	bool isCaptured = false;

public:
	vector<vector<Case*>> boxes;

	Goban(SDL_Renderer* renderer, SDL_Window* window, int taille);
	~Goban();

	int GetSize();
	void SetSize(int taille);

	void SetIfCaptured(bool a_capture);
	bool GetIfCaptured();

	bool GetBlackHasToPlay();
	void SetPlayerTurn();
	Etat GetPlayerColor();

	int GetYellowPoints();
	void AddYellowPoints(int a_pointsToAdd);	
	
	float GetPinkPoints();
	void AddPinkPoints(int a_pointsToAdd);

	void ResetHasBeenCheckedParameter();

	Case* GetUnavaiableBox();
	void SetUnavaiableBox(Case* a_case);

	void SetUnavaibleTurn(bool a_tourIndispo);
	bool GetUnavaibleTurn();

	void ResetUnavaibleBox();

	void PassTurn();
	void SetTurnPassed(bool a_tourPasse);
	bool GetTurnPassed();
	void GivingUp();
};

