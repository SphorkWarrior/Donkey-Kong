#include "movingObject.h"
#include "math.h"
#include "level1.h"
#include "random.h"
#include "mario.h"

/*
 * Need to maintain a list of moving objects
 * Need to implement random AI
 * Need to rough plan this out
 */
static DonkeyKong donkeyKong;
static char* donkeyKong_list[NUM_DONKEYKONG_IMGS] = {"DK1.BMP", "DK10.BMP", "DK2.BMP","DK9.BMP", "DK3.BMP",
		"DK4.BMP", "DK5.BMP", "DK6.BMP", "DK7.BMP", "DK8.BMP"};
static AnimMap DK_anim_list[NUM_DONKEYKONG_IMGS];
static double dk_frame_dir = 0.03;
static colour donkeyKong_alpha = { 0x00, 0x00, 0x00 };

static MovingObject* barrelListHead;
static char* barrel_list[NUM_BARREL_IMGS] = {"B1.BMP", "B2.BMP", "B3.BMP", "B4.BMP", "B5.BMP"};
static AnimMap barrel_anim_list[NUM_BARREL_IMGS];
static colour barrel_alpha = { 0x00, 0x00, 0x00 };
static double frame_dir = 0.07;

static MovingObject* fireListHead;
static char* fire_list[NUM_FIRE_IMGS] = {"FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP", "FIRE3.BMP"};
static AnimMap fire_anim_list[NUM_FIRE_IMGS];
static colour fire_alpha = { 0x1F, 0x00, 0x1F };

static Peach peach;
static char* peach_list[NUM_PEACH_IMGS] = {"PP1.BMP", "PP2.BMP", "PP3.BMP"};
static colour peach_alpha = { 0x00, 0x00, 0x00 };

static unsigned int goDownLadder(MovingObject* barrelItr);

MovingObject* getBarrelListHead(void)
{
	return barrelListHead;
}

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
	newFire->byLadder = 0;
	newFire->currentFloor = 6;
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

	if (barrel->state != THROWABLE)
	{
		if (barrel->speed < 0){
			draw_flipped_bmp(barrel_anim_list[cur_frame].handle, barrel->x, barrel->y, true, barrel_alpha, 1);
		}
		else {
			draw_bmp(barrel_anim_list[cur_frame].handle, barrel->x,
					barrel->y, true, barrel_alpha, 1);
		}
	}
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
	newBarrel->currentFloor = 1;
	newBarrel->byLadder = 0;
	newBarrel->current_frame = FLAT_BARREL;
	newBarrel->state = THROWABLE;
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
	while(barrelItr != NULL){
		barrelItr->past_frame = barrelItr->current_frame;

		if (barrelItr->current_frame < lowFrame || barrelItr->current_frame > highFrame) {
			barrelItr->current_frame = lowFrame;
		} else {
			barrelItr->current_frame += frame_dir;
		}

		if (barrelItr->current_frame > highFrame || barrelItr->current_frame < lowFrame) {
			frame_dir = -frame_dir;
		}
		barrelItr = barrelItr->next;
	}
}

static unsigned int goDownLadder(MovingObject* barrelItr) {
	unsigned int ret = 0;

	if (barrelItr->byLadder == 0 && getMario().currentFloor > barrelItr->currentFloor)
	{
		if (getMario().currentFloor == barrelItr->currentFloor + 1)
		{
			// Mario is on the floor directly below. Go down according to this rule:
			if (getMario().currentFloor % 2 == 0 && barrelItr->x > getMario().x)
			{
				ret = 1;
			}
			else if (getMario().currentFloor % 2 != 0 && barrelItr->x < getMario().x)
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
		}
		else
		{
			ret = 1;
		}
	}

	return ret && nextRand() % 3 == 0;
}

void stopBarrels(void)
{
	MovingObject* barrelItr = barrelListHead;

	while (barrelItr != NULL)
	{
		barrelItr->state = THROWABLE;
		MOdrawBackground(barrelItr->x, barrelItr->y,
				barrelItr->x + MOgetCurrentWidth(barrelItr), barrelItr->y + MOgetCurrentHeight(barrelItr));
		barrelItr = barrelItr->next;
	}
}

unsigned char handleCollision(void)
{
	MovingObject* barrelItr = barrelListHead;

	while (barrelItr != NULL)
	{

		if (getMario().x <= barrelItr->x + MOgetCurrentWidth(barrelItr)
				&& getMario().x + getCurrentWidth() >= barrelItr->x
				&& getMario().y <= barrelItr->y + MOgetCurrentHeight(barrelItr)
				&& getMario().y + getCurrentHeight() >= barrelItr->y
				&& (barrelItr->state == OBJ_FALLING || barrelItr->state == ROLLING))
		{
			// Colliding.
			if (getMarioState() == HAMMERING)
			{
				// Barrel dies;
				barrelItr->state = THROWABLE;
				MOdrawBackground(barrelItr->x, barrelItr->y,
								barrelItr->x + MOgetCurrentWidth(barrelItr), barrelItr->y + MOgetCurrentHeight(barrelItr));
			}
			else
			{
				changeMarioState(DEAD);
				stopBarrels();
				donkeyKong.state = ANGRY;
			}
			return 1;
		}

		barrelItr = barrelItr->next;
	}

	return 0;
}

unsigned char moveBarrels(BarrelImage lowFrame, BarrelImage highFrame)
{
	MovingObject* barrelItr = barrelListHead;
	unsigned char oneThrown = 0;

	while(barrelItr != NULL)
	{
		int ladderIndicator = -1;

		if (barrelItr->state == THROWABLE)
		{
			// Should have logic here to figure out when to throw barrel.
			if ((int) round(donkeyKong.current_frame) == ROLLING_BARREL &&
					(int) round(donkeyKong.past_frame) == CARRYING_BARREL && !oneThrown)
			{
				oneThrown = 1;
				barrelItr->x = 102;
				barrelItr->y = 83;
				barrelItr->speed = 1;
				barrelItr->state = ROLLING;
			}
		}
		else if (barrelItr->state == ROLLING)
		{
			if (should_barrel_die(barrelItr->x, barrelItr->y + MOgetCurrentHeight(barrelItr)))
			{
				if (barrelItr->x+MOgetCurrentWidth(barrelItr) <= 0) {
					barrelItr->state = THROWABLE;
					if (donkeyKong.state == ANGRY) {
						donkeyKong.state = THROWING;
						donkeyKong.current_frame = STANDING_STILL;
					}
				}
			}
			else if ((ladderIndicator = find_ladder_top(barrelItr->x, barrelItr->y, MOgetCurrentHeight(barrelItr), barrelItr->currentFloor)) != -1 && goDownLadder(barrelItr))
			{
				barrelItr->state = OBJ_FALLING;
				barrelItr->y += 1;
				MOdrawBackground(barrelItr->x, barrelItr->y - 1 , barrelItr->x + MOgetCurrentWidth(barrelItr), barrelItr->y);
			}
			else
			{
				if (ladderIndicator != -1 && !barrelItr->byLadder)
				{
					// The barrel is now by a ladder.
					barrelItr->byLadder = 1;
				}

				if (ladderIndicator == -1 && barrelItr->byLadder)
				{
					// This indicator makes sure the barrel will only attempt
					// to go down the ladder once, and not for each frame in which
					// it is by the ladder (otherwise it will nearly always go down).
					barrelItr->byLadder = 0;
				}

				if (barrelItr->x  + MOgetCurrentWidth(barrelItr) >= 320 || barrelItr->x <= 0)
					barrelItr->speed = -barrelItr->speed;

				int floorFound = find_floor(barrelItr->x, barrelItr->y, 0, &barrelItr->currentFloor);

				if (barrelItr->y + MOgetCurrentHeight(barrelItr) > floorFound)
				{
					barrelItr->y -= 1;
				}
				else if (barrelItr->y + MOgetCurrentHeight(barrelItr) < floorFound)
				{
					barrelItr->y += 1;
					MOdrawBackground(barrelItr->x, barrelItr->y - 1 , barrelItr->x + MOgetCurrentWidth(barrelItr),barrelItr->y);
				}
			}

			if (barrelItr->state != OBJ_FALLING)
			{
				barrelItr->x += barrelItr->speed;

				animateBarrels(lowFrame, highFrame);

				if (barrelItr->speed < 0)
				{
					if (barrelItr->x + MOgetCurrentWidth(barrelItr) < 320)
					{
						MOdrawBackground(barrelItr->x + MOgetCurrentWidth(barrelItr), barrelItr->y,
								barrelItr->x + MOgetPastWidth(barrelItr) + 1, barrelItr->y + MOgetCurrentHeight(barrelItr));
					}
				}
				else
				{
					if (barrelItr->x > 0)
					{
						MOdrawBackground(barrelItr->x-2, barrelItr->y,
										barrelItr->x-1, barrelItr->y + MOgetCurrentHeight(barrelItr));
					}
				}
			}
		}
		else if (barrelItr->state == OBJ_FALLING)
		{
			int ladder_ind = is_ladder(barrelItr->x, barrelItr->y, MOgetCurrentHeight(barrelItr), barrelItr->currentFloor);
			int ladder_end_y = getLaddersElement(ladder_ind).end.y;

			if (barrelItr->y + MOgetCurrentHeight(barrelItr) >= ladder_end_y-2 &&
					barrelItr->y + MOgetCurrentHeight(barrelItr) <= ladder_end_y+2)
			{
				barrelItr->state = ROLLING;
				barrelItr->speed = -barrelItr->speed;
			}
			else
			{
				barrelItr->y += 1;
				MOdrawBackground(barrelItr->x, barrelItr->y - 1 , barrelItr->x + MOgetCurrentWidth(barrelItr),barrelItr->y);
			}
		}

		barrelItr = barrelItr->next;
	}

	if (!oneThrown && (int) round(donkeyKong.current_frame) == ROLLING_BARREL &&
			(int) round(donkeyKong.past_frame) == CARRYING_BARREL)
	{
		// Change donkey kong state to pounding chest-- no barrels to throw.
		donkeyKong.state = ANGRY;
		donkeyKong.current_frame = ANGRY_LEFT;
	}

	return handleCollision();
}

void MOdrawBackground(int x0, int y0, int x1, int y1)
{
	pushEraseNode(x0, y0, x1, y1);
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
	load_bmp(donkeyKong_list[CARRYING_BARREL], &(donkeyKong.animation[CARRYING_BARREL].handle));
	load_bmp(donkeyKong_list[ROLLING_BARREL], &(donkeyKong.animation[ROLLING_BARREL].handle));
	load_bmp(donkeyKong_list[ANGRY_LEFT], &(donkeyKong.animation[ANGRY_LEFT].handle));
	load_bmp(donkeyKong_list[ANGRY_RIGHT], &(donkeyKong.animation[ANGRY_RIGHT].handle));
	load_bmp(donkeyKong_list[CLIMBING_LEFT], &(donkeyKong.animation[CLIMBING_LEFT].handle));
	load_bmp(donkeyKong_list[CLIMBING_RIGHT], &(donkeyKong.animation[CLIMBING_RIGHT].handle));
	load_bmp(donkeyKong_list[UPSIDE_DOWN_LEFT], &(donkeyKong.animation[UPSIDE_DOWN_LEFT].handle));
	load_bmp(donkeyKong_list[UPDATE_DOWN_RIGHT], &(donkeyKong.animation[UPDATE_DOWN_RIGHT].handle));

	// There are no right-facing animations. It's all either facing the player or not.
	// There are no flipping animations.

	donkeyKong.current_frame = STANDING_STILL;
	donkeyKong.x = x;
	donkeyKong.y = y;
	donkeyKong.state = THROWING;

}

void drawDonkeyKong(void)
{
	int cur_frame = (int) round(donkeyKong.current_frame);
	int past_frame = (int) round(donkeyKong.past_frame);

	draw_bmp(donkeyKong.animation[cur_frame].handle,
			donkeyKong.x, donkeyKong.y, true, donkeyKong_alpha, 1);

	int past_width = donkeyKong.animation[past_frame].handle->bmp_info_header->width;
	int cur_width = donkeyKong.animation[cur_frame].handle->bmp_info_header->width;

	if (past_width > cur_width)
	{
		// Went from a larger frame to smaller-- erase background it might have left.
		int cur_height = donkeyKong.animation[cur_frame].handle->bmp_info_header->height;
		pushEraseNode(donkeyKong.x + cur_width, donkeyKong.y,
				donkeyKong.x + past_width, donkeyKong.y + cur_height);
	}

	if (donkeyKong.state == THROWING)
	{
		animateDonkeyKong(STANDING_STILL, ROLLING_BARREL);
	}
	else if (donkeyKong.state == ANGRY)
	{
		animateDonkeyKong(ANGRY_LEFT, ANGRY_RIGHT);
	}
}

void animateDonkeyKong(DonkeyKongImage lowFrame, DonkeyKongImage highFrame)
{
	donkeyKong.past_frame = donkeyKong.current_frame;

	if (donkeyKong.current_frame < lowFrame || donkeyKong.current_frame > highFrame) {
		donkeyKong.current_frame = lowFrame;
	} else {
		donkeyKong.current_frame += dk_frame_dir;
	}

	if (donkeyKong.current_frame > highFrame || donkeyKong.current_frame < lowFrame) {
		dk_frame_dir = -dk_frame_dir;
	}
}

void setDonkeyKongState(DonkeyKongState state)
{
	donkeyKong.state = state;
}

void setDonkeyKongFrame(float frame)
{
	donkeyKong.current_frame = frame;
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
