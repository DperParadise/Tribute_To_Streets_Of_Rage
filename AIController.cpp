#include "AIController.h"
#include "Enemy.h"
#include "Player.h"
#include "Application.h"
#include "ModuleEnemies.h"


AIController::AIController() {}
AIController::~AIController() {}

void AIController::UpdateBehaviour()
{
	//Refactorizar cuando tenga colisiones


	if (!strcmp(m_enemy->m_name, "garcia"))
	{
		
		//if (m_enemy->m_position.x - 10 > m_player->m_position.x + 10)
		//{
		//	if (m_enemy->m_face_right)
		//		m_enemy->m_face_right = false;

		//	m_enemy->m_ai_walk = true;

		//	if (m_player->m_face_right)
		//		m_look_each_other = true;
		//}
		//if (m_enemy->m_position.x - 9 == m_player->m_position.x + 14)
		//{
		//	//combate
		//	m_enemy->m_ai_walk = false;
		//	
		//	if (m_continue)
		//	{
		//		m_enemy->m_state = enemy_state::IDLE;
		//		m_enemy->m_ai_attack = true;
		//	}
		//	else
		//	{
		//		m_enemy->m_ai_attack = false;	
		//	}

		//	
		//	//cuando no hay colision desactivo el ataque: m_enemy->m_ai_attack = false
		//}
		//if (m_enemy->m_position.x - 9 < m_player->m_position.x + 14)
		//{
		//	if (m_enemy->m_face_right == false)
		//		m_enemy->m_face_right = true;

		//	m_enemy->m_ai_walk = true;

		//	if (m_player->m_face_right == false)
		//		m_look_each_other = true;
		//}

		m_vector_enemy_to_player = { m_player->m_position.x + 14 - m_enemy->m_position.x - 9, m_player->m_position.y - m_enemy->m_position.y };

		if (m_vector_enemy_to_player.x > 0)
			m_sign_x = 1;

		if (m_vector_enemy_to_player.x == 0)
			m_sign_x = 0;

		if (m_vector_enemy_to_player.x < 0)
			m_sign_x = -1;

		if (m_vector_enemy_to_player.y > 0)
			m_sign_y = 1;

		if (m_vector_enemy_to_player.y == 0)
			m_sign_y = 0;

		if (m_vector_enemy_to_player.y < 0)
			m_sign_y = -1;

		if (m_sign_x <= 0)
			m_enemy->m_face_right = false;

		if (m_sign_x > 0)
			m_enemy->m_face_right = true;

		if (m_enemy->m_face_right && m_player->m_face_right == false)
			m_look_each_other = true;
		else
			m_look_each_other = false;
		
		if (m_enemy->m_ai_attack)
		{
			m_enemy->m_ai_walk = false;

			if (m_continue)
			{
				m_enemy->m_state = enemy_state::IDLE;
				m_enemy->m_ai_attack = true;
			}
			else
			{
				m_enemy->m_ai_attack = false;	
			}
		}

		if (m_enemy->m_ai_walk == true)
		{
			m_vector_enemy_to_player = { m_player->m_position.x + 14 - m_enemy->m_position.x - 9, m_player->m_position.y - m_enemy->m_position.y };

			if (m_vector_enemy_to_player.x > 0)
				m_sign_x = 1;

			if (m_vector_enemy_to_player.x == 0)
				m_sign_x = 0;

			if (m_vector_enemy_to_player.x < 0)
				m_sign_x = -1;

			if (m_vector_enemy_to_player.y > 0)
				m_sign_y = 1;

			if (m_vector_enemy_to_player.y == 0)
				m_sign_y = 0;

			if (m_vector_enemy_to_player.y < 0)
				m_sign_y = -1;
		}
	}


	if (m_enemy->m_ai_walk == true)
	{
		bool grab_condition = (
			m_player->m_state == player_state::GRAB ||
			m_player->m_state == player_state::GRAB_KICK ||
			m_player->m_state == player_state::GRAB_HEAD_HIT ||
			m_player->m_state == player_state::GRAB_AIR_SPIN_RIGHT ||
			m_player->m_state == player_state::GRAB_AIR_SPIN_LEFT ||
			m_player->m_state == player_state::GRAB_AIR_SPIN_FINISHER_RIGHT ||
			m_player->m_state == player_state::GRAB_AIR_SPIN_FINISHER_LEFT);

		if (!grab_condition)
		{
			m_enemy->m_position.x = (int)(m_enemy->m_position.x + m_sign_x);
			App->enemies->UpdateColliderPosition();
		}

		if (m_player->m_state != player_state::JUMPING && m_player->m_state != player_state::JUMPING_KICKING && !grab_condition)
		{
			m_enemy->m_position.y = (int)(m_enemy->m_position.y + m_sign_y);
			m_enemy->m_depth = m_enemy->m_position.y;
			App->enemies->UpdateColliderPosition();
		}
	}



	
}