#ifndef __GARCIAFSM_H__
#define __GARCIAFSM_H__

class Garcia;
class GarciaFSM
{
private:
	enum class State
	{
		START,
		IDLE,
		CHASE,		
		ATTACK,
		EVASIVE,
		DAMAGED,
		KNOCKED_DOWN,
		GRABBED
	};

	enum class EvasiveMovement
	{
		FIRST_STAGE,
		SECOND_STAGE,
		THIRD_STAGE
	};

	State curr_state = State::START;
	State prev_state = State::START;
	EvasiveMovement evasive_movement = EvasiveMovement::FIRST_STAGE;
	int evasive_v_count = 0;
	int evasive_h_count = 0;
	bool evasive_go = true;
	bool evasive_back = false;
	bool evasion_upper = false;
	bool evasion_lower = false;

	Garcia *garcia = nullptr;
	
	int num_frames = 60;
	int frames_counter = 0;

public:
	GarciaFSM(Garcia *garcia);
	~GarciaFSM();
	
	void Update();
	void UpdateColliderPosition();
	//-----------------------------  ACTIONS TO PERFORM IN EACH STATE ------------------------------------
	void Idle();
	void Chase();
	void Attack();
	void Evasive();
	void EvasiveFirstStage();
	void EvasiveSecondStage();
	void EvasiveThirdStage();
	void Grabbed();
	State GetCurrState()  const;
	void SetCurrState(State state);
};


#endif