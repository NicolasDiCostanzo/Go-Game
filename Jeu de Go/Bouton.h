#pragma once
#include "Header.h"

using namespace std;
class Bouton
{
protected:
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Rect position;
	SDL_Rect size;


public :
	Bouton(SDL_Renderer* a_renderer, SDL_Surface* a_bouton, string a_texte, int a_x, int a_y, int a_sizeX, int a_sizeY);
	~Bouton();

	bool handleEvent(SDL_Event* e);

	void SetSurface(SDL_Surface* a_surface);
	SDL_Surface* GetSurface();

	void SetPosition(int a_x, int a_y);
	SDL_Rect GetPosition();

	void SetSize(int a_x, int a_y);
	SDL_Rect GetSize();

};

