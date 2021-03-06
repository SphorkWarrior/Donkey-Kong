/*
 * mario.c
 *
 *  Created on: 2013-10-01
 *      Author: Jeremy
 */
#include "mario.h"
#include "audio.h"
#include <math.h>

#define FRAME_SPEED		0.05

static Mario mario;
static char* anim_list[NUM_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP", "M12.BMP"};
static colour mario_alpha = { 0x00, 0x00, 0x00 };
static double frame_dir = FRAME_SPEED;
static float past_frame = STAND_LEFT;

// Global Jump Buffer:
unsigned int* jumpSoundBuf = 0;
int jumpSoundBufLen = 0;

Mario getMario(void) {
	return mario;
}

void loadMario(int x, int y, int speed)
{
	/* Prepare the animation map. */
	load_bmp(anim_list[STAND_IMG], &(mario.animation[STAND_LEFT].handle));
	load_bmp(anim_list[WALK1_IMG], &(mario.animation[WALK1_LEFT].handle));
	load_bmp(anim_list[WALK2_IMG], &(mario.animation[WALK2_LEFT].handle));
	load_bmp(anim_list[CLIMB_IMG], &(mario.animation[CLIMB1].handle));

	/* The right-facing animations use the same images. */
	mario.animation[STAND_RIGHT].handle = mario.animation[STAND_LEFT].handle;
	mario.animation[WALK1_RIGHT].handle = mario.animation[WALK1_LEFT].handle;
	mario.animation[WALK2_RIGHT].handle = mario.animation[WALK2_LEFT].handle;
	mario.animation[CLIMB2].handle = mario.animation[CLIMB1].handle;

	mario.animation[STAND_LEFT].flip = false;
	mario.animation[WALK1_LEFT].flip = false;
	mario.animation[WALK2_LEFT].flip = false;
	mario.animation[STAND_RIGHT].flip = true;
	mario.animation[WALK1_RIGHT].flip = true;
	mario.animation[WALK2_RIGHT].flip = true;
	mario.animation[CLIMB2].flip = true;

	mario.current_frame = STAND_RIGHT;
	mario.state = WALKING;
	mario.x = x;
	mario.y = y;
	mario.jumpStart = y;
	mario.speed = speed;
	mario.currentFloor = 6;
}

void drawMario(bool bothBuffers)
{
	int cur_frame = (int) round(mario.current_frame);
	static int deadCount = 0;

	if (mario.state == DEAD)
	{
		deadCount++;
		if (deadCount > 300)
		{
			mario.state = WALKING;
			if (mario.x != MARIO_START_X && mario.y != MARIO_START_Y)
			{
				drawMarioBackground(mario.x, mario.y,
						mario.x + getCurrentWidth(), mario.y + getCurrentHeight());
			}
			mario.x = MARIO_START_X;
			mario.y = MARIO_START_Y;
			mario.current_frame = STAND_RIGHT;
			mario.jumpStart = MARIO_START_Y;
			mario.currentFloor = 6;
			deadCount = 0;
		}
		return;
	}

	if ((cur_frame >= STAND_RIGHT && cur_frame <= WALK2_RIGHT) || cur_frame == CLIMB2)
	{
		draw_flipped_bmp(mario.animation[cur_frame].handle,
				mario.x, mario.y, true, mario_alpha, 1);
		if (bothBuffers == true)
		{
			swap_buffers();
			draw_flipped_bmp(mario.animation[cur_frame].handle,
					mario.x, mario.y, true, mario_alpha, 1);
		}
	}
	else
	{
		draw_bmp(mario.animation[cur_frame].handle,
				mario.x, mario.y, true, mario_alpha, 1);
		if (bothBuffers == true)
		{
			swap_buffers();
			draw_bmp(mario.animation[cur_frame].handle,
					mario.x, mario.y, true, mario_alpha, 1);
		}
	}
}

bool moveMario(MarioDirection dir)
{
	bool ret = true;
	switch (dir)
	{
	case UP:
		ret = moveUp();
		break;
	case DOWN:
		ret = moveDown();
		break;
	case LEFT:
		ret = moveLeft();
		break;
	case RIGHT:
		ret = moveRight();
		break;
	}

	return ret;
}

void animate(MarioAnims lowFrame, MarioAnims highFrame)
{
	past_frame = mario.current_frame;

	if (mario.current_frame < lowFrame || mario.current_frame > highFrame) {
		mario.current_frame = lowFrame;
	} else {
		mario.current_frame += frame_dir;
	}

	if (mario.current_frame > highFrame || mario.current_frame < lowFrame) {
		frame_dir = -frame_dir;
	}
}

void move(int x, int y, MarioAnims lowFrame, MarioAnims highFrame, bool flip) {
	//double tmp_frame = mario.current_frame;
	mario.x += x;
	mario.y += y;

	if (mario.state == WALKING || mario.state == LADDER_TOP ||
			mario.state == LADDER_BOTTOM) {
		animate(lowFrame, highFrame);
	}
}

void drawMarioBackground(int x0, int y0, int x1, int y1) {
	pushEraseNode(x0, y0, x1, y1);
}

int getCurrentWidth(void)
{
	return mario.animation[(int) round(mario.current_frame)].handle->bmp_info_header->width;
}

int getCurrentHeight(void)
{
	return mario.animation[(int) round(mario.current_frame)].handle->bmp_info_header->height;
}

int getPastWidth(void)
{
		return mario.animation[(int) round(past_frame)].handle->bmp_info_header->width;
}

int getPastHeight(void)
{
	return mario.animation[(int) round(past_frame)].handle->bmp_info_header->height;
}

void setMarioJumpStart(int height)
{
	mario.jumpStart = height;
}

int getMarioJumpStart(void)
{
	return mario.jumpStart;
}

bool moveLeft(void)
{
	if (mario.x - mario.speed > 0) {
		move(-mario.speed, 0, STAND_LEFT, WALK2_LEFT, false);
		drawMarioBackground(mario.x + getCurrentWidth(), mario.y,
				mario.x + getPastWidth() + mario.speed, mario.y + getCurrentHeight());

		return true;
	}

	return false;
}

bool moveRight(void)
{
	if (mario.x + getCurrentWidth() + mario.speed < 320) {
		move(mario.speed, 0, STAND_RIGHT, WALK2_RIGHT, true);
		drawMarioBackground(mario.x-mario.speed, mario.y,
				mario.x, mario.y + getCurrentHeight());
		return true;
	}

	return false;
}

bool moveDown(void)
{
	mario.y += mario.speed;
	drawMarioBackground(mario.x, mario.y - mario.speed,
			mario.x + getCurrentWidth(), mario.y);

	if (mario.state == M_CLIMBING) {
		animate(CLIMB1, CLIMB2);
	}

	return true;
}

bool moveUp(void)
{
	mario.y -= mario.speed;
	drawMarioBackground(mario.x, mario.y + getCurrentHeight(),
			mario.x + getCurrentWidth(), mario.y + getCurrentHeight() + mario.speed);

	if (mario.state == M_CLIMBING) {
		animate(CLIMB1, CLIMB2);
	}

	return true;
}

void changeMarioState(MarioState state)
{
	mario.state = state;

	float frame = round(mario.current_frame);

	if (mario.state == JUMPING) {
		addInSound(jumpSoundBuf, jumpSoundBufLen);
	}

	if (frame >= STAND_RIGHT && frame <= WALK2_RIGHT)
	{
		if (mario.state == JUMPING || mario.state == FALLING) {
			mario.current_frame = WALK1_RIGHT;
		}
	}
	else if(frame >= STAND_LEFT && frame <= WALK2_LEFT)
	{
		if (mario.state == JUMPING || mario.state == FALLING) {
			mario.current_frame = WALK1_LEFT;
		}
	}

	if (mario.state == M_CLIMBING) {
		mario.current_frame = CLIMB1;
	}
}

MarioState getMarioState(void)
{
	return mario.state;
}

Mario* getMarioRef(void)
{
	return &mario;
}

int getMarioCurrentFloor(void)
{
	return mario.currentFloor;
}

void setMarioCurrentFloor(int currentFloor)
{
	if (currentFloor < 0 || currentFloor > 6) return;

	mario.currentFloor = currentFloor;
}
