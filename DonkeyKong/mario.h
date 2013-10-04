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

typedef enum { UP, DOWN, LEFT, RIGHT } MarioDirection;

/* Mario Files */
typedef enum { STAND_IMG, WALK1_IMG, WALK2_IMG, NUM_IMGS } MarioImages;

/* Mario Animations */
typedef enum { STAND_LEFT, WALK1_LEFT, WALK2_LEFT, STAND_RIGHT,
			WALK1_RIGHT, WALK2_RIGHT, NUM_ANIMS } MarioAnims;

/* Mario States */
typedef enum { WALKING, JUMPING, M_CLIMBING, LADDER_BOTTOM,
	LADDER_TOP, DEAD, NUM_STATES } MarioState;

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
	MarioState state;
} Mario;

void loadMario(int x, int y, int speed);
void drawMario(void);

bool moveMario(MarioDirection dir);
Mario getMario(void);

bool moveUp(void);
bool moveDown(void);
bool moveLeft(void);
bool moveRight(void);

int getCurrentWidth(void);
int getCurrentHeight(void);

void changeMarioState(MarioState state);
MarioState getMarioState(void);

#endif /* MARIO_H_ */
