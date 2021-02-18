#include "Header.h"
#include "Goban.h"
#include "Case.h"
#include "Fonctions.h"

class Case;

Goban::Goban(SDL_Renderer* renderer, SDL_Window* window, int gobanSize) {
	int index = 0;

	//Fond goban
	SDL_Surface* goban_background = SDL_LoadBMP("Sprites\\Images\\background_goban.bmp");

	int margin = 20;
	int size = BOX_SIZE * gobanSize + margin;

	int posX = (SCREEN_WIDTH / 2) - (BOX_SIZE / 2 * gobanSize) - margin / 2;
	int posY = (SCREEN_HEIGHT / 2) - (BOX_SIZE / 2 * gobanSize) + margin;

	SDL_Surface* empty = SDL_LoadBMP("Sprites\\Images\\case.bmp");
	SDL_Surface* noir = SDL_LoadBMP("Sprites\\Images\\noir.bmp");
	SDL_Surface* blanc = SDL_LoadBMP("Sprites\\Images\\blanc.bmp");


	SDL_Texture* emptyTexture = SDL_CreateTextureFromSurface(renderer, empty);
	SDL_Texture* noirTexture = SDL_CreateTextureFromSurface(renderer, noir);
	SDL_Texture* blancTexture = SDL_CreateTextureFromSurface(renderer, blanc);



	CreateBackground(renderer, goban_background, posX, posY, size, size);

	for (int i = 0; i < gobanSize; i++) {
		vector<Case*> line;
		boxes.push_back(line);

		for (int j = 0; j < gobanSize; j++) {
			Case* newCase = new Case(this, renderer, window, emptyTexture, index, gobanSize);
			boxes[i].push_back(newCase);
			index++;
		}
	}		

	SetSize(gobanSize);
}

Goban::~Goban() { boxes.clear(); }

int Goban::GetSize() { return size; }
void Goban::SetSize(int a_taille) { size = a_taille; }

bool Goban::GetIfCaptured() { return isCaptured; }
void Goban::SetIfCaptured(bool a_capture) { isCaptured = a_capture; }

bool Goban::GetBlackHasToPlay() { return noirDoitJouer; }
void Goban::SetPlayerTurn() { 
	noirDoitJouer = !noirDoitJouer;

	DisplayPlayerToPlay();
}

Etat Goban::GetPlayerColor() {
	if (GetBlackHasToPlay())       return Etat::Noir;
	else if(!GetBlackHasToPlay())  return Etat::Blanc;
}

int Goban::GetYellowPoints() { return yellowPoints; }
void Goban::AddYellowPoints(int a_pointsToAdd) { yellowPoints+= a_pointsToAdd; }

float Goban::GetPinkPoints() { return pinkPoints; }
void Goban::AddPinkPoints(int a_pointsToAdd) { pinkPoints+= a_pointsToAdd; }

void Goban::ResetHasBeenCheckedParameter() {
	for (int i = 0; i < GetSize(); i++)
		for (int j = 0; j < GetSize(); j++)
			if (boxes[i][j]->GetHasBeenChecked()) boxes[i][j]->SetHasBeenChecked(false);
}

void Goban::SetUnavaiableBox(Case* a_case) { 
	boxNotAvaiable = a_case;
}

Case* Goban::GetUnavaiableBox() {
	return boxNotAvaiable;
}

void Goban::SetUnavaibleTurn(bool a_tourIndispo) {
	unavaibleTurn = a_tourIndispo;
}

bool Goban::GetUnavaibleTurn() {
	return unavaibleTurn;
}

void Goban::ResetUnavaibleBox() { SetUnavaiableBox(NULL); }

void Goban::PassTurn() {

	if (GetPlayerColor() == Etat::Noir) AddYellowPoints(1);
	else								AddPinkPoints(1);

	DisplayScore();

	SetPlayerTurn();

	if (GetTurnPassed())	GameOver();
	else SetTurnPassed(true);
}

void Goban::SetTurnPassed(bool a_tourPasse) { turnedPass = a_tourPasse; }
bool Goban::GetTurnPassed() { return turnedPass; }
void Goban::GivingUp() { GameOver(); }

