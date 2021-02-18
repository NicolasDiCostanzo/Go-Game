#include "Case.h"
#include "Fonctions.h"
#include "Goban.h"

class Goban;

SDL_Surface* caseVideSurface = SDL_LoadBMP("Sprites\\Images\\case.bmp");
SDL_Surface* caseNoireSurface = SDL_LoadBMP("Sprites\\Images\\noir.bmp");
SDL_Surface* caseBlancheSurface = SDL_LoadBMP("Sprites\\Images\\blanc.bmp");

Mix_Chunk* putStoneSound;

int marginTop = 30;

Case::Case(Goban* goban, SDL_Renderer* renderer, SDL_Window* a_window, SDL_Texture* caseTexture, int a_index, int a_tailleGoban) {
	SetIndex(a_index, a_tailleGoban);

	p_goban = goban;

	int centerX = (SCREEN_WIDTH / 2) - (BOX_SIZE / 2 * a_tailleGoban);
	int centerY = (SCREEN_HEIGHT / 2) - (BOX_SIZE / 2 * a_tailleGoban) + marginTop;

	int x = centerX + GetIndex().first * BOX_SIZE;
	int y = centerY + GetIndex().second * BOX_SIZE;

	p_renderer = renderer;
	p_window = a_window;

	SetPositionInPx(x, y);
	SetTexture(caseTexture);
}

//Destructeur
Case::~Case() { cout << "Case x : " + to_string(coordinates.first) + " y :" + to_string(coordinates.second) + " detruite" << endl; }

//Gérer le click sur les cases
void Case::handleEvent(SDL_Event* e)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		int thisCaseX = GetPositionInPx().first;
		int thisCaseY = GetPositionInPx().second;

		//Check if mouse is in button
		bool inside = true;

		if		(x <= thisCaseX)			 inside = false;
		else if (x > thisCaseX + BOX_SIZE) inside = false;
		else if (y <= thisCaseY)			 inside = false;
		else if (y > thisCaseY + BOX_SIZE) inside = false;

		if (inside) {
			system("cls");

			bool doitResetCaseIndispo = false;

			if (!BoxIsEmpty() || this == p_goban->GetUnavaiableBox()) {
				cout << "Case occupee" << endl;
				return;
			}

			if (p_goban->GetTurnPassed()) p_goban->SetTurnPassed(false);

			if (p_goban->GetUnavaiableBox()) doitResetCaseIndispo = true;

			state = p_goban->GetPlayerColor();

			p_goban->ResetHasBeenCheckedParameter();

			vector<Case*> groupe = StoneGroup(this);
			p_goban->ResetHasBeenCheckedParameter();

			if (!KilledItself(groupe)) CreateBox();
			else {
				state = Etat::Empty;
				cout << "Suicide" << endl;
			}
			if (p_goban->GetUnavaiableBox() && doitResetCaseIndispo) p_goban->ResetUnavaibleBox();
		}
	}
}

void Case::CreateBox() {
	p_goban->SetPlayerTurn();

	SDL_Surface* newSurface = caseVideSurface;


	switch (state) {
	case Etat::Noir:
		newSurface = caseNoireSurface;
		break;
	case Etat::Blanc:
		newSurface = caseBlancheSurface;
		break;
	}

	if (state != Etat::Empty) {
		putStoneSound = Mix_LoadWAV("Sons\\pose.wav");
		Mix_PlayChannel(-1, putStoneSound, 0);
	}

	SDL_DestroyTexture(GetTexture());

	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(p_renderer, newSurface);
	SetTexture(newTexture);
}

//Index de la case
void Case::SetIndex(int a_index, int a_tailleGoban) {
	coordinates.first = a_index / a_tailleGoban;
	coordinates.second = a_index % a_tailleGoban;
}

std::pair<int, int> Case::GetIndex() { return coordinates; }

//Position de la case en pixels
void Case::SetPositionInPx(int x, int y) {
	positionInPx.first = x;
	positionInPx.second = y;
}

pair<int, int> Case::GetPositionInPx() { return positionInPx; }

//Etat de la case : vide, noire ou blanche
Etat Case::GetState() { return state; }

Etat Case::GetOppositeState(Etat a_etat) {
	if (a_etat == Etat::Noir) return Etat::Blanc;
	if (a_etat == Etat::Blanc) return Etat::Noir;
}

void Case::SetState(Etat a_etat) { state = a_etat; }

bool Case::BoxIsEmpty() { return state == Etat::Empty; }

bool Case::KilledItself(vector<Case*> a_groupe) { //Si est entourée exclusivement de cases ennemies SAUF s'il va tuer

	vector <vector<Case*>> allEnemisGroups; //Vecteur de vecteurs qui contient sur chaque vecteur, un groupe de pierres ennemies
	vector <Case*> tempVector;

	for (int i = 0; i < a_groupe.size(); i++) {

		vector <Case*> surrounding = a_groupe[i]->GetSurroundings();

		for (int j = 0; j < surrounding.size(); j++)
		{
			if (surrounding[j]->GetState() == GetOppositeState(p_goban->GetPlayerColor()) && (!surrounding[j]->GetHasBeenChecked())) {
				tempVector = StoneGroup(surrounding[j]);

				if (!tempVector.empty()) {
					allEnemisGroups.push_back(tempVector);
					tempVector.clear();
				}
			}
		}
	}

	int nbOfGroupsCaptured = 0;

	for (int i = 0; i < allEnemisGroups.size(); i++)
	{
		for (int j = 0; j < allEnemisGroups[i].size(); j++)
		{
			vector<Case*> enemyGroup = allEnemisGroups[i];

			if (GroupIsCaptured(enemyGroup)) { 	//Si un groupe ennemis n'a pas de libertés, alors ça veut dire qu'on le tue donc on peut poser sa pierre
				nbOfGroupsCaptured++;
				for (int j = 0; j < enemyGroup.size(); j++) enemyGroup[j]->Erase();

				if (nbOfGroupsCaptured == 1 && enemyGroup.size() == 1) p_goban->SetUnavaiableBox(enemyGroup[0]);
			}
		}
	}

	if (nbOfGroupsCaptured > 0)	CaptureEffects();

	if (GroupIsCaptured(a_groupe)) return true;

	return false;
}

vector<Case*> Case::GetSurroundings() {
	vector<Case*> surroundingsBoxes;

	int x = GetIndex().first;
	int y = GetIndex().second;

	int gobanSize = p_goban->GetSize();

	if (x - 1 >= 0)			 surroundingsBoxes.push_back(p_goban->boxes[x - 1][y]);
	if (x + 1 < gobanSize) surroundingsBoxes.push_back(p_goban->boxes[x + 1][y]);
	if (y - 1 >= 0)			 surroundingsBoxes.push_back(p_goban->boxes[x][y - 1]);
	if (y + 1 < gobanSize) surroundingsBoxes.push_back(p_goban->boxes[x][y + 1]);

	return surroundingsBoxes;
}

bool Case::HasFreedoms() {
	vector<Case*> surroundingsBoxes = GetSurroundings();

	for (int i = 0; i < surroundingsBoxes.size(); i++)
		if (surroundingsBoxes[i]->state == Etat::Empty) return true;

	return false;
}

//Si on a déjà fait des vérifications sur cette case
void Case::SetHasBeenChecked(bool a_hasBeenCheckedForGroupe) { hasBeenChecked = a_hasBeenCheckedForGroupe; }
bool Case::GetHasBeenChecked() { return hasBeenChecked; }

//Image qui représente la case
void Case::SetTexture(SDL_Texture* a_caseTexture) {
	int x = GetPositionInPx().first;
	int y = GetPositionInPx().second;

	SDL_Rect dstrect = { x, y, BOX_SIZE, BOX_SIZE };

	boxTexture = a_caseTexture;

	SDL_RenderCopy(p_renderer, boxTexture, NULL, &dstrect);
	SDL_RenderPresent(p_renderer);
}

SDL_Texture* Case::GetTexture() { return boxTexture; }

void Case::Erase() {
	SetState(Etat::Empty);

	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(p_renderer, caseVideSurface);
	SetTexture(newTexture);

	if (p_goban->GetPlayerColor() == Etat::Noir)		 p_goban->AddPinkPoints(1);
	else if (p_goban->GetPlayerColor() == Etat::Blanc)	 p_goban->AddYellowPoints(1);	

	//cout << "noir : " << p_goban->GetPtsNoir() << endl;
	//cout << "blanc : " << p_goban->GetPtsBlanc() << endl;
}

void Case::SetUnavailability(bool a_indispo) { unavaibleForOneTurn = a_indispo; }

bool Case::GetUnavailability() { return unavaibleForOneTurn; }