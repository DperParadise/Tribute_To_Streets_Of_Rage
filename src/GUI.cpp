#include "GUI.h"

GUI::GUI(
	SDL_Texture *texture,
	Animation *curr_anim,
	const char *name, 
	entity_type type, 
	iPoint position, 
	int depth) : Entity(texture, curr_anim, name, type, position, depth){}

GUI::~GUI() {}