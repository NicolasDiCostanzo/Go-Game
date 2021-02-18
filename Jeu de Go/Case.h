#pragma once
#include "Header.h"

class Goban;

bool GroupIsCaptured(vector<Case*> a_groupe);
vector<Case*> StoneGroup(Case* a_case);

//Fonctions de debug
void DisplayCoordinates(Case* _case);
void DisplayState(Etat a_etat);
void DisplayVector(vector<Case*> a_vecteur);


class Case
{
protected:
	Etat state;

	pair<int, int> positionInPx;
	pair<int, int> coordinates;

	Goban* p_goban;

	SDL_Texture* boxTexture;

	SDL_Renderer* p_renderer;
	SDL_Window* p_window;

	bool hasBeenChecked = false;

	bool unavaibleForOneTurn = false;
public:
	Case();
	Case(Goban* goban, SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* caseTexture, int a_index, int a_tailleGoban);

	~Case();

	void CreateBox();

	void handleEvent(SDL_Event* e);

	void SetIndex(int a_index, int a_tailleGoban);
	pair<int, int> GetIndex();

	void SetPositionInPx(int x, int y);
	std::pair<int, int> GetPositionInPx();

	void SetState(Etat a_etat);
	Etat GetOppositeState(Etat a_etat);
	Etat GetState();

	bool BoxIsEmpty();

	bool KilledItself(vector<Case*> a_groupe);

	bool HasFreedoms();

	vector<Case*> GetSurroundings();

	void SetHasBeenChecked(bool a_hasBeenChecked);
	bool GetHasBeenChecked();
	
	void SetTexture(SDL_Texture* caseTexture);
	SDL_Texture* GetTexture();

	void SetUnavailability(bool a_indispo);
	bool GetUnavailability();

	void Erase();
};



