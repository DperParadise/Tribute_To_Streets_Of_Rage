#ifndef __DESTROYABLE_ITEM__
#define __DESTROYABLE_ITEM__

#include "Entity.h"

struct SDL_Texture;
class Animation;
class DestroyableItem : public Entity{

public:
	DestroyableItem(SDL_Texture *texture, Animation *curr_anim, const char *name, entity_type type, iPoint position, int depth);

public:
	bool m_destroyed;
};


#endif //__DESTROYABLE_ITEM__
