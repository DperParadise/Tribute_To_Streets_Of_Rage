#ifndef __GARCIAKNIFE_H__
#define __GARCIAKNIFE_H__

#include "Entity.h"
#include "Enemy.h"

class GarciaKnife : public Enemy
{
public:
	GarciaKnife(SDL_Texture *texture, Animation *curr_anim, const char *name, entity_type type, iPoint position, int depth);
	GarciaKnife(const GarciaKnife &garcia_knife);
	~GarciaKnife();



public:
	
	Animation m_npc_garcia_knife_idle_right;
	Animation m_npc_garcia_knife_idle_left;

	Animation m_npc_garcia_knife_walk_right;
	Animation m_npc_garcia_knife_walk_left;

	Animation m_npc_garcia_knife_attack_right;
	Animation m_npc_garcia_knife_attack_left;

private:
	std::list<int*> m_animation_list;
};


#endif


