/*
 * mario.h
 *
 *  Created on: 2013-10-01
 *      Author: Jeremy
 */

#ifndef MARIO_H_
#define MARIO_H_

#include "display.h"
#include "bitmap.h"

#define MARIO_START_X 	0
#define MARIO_START_Y 	217
typedef enum { UP, DOWN, LEFT, RIGHT } MarioDirection;

/* Mario Files */
typedef enum { STAND_IMG, WALK1_IMG, WALK2_IMG, CLIMB_IMG, NUM_IMGS } MarioImages;

/* Mario Animations */
typedef enum { STAND_LEFT, WALK1_LEFT, WALK2_LEFT, STAND_RIGHT,
			WALK1_RIGHT, WALK2_RIGHT, CLIMB1, CLIMB2, NUM_ANIMS } MarioAnims;

/* Mario States */
typedef enum { WALKING, JUMPING, FALLING, M_CLIMBING, LADDER_BOTTOM,
	LADDER_TOP, DEAD, HAMMERING, NUM_MARIO_STATES } MarioState;

#define MAX_JUMP 15

typedef struct
{
	BitmapHandle* handle;
	bool flip;
} MarioAnimMap;

typedef struct
{
	MarioAnimMap animation[NUM_ANIMS];
	int x, y;
	int speed;
	float current_frame;
	int jumpStart;
	int currentFloor;
	MarioState state;
} Mario;

void loadMario(int x, int y, int speed);
void drawMario(bool bothBuffers);

bool moveMario(MarioDirection dir);
Mario getMario(void);
Mario* getMarioRef(void);

bool moveUp(void);
bool moveDown(void);
bool moveLeft(void);
bool moveRight(void);

int getCurrentWidth(void);
int getCurrentHeight(void);
void setMarioJumpStart(int height);
int getMarioJumpStart(void);

int getMarioCurrentFloor(void);
void setMarioCurrentFloor(int currentFloor);

void changeMarioState(MarioState state);
MarioState getMarioState(void);

#endif /* MARIO_H_ */
