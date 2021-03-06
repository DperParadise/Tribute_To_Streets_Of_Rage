#include "GarciaFSM.h"
#include "Garcia.h"
#include "Player.h"
#include "Point.h"
#include "Application.h"
#include "ModuleSceneRound1.h"
#include "ModuleSceneRound1.h"
#include "HealthBar.h"
#include "ModuleAudio.h"

GarciaFSM::GarciaFSM(Garcia *grc) : garcia(grc) 
{
	prev_state = State::START;
	curr_state = State::IDLE;
	
}

GarciaFSM::~GarciaFSM() {}

void GarciaFSM::Update()
{
	switch (curr_state)
	{
	case State::IDLE:
		Idle();
		prev_state = curr_state;

		if (garcia->attack)
		{
			curr_state = State::ATTACK;
			break;
		}
		if (garcia->grabbed)
		{
			garcia->attack = false;
			curr_state = State::GRABBED;
			garcia->start_pos = garcia->position;
			break;
		}
		if (garcia->damaged)
		{
			if (garcia->life > 0)
				curr_state = State::DAMAGED;
			else
			{
				garcia->knocked_down = true;
				curr_state = State::KNOCKED_DOWN;
			}
				
			break;
		}
		if (garcia->knocked_down)
		{
			curr_state = State::KNOCKED_DOWN;
			break;
		}
		if (garcia->player_in_sight)
		{
			curr_state = State::CHASE;
			break;
		}
		break;

	case State::CHASE:
		Chase();
		if (garcia->attack)
		{
			curr_state = State::IDLE;
			break;
		}
		if (garcia->grabbed)
		{
			garcia->attack = false;
			curr_state = State::GRABBED;
			garcia->start_pos = garcia->position;
			break;
		}
		if (garcia->damaged)
		{
			if (garcia->life > 0)
				curr_state = State::DAMAGED;
			else
			{
				garcia->knocked_down = true;
				curr_state = State::KNOCKED_DOWN;
			}
			break;
		}
		if (garcia->knocked_down)
		{
			curr_state = State::KNOCKED_DOWN;
			break;
		}
		break;

	case State::ATTACK:			
		Attack();
		
		if (garcia->attack == false && garcia->punch_finished)
		{
			curr_state = State::IDLE;
			garcia->punch_hits = 0;
			garcia->punch_finished = false;
			break;
		}
		if (garcia->evasive)
		{
			garcia->evasive_started_facing_right = garcia->facing_right;
			if ((garcia->position.y - garcia->evasive_v_offset) <= App->scene_round1->upper_limit)
			{
				evasion_upper = false;
				evasion_lower = true;
			}
			else if ((garcia->position.y + garcia->evasive_v_offset) >= App->scene_round1->lower_limit)
			{
				evasion_upper = true;
				evasion_lower = false;
			}
			else
			{
				evasion_upper = true;
				evasion_lower = false;
			}
			curr_state = State::EVASIVE;
			break;
		}
		if (garcia->grabbed)
		{
			garcia->attack = false;
			garcia->punch_hits = 0;
			curr_state = State::GRABBED;
			garcia->start_pos = garcia->position;
			break;
		}
		if (garcia->damaged)
		{	
			garcia->punch_hits = 0;
			garcia->attack = false;

			if (garcia->life > 0)
				curr_state = State::DAMAGED;
			else
			{
				garcia->knocked_down = true;
				curr_state = State::KNOCKED_DOWN;
			}
			break;
		}
		if (garcia->knocked_down)
		{
			curr_state = State::KNOCKED_DOWN;
			break;
		}
		break;

	case State::EVASIVE:
		Evasive();
		if (garcia->evasive == false)
		{
			curr_state = State::IDLE;
		}
		if (garcia->damaged)
		{
			garcia->evasive = false;
			evasive_v_count = 0;
			evasive_h_count = 0;

			if (garcia->life > 0)
				curr_state = State::DAMAGED;
			else
			{
				garcia->knocked_down = true;
				curr_state = State::KNOCKED_DOWN;
			}
			break;
		}
		if (garcia->knocked_down)
		{
			curr_state = State::KNOCKED_DOWN;
			break;
		}
		if (garcia->grabbed)
		{		
			garcia->evasive = false;
			evasive_v_count = 0;
			evasive_h_count = 0;
			curr_state = State::GRABBED;
			garcia->start_pos = garcia->position;
			break;
		}
		break;

	case State::DAMAGED:
		Damaged();
		if (garcia->damaged == false)
		{
			if (prev_state != State::GRABBED)
				curr_state = State::IDLE;
			else
				curr_state = State::GRABBED;
		}
		break;

	case State::KNOCKED_DOWN:
		KnockedDown();
		prev_state = curr_state;
		if (garcia->knocked_down == false)
		{
			if (garcia->life > 0)
				curr_state = State::IDLE;
			else			
				curr_state = State::DEAD;							
		}
		break;

	case State::GRABBED:
		Grabbed();
		if (!garcia->grabbed)
		{
			if (garcia->facing_right)
			{
				garcia->curr_anim = &(garcia->garcia_idle_right);
			}
			else
			{
				garcia->curr_anim = &(garcia->garcia_idle_left);
			}
			garcia->frames_counter++;
			if (garcia->frames_counter >= garcia->num_frames)
			{
				garcia->frames_counter = 0;
				curr_state = State::IDLE;
			}
		}
		if (garcia->damaged)
		{
			if (garcia->life > 0)
				curr_state = State::DAMAGED;
			else
			{
				garcia->knocked_down = true;
				curr_state = State::KNOCKED_DOWN;
			}

			prev_state = State::GRABBED;
			break;
		}
		if (garcia->knocked_down)
		{
			curr_state = State::KNOCKED_DOWN;
			break;
		}
		break;

	case State::DEAD:
		Dead();
		break;
	}
}

//-----------------------------  ACTIONS TO PERFORM IN EACH STATE ------------------------------------
void GarciaFSM::Idle()
{
	if(garcia->facing_right)
		garcia->curr_anim = &(garcia->garcia_idle_right);
	if (!garcia->facing_right)
		garcia->curr_anim = &(garcia->garcia_idle_left);
}
void GarciaFSM::Chase()
{
	iPoint target;

	target.x = garcia->the_player->position.x - garcia->position.x;
	target.y = garcia->the_player->depth - garcia->position.y;
		
	iPoint direction{ 0,0 };
	
	if (target.x > 0)
	{
		direction.x = 1;
		if (!garcia->facing_right)
			garcia->facing_right = true;	
	}

	if(target.x < 0)
	{
		direction.x = -1;
		if (garcia->facing_right)
			garcia->facing_right = false;	
	}

	if (target.y > 0)	
		direction.y = 1;		
	
	if (target.y < 0)
		direction.y = -1;		
	

	int distance_x = target.x > 0 ? target.x : -target.x;
	int distance_y = target.y > 0 ? target.y : -target.y;

	if (garcia->facing_right && garcia->the_player->facing_right || !garcia->facing_right && !garcia->the_player->facing_right)
	{
		if (distance_x < garcia->speed_vect.x)
		{
			garcia->speed_vect.x = 1;
			garcia->garcia_walk_left.speed = 0.1f;
			garcia->garcia_walk_right.speed = 0.1f;
		}
		else
		{
			garcia->speed_vect.x = 2;
			garcia->garcia_walk_left.speed = 0.2f;
			garcia->garcia_walk_right.speed = 0.2f;
		}

		if (distance_y < garcia->speed_vect.y)
		{
			garcia->speed_vect.y = 1;
			garcia->garcia_walk_left.speed = 0.1f;
			garcia->garcia_walk_right.speed = 0.1f;
		}
		else
		{
			garcia->speed_vect.y = 2;
			garcia->garcia_walk_left.speed = 0.2f;
			garcia->garcia_walk_right.speed = 0.2f;
		}
	}
	else
	{
		garcia->speed_vect = { 1,1 };
	}
	garcia->position += {direction.x * garcia->speed_vect.x , direction.y * garcia->speed_vect.y};
	garcia->depth = garcia->position.y;
	UpdateColliderPosition();

	//set animations
		
	if (!garcia->facing_right)
	{
		if (garcia->curr_anim != &(garcia->garcia_walk_left))
		{
			garcia->curr_anim = &(garcia->garcia_walk_left);
			garcia->curr_anim->Reset();
		}
	}
			
	if (garcia->facing_right)
	{
		if (garcia->curr_anim != &(garcia->garcia_walk_right))
		{
			garcia->curr_anim = &(garcia->garcia_walk_right);
			garcia->curr_anim->Reset();
		}		
	}
}
	
void GarciaFSM::Attack()
{
	if (garcia->punch_hits < 2 && !punch_wait)
	{	
		if(garcia->facing_right)
			garcia->curr_anim = &garcia->garcia_punch_right1;
		else
			garcia->curr_anim = &garcia->garcia_punch_left1;

		if (garcia->curr_anim->Finished())
		{
			garcia->curr_anim->Reset();

			if (garcia->attack)
			{
				garcia->the_player->damaged = true;	//Damage the player	
				App->audio->PlayFx(audio_fx::ENEMY_ATTACK);
				if (garcia->the_player->god_mode == false)
				{
					garcia->the_player->life -= garcia->punch_damage;
					App->scene_round1->player_HP->ScaleHPBar(garcia->the_player->life, garcia->the_player->max_life);
				}
				garcia->the_player->enemy_attacker = garcia;	//enemy to react to					
				garcia->punch_hits++;
			}
			punch_wait = true;
		}		
	}
	if (garcia->punch_hits == 2 && !punch_wait)
	{	
		if(garcia->facing_right)
			garcia->curr_anim = &garcia->garcia_punch_right2;
		else
			garcia->curr_anim = &garcia->garcia_punch_left2;

		if (garcia->curr_anim->Finished())
		{
			garcia->curr_anim->Reset();
			if (garcia->attack)
			{
				garcia->the_player->knocked_down = true;	//Knock down the player
				App->audio->PlayFx(audio_fx::ENEMY_ATTACK);
				if (garcia->the_player->god_mode == false)
				{					
					garcia->the_player->life -= garcia->punch_damage;
					App->scene_round1->player_HP->ScaleHPBar(garcia->the_player->life, garcia->the_player->max_life);
				}
				garcia->the_player->enemy_attacker = garcia;	//enemy to react to
				garcia->punch_hits = 0;
				garcia->evasive = true;
			}
			punch_wait = true;
		}			
	}
	if (punch_wait)
	{
		if (garcia->facing_right)
			garcia->curr_anim = &garcia->garcia_idle_right;
		else
			garcia->curr_anim = &garcia->garcia_idle_left;

		if (garcia->curr_anim->Finished())
		{
			garcia->curr_anim->Reset();
			punch_wait = false;
			garcia->punch_finished = true;
		}
	}
}

void GarciaFSM::Evasive()
{
	switch (evasive_movement)
	{
	case EvasiveMovement::FIRST_STAGE:
		EvasiveFirstStage();
		break;

	case EvasiveMovement::SECOND_STAGE:
		EvasiveSecondStage();
		break;

	case EvasiveMovement::THIRD_STAGE:
		EvasiveThirdStage();
		break;
	}
}

void GarciaFSM::EvasiveFirstStage()
{
	if (evasive_v_count < garcia->evasive_v_offset)
	{
		if (evasive_go)
		{
			if (evasion_upper)
			{
				iPoint temp = garcia->position;
				temp.y -= garcia->speed;
				garcia->position = temp;
				garcia->depth = temp.y;
				evasive_v_count++;
			}
			if (evasion_lower)
			{
				iPoint temp = garcia->position;
				temp.y += garcia->speed;
				garcia->position = temp;
				garcia->depth = temp.y;
				evasive_v_count++;
			}
		}
		if (evasive_back)
		{
			if (evasion_upper)
			{
				iPoint temp = garcia->position;
				temp.y += garcia->speed;
				garcia->position = temp;
				garcia->depth = temp.y;
				evasive_v_count++;
			}
			if (evasion_lower)
			{
				iPoint temp = garcia->position;
				temp.y -= garcia->speed;
				garcia->position = temp;
				garcia->depth = temp.y;
				evasive_v_count++;
			}
		}

		//set animation
		if (garcia->facing_right)
		{
			if (garcia->curr_anim != &garcia->garcia_walk_right)
			{
				garcia->curr_anim = &garcia->garcia_walk_right;
				garcia->curr_anim->Reset();
			}
		}
		else
		{
			if (garcia->curr_anim != &garcia->garcia_walk_left)
			{
				garcia->curr_anim = &garcia->garcia_walk_left;
				garcia->curr_anim->Reset();
			}
		}
	}
	else
	{
		evasive_v_count = 0;

		if(evasive_go)
			evasive_movement = EvasiveMovement::SECOND_STAGE;
		if (evasive_back)
		{
			curr_state = State::CHASE;
			evasive_back = false;
			evasive_go = true;
			garcia->evasive = false;
		}
	}

	UpdateColliderPosition();
}

void GarciaFSM::EvasiveSecondStage()
{
	if (evasive_h_count < garcia->evasive_h_offset)
	{
		if (evasive_go)
		{
			if (garcia->evasive_started_facing_right)
			{
				iPoint temp = garcia->position;
				temp.x += garcia->speed;
				garcia->position = temp;
				evasive_h_count++;
			}
			else 
			{
				iPoint temp = garcia->position;
				temp.x -= garcia->speed;
				garcia->position = temp;				
				evasive_h_count++;
			}
		}
		else if (evasive_back)
		{
			if (garcia->evasive_started_facing_right)
			{
				iPoint temp = garcia->position;
				temp.x -= garcia->speed;
				garcia->position = temp;
				evasive_h_count++;
			}
			else
			{
				iPoint temp = garcia->position;
				temp.x += garcia->speed;
				garcia->position = temp;
				evasive_h_count++;
			}
		}

		//set enemy facing right or left
		int target_x = garcia->the_player->position.x - garcia->position.x;

		if (target_x >= 0)
			garcia->facing_right = true;
		else
			garcia->facing_right = false;

		//set animation
		if (garcia->facing_right)
		{
			if (garcia->curr_anim != &garcia->garcia_walk_right)
			{
				garcia->curr_anim = &garcia->garcia_walk_right;
				garcia->curr_anim->Reset();
			}
		}
		else
		{
			if (garcia->curr_anim != &garcia->garcia_walk_left)
			{
				garcia->curr_anim = &garcia->garcia_walk_left;
				garcia->curr_anim->Reset();
			}
		}
	}
	else
	{
		evasive_h_count = 0;
		if (evasive_go)
			evasive_movement = EvasiveMovement::THIRD_STAGE;
		if (evasive_back)
			evasive_movement = EvasiveMovement::FIRST_STAGE;
	}

	UpdateColliderPosition();
}

void GarciaFSM::EvasiveThirdStage()
{
	if (evasive_v_count < garcia->evasive_v_offset)
	{
		if (evasive_go)
		{
			if (garcia->evasive_started_facing_right)
				garcia->position.x -= garcia->speed;
			else
				garcia->position.x += garcia->speed;

			if(evasion_upper)
				garcia->position.y += garcia->speed;
			if(evasion_lower)
				garcia->position.y -= garcia->speed;

			evasive_v_count++;
		}
		if (evasive_back)
		{
			if (garcia->evasive_started_facing_right)
				garcia->position.x += garcia->speed;
			else
				garcia->position.x -= garcia->speed;

			if(evasion_upper)
				garcia->position.y -= garcia->speed;
			if(evasion_lower)
				garcia->position.y += garcia->speed;

			evasive_v_count++;
		}
	}
	else
	{
		if (evasive_go)
		{
			evasive_go = false;
			evasive_back = true;
		}
		else if (evasive_back)
		{
			evasive_movement = EvasiveMovement::SECOND_STAGE;
		}
		
		evasive_v_count = 0;
	}

	UpdateColliderPosition();
}

void GarciaFSM::Grabbed()
{
	switch (grab_stage)
	{
	case GrabStage::FIRST_STAGE:
		GrabbedFirstStage();
		break;
	case GrabStage::SECOND_STAGE:
		GrabbedSecondStage();
		break;
	case GrabStage::THIRD_STAGE:
		GrabbedThirdStage();
		break;
	case GrabStage::FOURTH_STAGE:
		GrabbedFourthStage();
		break;
	case GrabStage::FIFTH_STAGE:
		GrabbedFifthStage();
		break;
	case GrabStage::SIXTH_STAGE:
		GrabbedSixthStage();
		break;
	case GrabStage::SEVENTH_DOWN_STAGE:
		GrabbedSeventhDownStage();
		break;
	}	
}

void GarciaFSM::GrabbedFirstStage()
{
	if (garcia->facing_right)
	{
		garcia->curr_anim = &garcia->garcia_grabbed_right;

		if (from_second_stage)
		{
			garcia->position.x -= garcia->offset_right_2.x;
			from_second_stage = false;
		}
	}
	else
		garcia->curr_anim = &garcia->garcia_grabbed_left;	
}

void GarciaFSM::GrabbedSecondStage()
{
	if (garcia->facing_right)
	{
		if (garcia->curr_anim != &garcia->garcia_grabbed_finisher_right)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_right;
			garcia->position.x += garcia->offset_right_2.x;
			from_second_stage = true;
		}
	}
	else
		garcia->curr_anim = &garcia->garcia_grabbed_finisher_left;
	
}

void GarciaFSM::GrabbedThirdStage()
{
	if (garcia->facing_right)
	{
		if (garcia->offset_applied == false)
		{			
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_right;
			garcia->position.x -= garcia->offset_right_3.x;
			garcia->offset_applied = true;
		}
	}
	else
	{
		if (garcia->offset_applied == false)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_left;
			garcia->position.x += garcia->offset_left_3.x;
			garcia->offset_applied = true;
		}
	}
}

void GarciaFSM::GrabbedFourthStage()
{
	garcia->offset_applied = false; //reset flag

	if (garcia->facing_right)
	{
		if (garcia->curr_anim != &garcia->garcia_grabbed_finisher_horiz_right)
		{
			garcia->position.x -= garcia->offset_right_4.x;
			garcia->position.y -= garcia->offset_right_4.y;
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_horiz_right;
		}
	}
	else
	{
		if (garcia->curr_anim != &garcia->garcia_grabbed_finisher_horiz_left)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_horiz_left;
			garcia->position.x += garcia->offset_left_4.x;
			garcia->position.y -= garcia->offset_left_4.y;
		}
	}
}

void GarciaFSM::GrabbedFifthStage()
{

	if (garcia->facing_right)
	{
		if (garcia->offset_applied == false)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_vert_right;
			garcia->position.x += garcia->offset_right_5.x;
			garcia->position.y -= garcia->offset_right_5.y;
			garcia->offset_applied = true;
		}
	}
	else
	{
		if (garcia->offset_applied == false)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_vert_left;
			garcia->position.x += garcia->offset_left_5.x;
			garcia->position.y -= garcia->offset_left_5.y;
			garcia->offset_applied = true;
		}
	}
}

void GarciaFSM::GrabbedSixthStage()
{
	if (garcia->offset_applied)
	{
		//reset flag and animation
		garcia->offset_applied = false;
		garcia->curr_anim->Reset();
	}
	
	if (garcia->facing_right)
	{
		if (garcia->offset_applied_2 == false)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_vert_right;
			garcia->position.x -= garcia->offset_right_6.x;
			garcia->position.y += garcia->offset_right_6.y;
			garcia->offset_applied_2 = true;
		}
	}
	else
	{
		if (garcia->offset_applied_2 == false)
		{
			garcia->curr_anim = &garcia->garcia_grabbed_finisher_vert_left;
			garcia->position.x += garcia->offset_left_6.x;
			garcia->position.y += garcia->offset_left_6.y;
			garcia->offset_applied_2 = true;
		}
	}

	if (garcia->curr_anim->Finished())
	{
		grab_stage = GrabStage::SEVENTH_DOWN_STAGE;		
		garcia->offset_applied_2 = false;
	}
}

void GarciaFSM::GrabbedSeventhDownStage()
{
	App->audio->PlayFx(audio_fx::GROUND_HIT);
	
	if (garcia->facing_right)
	{
		garcia->curr_anim = &garcia->garcia_down_right2;
		garcia->position.x -= garcia->offset_right_7.x;
		garcia->position.y = garcia->start_pos.y;	
	}
	else
	{
		garcia->curr_anim = &garcia->garcia_down_left2;
		garcia->position.x -= garcia->offset_left_7.x;
		garcia->position.y = garcia->start_pos.y;		
	}	
		garcia->knocked_down = true;		
}

void GarciaFSM::Damaged()
{
	if (garcia->facing_right)
		garcia->curr_anim = &garcia->garcia_damaged_right;
	else
		garcia->curr_anim = &garcia->garcia_damaged_left;

	if (garcia->curr_anim->Finished())
	{
		garcia->curr_anim->Reset();
		garcia->damaged = false;
	}
}

void GarciaFSM::KnockedDown()
{
	if (prev_state != State::KNOCKED_DOWN)
	{
		garcia->pos_before_knockdown = garcia->position;

		if (garcia->facing_right)
			garcia->curr_anim = &(garcia->garcia_down_right1);

		if (!garcia->facing_right)
			garcia->curr_anim = &(garcia->garcia_down_left1);

		if (garcia->jump_kick_damage)
		{
			App->audio->PlayFx(audio_fx::PLAYER_ATTACK_HIT);
			garcia->jump_kick_damage = false;
		}

		if (garcia->life <= 0)
		{
			App->audio->PlayFx(audio_fx::ENEMY_DEAD);
		}
	}
	else
	{
		//position offset
		iPoint temp = garcia->position;

		garcia->down_count++;
		if (garcia->down_count < garcia->down_frames)
		{
			if (garcia->facing_right)
			{
				if (garcia->down_count <= garcia->down_inflection)
				{
					temp.x -= 2 * garcia->speed;
					temp.y -= garcia->speed;
				}
				else
				{
					temp.x -= 2 * garcia->speed;
					temp.y += garcia->speed;
				}
			}
			if (!garcia->facing_right)
			{
				if (garcia->down_count <= garcia->down_inflection)
				{
					temp.x += 2 * garcia->speed;
					temp.y -= garcia->speed;
				}
				else
				{
					temp.x += 2 * garcia->speed;
					temp.y += garcia->speed;
				}
			}

			garcia->position = temp;
			UpdateColliderPosition();
		}
		if (garcia->down_count == garcia->down_frames)
		{
			garcia->up = true;
			garcia->curr_anim->Reset();			
		}

		if (garcia->up)
		{
			//right animations
			if (garcia->curr_anim == &(garcia->garcia_down_right1))
			{
				garcia->curr_anim->Reset();
				garcia->curr_anim = &(garcia->garcia_down_right2);
				App->audio->PlayFx(audio_fx::GROUND_HIT);
			}
			else if (garcia->curr_anim == &(garcia->garcia_down_right2))
			{
				if (garcia->curr_anim->Finished())
				{
					garcia->curr_anim->Reset();

					if(garcia->life > 0)
						garcia->curr_anim = &(garcia->garcia_up_right);
					else					
						garcia->knocked_down = false;						
					
				}
			}
			else if (garcia->curr_anim == &(garcia->garcia_up_right))
			{
				if (garcia->curr_anim->Finished())
				{
					garcia->curr_anim->Reset();
					garcia->knocked_down = false;					
					garcia->up = false;
					garcia->down_count = 0;
					garcia->position.y = garcia->pos_before_knockdown.y;
					garcia->depth = garcia->position.y;
				}
			}

			//left animations
			if (garcia->curr_anim == &(garcia->garcia_down_left1))
			{
				garcia->curr_anim->Reset();
				garcia->curr_anim = &(garcia->garcia_down_left2);
				App->audio->PlayFx(audio_fx::GROUND_HIT);
			}
			else if (garcia->curr_anim == &(garcia->garcia_down_left2))
			{
				if (garcia->curr_anim->Finished())
				{
					garcia->curr_anim->Reset();

					if(garcia->life > 0)
						garcia->curr_anim = &(garcia->garcia_up_left);
					else
						garcia->knocked_down = false;
				}
			}
			else if (garcia->curr_anim == &(garcia->garcia_up_left))
			{
				if (garcia->curr_anim->Finished())
				{
					garcia->curr_anim->Reset();
					garcia->knocked_down = false;					
					garcia->up = false;
					garcia->down_count = 0;
					garcia->position.y = garcia->pos_before_knockdown.y;
					garcia->depth = garcia->position.y;
				}
			}
		}
	}
}

void GarciaFSM::Dead()
{
	if (garcia->facing_right)
	{
		if (garcia->blink)
			garcia->curr_anim = &garcia->garcia_dead_blink_effect;
		else
			garcia->curr_anim = &garcia->garcia_down_right2;
	}
	else
	{
		if (garcia->blink)
			garcia->curr_anim = &garcia->garcia_dead_blink_effect;
		else
			garcia->curr_anim = &garcia->garcia_down_left2;
	}

	garcia->blink_counter++;
	if (garcia->blink_counter == garcia->blink_wait_frames)
	{
		garcia->blink_counter = 0;
		garcia->blink = !garcia->blink;
		garcia->blink_times_counter++;
		if (garcia->blink_times_counter > garcia->blink_max_times)
		{
			//Mark colliders as deleteable	
			garcia->body_collider->to_delete = true;
			garcia->hit_collider->to_delete = true;

			//destroy de entity
			garcia->destroy_this = true;	
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------


GarciaFSM::State GarciaFSM::GetCurrState()  const
{
	return curr_state;
}

void GarciaFSM::SetCurrState(State state)
{
	curr_state = state;
}

void GarciaFSM::UpdateColliderPosition()
{
	if (garcia->facing_right)
	{
		garcia->body_collider->SetPos(garcia->position.x + garcia->body_collider_offset_right, garcia->position.y);
		garcia->hit_collider->SetPos(garcia->position.x + garcia->hit_collider_offset_right, garcia->position.y);
	}
	else
	{
		garcia->body_collider->SetPos(garcia->position.x + garcia->body_collider_offset_left, garcia->position.y);
		garcia->hit_collider->SetPos(garcia->position.x + garcia->hit_collider_offset_left, garcia->position.y);
	}

}
