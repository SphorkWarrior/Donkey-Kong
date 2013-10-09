#include "movingObject.h"
#include "math.h"


/*
 * Need to maintain a list of moving objects
 * Need to implement random AI
 * Need to rough plan this out
 */
static DonkeyKong donkeyKong;
static char* donkeyKong_list[NUM_DONKEYKONG_IMGS] = {"DK1.BMP", "DK2.BMP", "DK3.BMP",
		"DK4.BMP", "DK5.BMP", "DK6.BMP", "DK7.BMP", "DK8.BMP", "DK9.BMP", "DK10.BMP", "DK11.BMP" };
static colour donkeyKong_alpha = { 0x1F, 0x00, 0x1F };

static MovingObject* barrelListHead;
static char* barrel_list[NUM_BARREL_IMGS] = {"B1.BMP", "B2.BMP", "B3.BMP", "B4.BMP", "B5.BMP"};
static AnimMap barrel_anim_list[NUM_BARREL_IMGS];
static colour barrel_alpha = { 0x1F, 0x00, 0x1F };
static double frame_dir = 0.5;
static int barrel_dir = 1;

static MovingObject* fireListHead;
static char* fire_list[NUM_FIRE_IMGS] = {"FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP", "FIRE3.BMP"};
static AnimMap fire_anim_list[NUM_FIRE_IMGS];
static colour fire_alpha = { 0x1F, 0x00, 0x1F };

static Peach peach;
static char* peach_list[NUM_PEACH_IMGS] = {"PP1.BMP", "PP2.BMP", "PP3.BMP"};
static colour peach_alpha = { 0x1F, 0x00, 0x1F };

void drawFire(MovingObject* fire)
{
	int cur_frame = (int) round(fire->current_frame);
	draw_bmp(fire_anim_list[cur_frame].handle, fire->x, fire->y,true, fire_alpha, 1);
}

void drawFires()
{
	MovingObject* fireItr = fireListHead;
	while(fireItr != NULL)
	{
		drawFire(fireItr);
	}
}

void loadFire( int x, int y)
{
	MovingObject* fireItr = fireListHead;
	while(fireItr->next != NULL)
	{
		fireItr = fireItr->next;
	}

	MovingObject* newFire = malloc(sizeof(MovingObject));
	addFire(newFire, x, y);
}

void addFire(MovingObject* newFire, int x, int y)
{
	newFire->x = x;
	newFire->y = y;
	newFire->speed = 1;
	newFire->current_frame = FIRE_BUCKET;
	newFire->state = STILL;
}

void loadFires()
{
	load_bmp(fire_list[FIRE_BUCKET], &(fire_anim_list[FIRE_BUCKET].handle));
	load_bmp(fire_list[FIRE_BALL_ONE], &(fire_anim_list[FIRE_BALL_ONE].handle));
	load_bmp(fire_list[FIRE_BALL_TWO], &(fire_anim_list[FIRE_BALL_TWO].handle));
	load_bmp(fire_list[FIRE_BALL_THREE], &(fire_anim_list[FIRE_BALL_THREE].handle));
}

void drawBarrel(MovingObject* barrel)
{
	int cur_frame = (int) round(barrel->current_frame);
	draw_bmp(barrel_anim_list[cur_frame].handle, barrel->x,
			barrel->y, true, barrel_alpha, 1);
}

void drawBarrels()
{
	MovingObject* barrelItr = barrelListHead;
	while(barrelItr != NULL)
	{
		drawBarrel(barrelItr);
		barrelItr = barrelItr->next;
	}
}

void loadBarrel( int x, int y)
{
	MovingObject* barrelItr = barrelListHead;
	MovingObject* lastItr = NULL;

	while(barrelItr != NULL)
	{
		lastItr = barrelItr;
		barrelItr = barrelItr->next;
	}

	barrelItr = malloc(sizeof(MovingObject));
	barrelItr->next = NULL;
	barrelItr->prev = lastItr;
	if (lastItr != NULL) lastItr->next = barrelItr;

	if (barrelListHead == NULL) barrelListHead = barrelItr;

	addBarrel(barrelItr, x, y);
}

void addBarrel(MovingObject* newBarrel, int x, int y)
{
	newBarrel->x = x;
	newBarrel->y = y;
	newBarrel->speed = 1;
	newBarrel->current_frame = FLAT_BARREL;
	newBarrel->state = LAYING;
}

void loadBarrels()
{
	load_bmp(barrel_list[FLAT_BARREL], &(barrel_anim_list[FLAT_BARREL].handle));
	load_bmp(barrel_list[ROLLING_TOP_LEFT], &(barrel_anim_list[ROLLING_TOP_LEFT].handle));
	load_bmp(barrel_list[ROLLING_TOP_RIGHT], &(barrel_anim_list[ROLLING_TOP_RIGHT].handle));
	load_bmp(barrel_list[ROLLING_BOTTOM_LEFT], &(barrel_anim_list[ROLLING_BOTTOM_LEFT].handle));
	load_bmp(barrel_list[ROLLING_BOTTOM_RIGHT], &(barrel_anim_list[ROLLING_BOTTOM_RIGHT].handle));
}

void animateBarrels(BarrelImage lowFrame, BarrelImage highFrame)
{
	MovingObject* barrelItr = barrelListHead;
	barrelItr->past_frame = barrelItr->current_frame;

	if (barrelItr->current_frame < lowFrame || barrelItr->current_frame > highFrame) {
		barrelItr->current_frame = lowFrame;
	} else {
		barrelItr->current_frame += 0.1;
	}

	if (barrelItr->current_frame > highFrame || barrelItr->current_frame < lowFrame) {
		frame_dir = -0.1;
	}
}

void moveBarrels(BarrelImage lowFrame, BarrelImage highFrame) {
	//double tmp_frame = mario.current_frame;
	MovingObject* barrelItr = barrelListHead;
	if (barrelItr->x  + MOgetCurrentWidth(barrelItr) >= 320 || barrelItr->x <= 0)
		barrel_dir = -barrel_dir;
	barrelItr->x += barrel_dir;

	if (barrelItr->y + MOgetCurrentHeight(barrelItr) > find_floor(barrelItr->x, barrelItr->y, 0))
		barrelItr->y -= 1;
	else if (barrelItr->y + MOgetCurrentHeight(barrelItr) < find_floor(barrelItr->x, barrelItr->y, 0)){
		barrelItr->y += 1;
		MOdrawBackground(barrelItr->x, barrelItr->y - 1 , barrelItr->x + MOgetCurrentWidth(barrelItr),barrelItr->y);
	}

	animateBarrels(lowFrame, highFrame);

	if (barrel_dir < 0)
		MOdrawBackground(barrelItr->x + MOgetCurrentWidth(barrelItr) + 1, barrelItr->y,
				barrelItr->x + getPastWidth(barrelItr) + 1, barrelItr->y + MOgetCurrentHeight(barrelItr));
	else
		MOdrawBackground(barrelItr->x-1, barrelItr->y,
						barrelItr->x, barrelItr->y + MOgetCurrentHeight(barrelItr));
}

void MOdrawBackground(int x0, int y0, int x1, int y1) {
	drawBackgroundSection(x0, y0, x1, y1);
	swap_buffers();
	drawBackgroundSection(x0, y0, x1, y1);
}

int MOgetCurrentWidth(MovingObject* itr)
{
	return barrel_anim_list[(int) round(itr->current_frame)].handle->bmp_info_header->width;
}

int MOgetCurrentHeight(MovingObject* itr)
{
	return barrel_anim_list[(int) round(itr->current_frame)].handle->bmp_info_header->height;
}

int MOgetPastWidth(MovingObject* itr)
{
		return barrel_anim_list[(int) round(itr->past_frame)].handle->bmp_info_header->width;
}

int MOgetPastHeight(MovingObject* itr)
{
	return barrel_anim_list[(int) round(itr->past_frame)].handle->bmp_info_header->height;
}



void loadDonkeyKong(int x, int y)
{
	// Animation list for Donkey Kong
	load_bmp(donkeyKong_list[STANDING_STILL], &(donkeyKong.animation[STANDING_STILL].handle));
	load_bmp(donkeyKong_list[GRABBING_BARREL], &(donkeyKong.animation[GRABBING_BARREL].handle));
	load_bmp(donkeyKong_list[ANGRY_LEFT], &(donkeyKong.animation[ANGRY_LEFT].handle));
	load_bmp(donkeyKong_list[ANGRY_RIGHT], &(donkeyKong.animation[ANGRY_RIGHT].handle));
	load_bmp(donkeyKong_list[CLIMBING_LEFT], &(donkeyKong.animation[CLIMBING_LEFT].handle));
	load_bmp(donkeyKong_list[CLIMBING_RIGHT], &(donkeyKong.animation[CLIMBING_RIGHT].handle));
	load_bmp(donkeyKong_list[UPSIDE_DOWN_LEFT], &(donkeyKong.animation[UPSIDE_DOWN_LEFT].handle));
	load_bmp(donkeyKong_list[UPDATE_DOWN_RIGHT], &(donkeyKong.animation[UPDATE_DOWN_RIGHT].handle));
	load_bmp(donkeyKong_list[WALK_RIGHT], &(donkeyKong.animation[WALK_RIGHT].handle));
	load_bmp(donkeyKong_list[SUPER_ANGRY], &(donkeyKong.animation[SUPER_ANGRY].handle));

	// There are no right-facing animations. It's all either facing the player or not.
	// There are no flipping animations.

	donkeyKong.current_frame = STANDING_STILL;
	donkeyKong.x = x;
	donkeyKong.y = y;
	donkeyKong.state = STANDING;

}

void drawDonkeyKong(void)
{
	int cur_frame = (int) round(donkeyKong.current_frame);
	draw_bmp(donkeyKong.animation[cur_frame].handle,
			donkeyKong.x, donkeyKong.y, true, donkeyKong_alpha, 1);
}

void drawPeach(void)
{
	int cur_frame = (int) round(peach.current_frame);
	draw_bmp(peach.animation[cur_frame].handle,
			 peach.x, peach.y, true, peach_alpha, 1);
}

void loadPeach(int x, int y)
{
	load_bmp(peach_list[PEACH_HELP], &(peach.animation[PEACH_HELP].handle));
	load_bmp(peach_list[PEACH_HEARTBROKEN], &(peach.animation[PEACH_HEARTBROKEN].handle));
	load_bmp(peach_list[PEACH_HEART], &(peach.animation[PEACH_HEART].handle));

	peach.current_frame = PEACH_HELP;
	peach.x = x;
	peach.y = y;
	peach.state = HELP;
}
