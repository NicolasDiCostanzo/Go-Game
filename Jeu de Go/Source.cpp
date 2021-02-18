#include "Header.h"
#include "Case.h"
#include "Goban.h"
#include "Bouton.h"

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* abandonner_btn_surface = SDL_LoadBMP("Sprites\\Boutons\\abandonner.bmp"), * passer_btn_surface = SDL_LoadBMP("Sprites\\Boutons\\passer.bmp"), * rejouerBouton_surface = SDL_LoadBMP("Sprites\\Boutons\\button.bmp"),
* background = SDL_LoadBMP("Sprites\\Images\\Background.bmp"), * screen = NULL, * logo = SDL_LoadBMP("Sprites\\Images\\logo.bmp"), * yellowWin = SDL_LoadBMP("Sprites\\Images\\Victoire_Jaune.bmp"), 
* pinkWin = SDL_LoadBMP("Sprites\\Images\\Victoire_Rose.bmp"), * yellow = SDL_LoadBMP("Sprites\\Images\\Jaune.bmp"), * pink = SDL_LoadBMP("Sprites\\Images\\Rose.bmp");

TTF_Font *police = NULL;
SDL_Color white = { 255,255,255 };

Mix_Music* music;
Mix_Chunk* crunchSound, * winSound;

SDL_Event event;

Goban* goban;

bool quit = false;

Bouton* givingUp_btn, * pass_btn, * replay_btn;

bool gameIsRunning = true;
bool choiceToMake = true;
bool appIsRunning = true;

vector<Bouton*> choices_btns;

//Prototypes fonctions
vector<int> InitUITaille();
void FreeChoiceScreen(vector<Bouton*> a_boutons);
string Round(float value);

int main(int argc, char** argv)
{
    while (appIsRunning) {
        InitAll();

        int tailleChoisieGoban = 0;

        while (choiceToMake) tailleChoisieGoban = ChoiceSizeGoban_Screen();

        FreeChoiceScreen(choices_btns);

        InitGame(tailleChoisieGoban);

        while (!quit) LoopGame();
    }

    KillAll();

    return 0;
}


//Choice goban size screen UI
vector<int> InitUITaille() {
    police = TTF_OpenFont("Sprites\\Feelin Sweet.ttf", 70);

    vector<int> taillesGoban = { 5, 9, 13, 15, 19 };

    for (int i = 0; i < taillesGoban.size(); i++)
    {
        SDL_Surface* newBoutonSurface = SDL_LoadBMP("Sprites\\Boutons\\button.bmp");

        int marginTop = 300;
        int margeY_entreBoutons = 10;
        int taille = taillesGoban[i];

        Bouton* bouton = new Bouton(renderer, newBoutonSurface, to_string(taille), SCREEN_WIDTH / 2 - newBoutonSurface->w / 2, i * (newBoutonSurface->h + margeY_entreBoutons) + marginTop, newBoutonSurface->w, newBoutonSurface->h);

        choices_btns.push_back(bouton);
    }

    return taillesGoban;
}

int ChoiceSizeGoban_Screen() {
    vector<int> boutonsValues = InitUITaille();

    SDL_WaitEvent(&event);

    for (size_t i = 0; i < boutonsValues.size(); i++)
        if (choices_btns[i]->handleEvent(&event)) {
            choiceToMake = false;
            return boutonsValues[i];
        }
}

void FreeChoiceScreen(vector<Bouton*> a_boutons) {
    SDL_RenderClear(renderer);
    a_boutons.clear();
    quit = false;
}


//Inits

void InitGame(int a_tailleGoban) {
    background = SDL_LoadBMP("Sprites\\Images\\Background.bmp");
    //Affichage tour
    SDL_Rect positionTour;
    positionTour.x = SCREEN_WIDTH / 2;
    positionTour.y = 0;
    SDL_Surface* text = TTF_RenderText_Blended(police, "Au tour de", white);
    SDL_BlitSurface(text, NULL, background, &positionTour);

    CreateBackground(renderer, background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    CreateBackground(renderer, logo, 0, 0, logo->w, logo->h);

    goban = new Goban(renderer, window, a_tailleGoban);

    DisplayPlayerToPlay();

    InitUIGame();

    SDL_FreeSurface(text);
}

void DisplayPlayerToPlay() {
    int marginTop = 10;
    int marginLeft = 30;

    SDL_Surface* playerToPlaySurface;

    if (goban->GetBlackHasToPlay()) playerToPlaySurface = yellow;
    else                           playerToPlaySurface = pink;

    CreateBackground(renderer, playerToPlaySurface, SCREEN_WIDTH / 2 + marginLeft, marginTop, yellow->w / 4, yellow->h / 4);
}

void InitAll() {
    window = SDL_CreateWindow("Jeu de Go",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    InitSDLElements(window);

    police = TTF_OpenFont("Sprites\\Feelin Sweet.ttf", 120);

    SDL_Surface* question = TTF_RenderText_Blended(police, "Choisis la taille de ton goban :", white);

    int marginTop = 40;
    //Position du texte dans son parent
    SDL_Rect positionText;
    positionText.x = SCREEN_WIDTH / 4 + marginTop;
    positionText.y = 150;

    background = SDL_LoadBMP("Sprites\\Images\\Background.bmp");
    SDL_BlitSurface(question, NULL, background, &positionText);
    CreateBackground(renderer, background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    InitMusic();
}

void InitMusic() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        cout << Mix_GetError() << endl;
    }
    else {
        music = Mix_LoadMUS("Sons\\menu.wav");
        Mix_PlayMusic(music, -1);
    }

    // Load sounds
    crunchSound = Mix_LoadWAV("Sons\\crunch.wav");
    winSound = Mix_LoadWAV("Sons\\win.wav");

    if (!crunchSound) cout << "Error loading sound: " << Mix_GetError() << endl;
    if (!winSound) cout << "Error loading sound: " << Mix_GetError() << endl;
}

void InitUIGame() {
    SDL_Rect positionBouton;
    SDL_Rect sizeBouton;

    sizeBouton.x = 300;
    sizeBouton.y = 75;

    int marginLeft = 30;

    //Bouton abandonner
    int positionCaseX = goban->boxes[goban->GetSize() - 1][0]->GetPositionInPx().first;

    SDL_Rect tailleBoutons;
    tailleBoutons.x = 370;
    tailleBoutons.y = 74;

    positionBouton.x = positionCaseX + BOX_SIZE + marginLeft;
    positionBouton.y = SCREEN_HEIGHT / 2 - sizeBouton.y / 2;

    givingUp_btn = new Bouton(renderer, abandonner_btn_surface, "", positionBouton.x, positionBouton.y, tailleBoutons.x, tailleBoutons.y);


    //Bouton passer
    int marginTop = 10;
    positionBouton.y += abandonner_btn_surface->h + marginTop;

    pass_btn = new Bouton(renderer, passer_btn_surface, "", positionBouton.x, positionBouton.y, tailleBoutons.x, tailleBoutons.y);
    
    //Affichage du score
    DisplayScore();
}

void InitSDLElements(SDL_Window* window) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) cout << "Error initializing SDL: " << SDL_GetError() << endl;

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (TTF_Init() == -1)
    {
        cout << "Erreur d'initialisation de TTF_Init :" << TTF_GetError() << endl;
        exit(EXIT_FAILURE);
    }
}


//UI

void DisplayScore() {
    SDL_Surface* scoreBackground = SDL_LoadBMP("Sprites\\Images\\score.bmp");

    int positionCaseY = goban->boxes[0][goban->GetSize() - 1]->GetPositionInPx().second;

    int marginTop = 30;

    SDL_Rect positionSurface;
    positionSurface.x = SCREEN_WIDTH / 2 - scoreBackground->w / 2;
    positionSurface.y = SCREEN_HEIGHT - scoreBackground->h - marginTop;


    string scoreWhite = Round(goban->GetPinkPoints());;
    string scoreBlack = to_string(goban->GetYellowPoints());

    string str = scoreWhite.append("|").append(scoreBlack);
    const char* c = &str[0];

    SDL_Surface* textScore = TTF_RenderText_Blended(police, c, white);

    //Position du texte dans son parent
    SDL_Rect positionText;
    positionText.x = scoreBackground->w / 2 - textScore->w / 2;
    positionText.y = scoreBackground->h / 2 - textScore->h / 2;

    SDL_Rect scoreSize;
    scoreSize.x = 430;
    scoreSize.y = 86;

    SDL_BlitSurface(textScore, NULL, scoreBackground, &positionText);
    CreateBackground(renderer, scoreBackground, positionSurface.x, positionSurface.y, scoreSize.x, scoreSize.y);

    SDL_FreeSurface(textScore);
}

void CreateBackground(SDL_Renderer* a_renderer, SDL_Surface* a_background, int a_x, int a_y, int a_sizeX, int a_sizeY) {
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(a_renderer, a_background);
    SDL_Rect dstrect = { a_x, a_y, a_sizeX, a_sizeY };
    SDL_RenderCopy(a_renderer, newTexture, NULL, &dstrect);
    SDL_RenderPresent(a_renderer);
}

void DisplayWinner() {

    SDL_Surface* winnerImg;

    if (goban->GetPinkPoints() > goban->GetYellowPoints()) winnerImg = yellowWin;
    else winnerImg = pinkWin;

    //Taille encadré
    SDL_Rect taille;
    taille.x = winnerImg->w / 2;
    taille.y = winnerImg->h / 2;

    //Position de l'encadré
    SDL_Rect pos;
    int marginTop = 50;
    int marginRight = 50;
    pos.x = SCREEN_WIDTH - winnerImg->w / 2 - marginTop;
    pos.y = marginRight;

    CreateBackground(renderer, winnerImg, pos.x, pos.y, taille.x, taille.y);

    int marginBottom = 30;
    replay_btn = new Bouton(renderer, rejouerBouton_surface, "Rejouer", pos.x + (taille.x / 2 - rejouerBouton_surface->w / 4), pos.y + taille.y - rejouerBouton_surface->h / 2 - marginBottom, rejouerBouton_surface->w / 2, rejouerBouton_surface->h / 2);
}


//Gameplay

void LoopGame() {
    SDL_WaitEvent(&event);

    if (event.type == SDL_QUIT) {
        quit = true;
        appIsRunning = false;
    }

    if (gameIsRunning) {
        int tailleGoban = goban->GetSize();

        for (int i = 0; i < tailleGoban; i++)
            for (int j = 0; j < tailleGoban; j++)
                goban->boxes[i][j]->handleEvent(&event);

        if (givingUp_btn->handleEvent(&event))  goban->GivingUp();
        else if (pass_btn->handleEvent(&event)) goban->PassTurn();
    }
    else {
        if (replay_btn->handleEvent(&event)) Replay();
    }

}

void CaptureEffects() {
    Mix_PlayChannel(-1, crunchSound, 0);

    DisplayScore();
    goban->SetIfCaptured(false);
}

string Round(float value) { 

    string result = to_string(((float)((int)(value * 10))) / 10);

    vector<char> tempVector;

    bool chiffresApresPoint = false;

    for (int i = 0; i < result.size(); i++) {
        tempVector.push_back(result[i]);

        if (result[i] == '.') chiffresApresPoint = true;
        else if (chiffresApresPoint) break;
    }

    string stringOut(tempVector.begin(), tempVector.end());

    return stringOut;
}

bool GroupIsCaptured(vector<Case*> a_groupe) {
    for (int i = 0; i < a_groupe.size(); i++)
        if (a_groupe[i]->HasFreedoms()) return false;

    return true;
}


//Constitue le groupe liée à la pierre envoyée en paramètre
vector<Case*> StoneGroup(Case* a_case) {

    vector<Case*> tempVector; //Stocke pierres alliées qui n'ont pas été checkées
    vector<Case*> casesAutour = a_case->GetSurroundings();//Récupère les cases autour de la pierre passée en paramètre

    tempVector.push_back(a_case);
    a_case->SetHasBeenChecked(true);

    for (int i = 0; i < casesAutour.size(); i++) //On parcourt les cases autour de la case passée en paramètre
    {
        Case* newCase = casesAutour[i];

        if (newCase->GetState() == a_case->GetState() && newCase->GetHasBeenChecked() == false) { //Si la case est alliée de cette passée en paramètre ET qu'elle n'a pas été checkée...
            newCase->SetHasBeenChecked(true);													//...on indique qu'elle a été checkée...
            vector <Case*> tempVector2 = StoneGroup(newCase);								//...on constitue depuis cette pierre...
            tempVector.insert(end(tempVector), begin(tempVector2), end(tempVector2));			//...on ajoute le groupe de cette pierre au groupe de pierre déjà existant.
        }
    }

    return tempVector; //Retourne le groupe constitué
}

Etat CheckGroupSurroundingsStatus(vector<Case*> a_groupeDeCases) {

    Etat etatEntourageGroupe = Etat::Empty;

    for (int i = 0; i < a_groupeDeCases.size(); i++)
    {
        Case* newCase = a_groupeDeCases[i];

        vector<Case*> entourageNewCase = newCase->GetSurroundings();

        for (int j = 0; j < entourageNewCase.size() ; j++)
        {
            Case* newCaseEntourage = entourageNewCase[j];

            if (newCaseEntourage->GetState() != Etat::Empty) {
                if (etatEntourageGroupe == Etat::Empty)
                    etatEntourageGroupe = newCaseEntourage->GetState();
                else if (etatEntourageGroupe != Etat::Empty && newCaseEntourage->GetState() != etatEntourageGroupe) 
                    return Etat::Empty;
            }
        }
    }

    return etatEntourageGroupe;
}


//Free functions

void FreeAllSurfaces() {
    SDL_FreeSurface(rejouerBouton_surface);
    SDL_FreeSurface(abandonner_btn_surface);
    SDL_FreeSurface(passer_btn_surface);
    SDL_FreeSurface(background);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(logo);
    SDL_FreeSurface(yellowWin);
    SDL_FreeSurface(pinkWin);
    SDL_FreeSurface(yellow);
    SDL_FreeSurface(pink);
}

void FreeSounds() {
    Mix_FreeChunk(crunchSound);
    Mix_FreeChunk(winSound);
    Mix_FreeMusic(music);
}

void KillAll() {
    FreeAllSurfaces();
    FreeSounds();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(police);

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}


//End of the game

void PointsCounting() {
    goban->ResetHasBeenCheckedParameter();
    int tailleGoban = goban->GetSize();

    vector<Case*> tempVector;

    for (int i = 0; i < tailleGoban; i++)
    {
        for (int j = 0; j < tailleGoban; j++)
        {
            Case* newCase = goban->boxes[i][j];

            if (newCase->GetState() == Etat::Empty && !newCase->GetHasBeenChecked()) {
                tempVector = StoneGroup(newCase);
                DisplayState(CheckGroupSurroundingsStatus(tempVector));

                Etat etatEntourageGroupe = CheckGroupSurroundingsStatus(tempVector);

                if      (etatEntourageGroupe == Etat::Noir)  goban->AddPinkPoints(tempVector.size());
                else if (etatEntourageGroupe == Etat::Blanc) goban->AddYellowPoints(tempVector.size());
            }
        }
        DisplayScore();
    }
}

void Replay() {
    SDL_RenderClear(renderer);
    choiceToMake = true;
    quit = true;
    gameIsRunning = true;
    goban->~Goban();
    SDL_DestroyWindow(window);
}

void GameOver() {
    gameIsRunning = false;

    PointsCounting();

    Mix_HaltMusic();
    Mix_VolumeChunk(winSound, 40);
    Mix_PlayChannel(-1, winSound, 0);
    DisplayWinner();
}


//Fonctions de debug :
void DisplayVector(vector<Case*> a_vecteur) {
    for (int i = 0; i < a_vecteur.size(); i++)
        DisplayCoordinates(a_vecteur[i]);

    cout << endl << endl;
}

void DisplayCoordinates(Case* _case) { cout << "x : " << _case->GetIndex().first << " y : " << _case->GetIndex().second << endl; }

void DisplayState(Etat a_etat) {
    if (a_etat == Etat::Empty)
        cout << "Vide ";
    else if (a_etat == Etat::Noir)
        cout << "Noir ";
    else if (a_etat == Etat::Blanc)
        cout << "Blanc ";
}


