#pragma once
#include "Header.h"

class Case;
class Bouton;

int ChoiceSizeGoban_Screen();

void InitSDLElements(SDL_Window* window);
void InitMusic();
void InitUIGame();
void InitGame(int a_tailleGoban);
void InitAll();

void LoopGame();
void DisplayPlayerToPlay();
void CreateBackground(SDL_Renderer* a_renderer, SDL_Surface* a_bg, int a_x, int a_y, int a_sizeX, int a_sizeY);
void DisplayScore();

void CaptureEffects();
void Replay();
void GameOver();
void DisplayWinner();

void KillAll();