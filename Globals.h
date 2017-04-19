#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "MemLeaks.h"
#include "SDL/include/SDL_rect.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum collider_type
{
	ENEMY_BODY,
	ENEMY_HIT,
	COLLECTABLE,
	PLAYER_BODY,
	PLAYER_HIT,
	DESTROYABLE,
	BOSS_AOE
};

// Useful typedefs ---------
typedef unsigned int uint;

// Deletes a buffer
#define RELEASE( x ) \
    {									  \
       if( x != nullptr )   \
       {						      \
         delete x;                  \
	     x = nullptr;             \
       }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{                              \
       if( x != nullptr )              \
       {                            \
           delete[] x;                \
	       x = nullptr;                    \
		 }                            \
                              \
	 }

// Configuration -----------
#define CAMERA_STARTING_POS_X 771
#define SCREEN_SIZE 3
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Tribute to Streets of Rage"

//Player movement limits
#define MIN_Y_POSITION 90
#define MAX_Y_POSITION 150
#define MIN_X_POSITION 700
#define MAX_X_POSITION 3870

#endif //__GLOBALS_H__