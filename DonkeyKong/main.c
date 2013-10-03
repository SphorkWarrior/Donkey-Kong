#include <stdio.h>
#include "sdcard.h"
#include "bitmap.h"
#include "display.h"
#include "level1.h"
#include "background.h"
#include "priv/alt_busy_sleep.h"
#include "sys/alt_alarm.h"
#include "io.h"
#include "sys/alt_timestamp.h"
#include "mario.h"

#define NUM_FILES 44
// Controller Out: Bits: 000000AB
// A is the Controller P/S latch
// B is the Controller Clock
#define controller_out (volatile char *) CONTROLLER_OUTPUT_BASE

// Controller In: Only least significant bit matters.
#define controller_in (char *) CONTROLLER_INPUT_BASE

#define leds (volatile char *) LEDS_BASE

static char* file_list[NUM_FILES] = { "4B.BMP", "B1.BMP", "B2.BMP", "B3.BMP",
		"B4.BMP", "B5.BMP", "DK1.BMP", "DK2.BMP", "DK3.BMP", "DK4.BMP",
		"DK5.BMP", "DK6.BMP", "DK7.BMP", "DK8.BMP", "DK9.BMP", "DK10.BMP",
		"DK11.BMP", "FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP", "FIRE3.BMP",
		"HMR.BMP", "M1.BMP", "M2.BMP", "M3.BMP", "M4.BMP", "M5.BMP", "M6.BMP",
		"M7.BMP", "M8.BMP", "M9.BMP", "M10.BMP", "M11.BMP", "M12.BMP",
		"M13.BMP", "M14.BMP", "M15.BMP", "P1.BMP", "P2.BMP", "PP1.BMP",
		"PP2.BMP", "PP3.BMP", "PURSE.BMP", "UMBRLA.BMP" };
static BitmapHandle* bmp;
static alt_u32 ticks_per_sec;
static alt_u32 num_ticks;
static alt_32 update(void *context);
static void readDat();

static void readDat(){
	unsigned short accumulatedData = 0;
	int i;

	IOWR_8DIRECT(controller_out, 0, 0x01);
	IOWR_8DIRECT(controller_out, 0, 0x03);
	alt_busy_sleep(12);
	IOWR_8DIRECT(controller_out, 0, 0x01);
	alt_busy_sleep(6);

	accumulatedData = IORD_8DIRECT(controller_in, 0);

	for (i = 0; i < 16; i++)
	{
		IOWR_8DIRECT(controller_out, 0, 0x00);
		alt_busy_sleep(6);
		accumulatedData <<= 1;
		accumulatedData += IORD_8DIRECT(controller_in, 0);
		alt_busy_sleep(6);

		IOWR_8DIRECT(controller_out, 0, 0x01); // Pulse clock
	}

	printf("Accumulated Data: %x\n", accumulatedData);
	IOWR_8DIRECT(leds, 0, accumulatedData);
}

int main(void) {
	// Set latch and clock to 0.
	IOWR_8DIRECT(controller_out, 0, 0x00);

	init_display();
	sdcard_handle *sd_dev = init_sdcard();

	clear_display();

	if (sd_dev == NULL)
		return 1;

	printf("Card connected.\n");

	//load_bmp("M1.BMP", &bmp);
	loadMario(0, 5, 1);
	draw_level1();
	drawMario();

	ticks_per_sec = alt_ticks_per_second();

	num_ticks = ticks_per_sec / 30;
	alt_alarm *update_alarm = malloc(sizeof(alt_alarm));
	alt_alarm_start(update_alarm, num_ticks, update, (void*)0);
	// function calls here are tricky to predict
	// printf only worked if only one printf statement was put
	while (true) { }

	return 0;
}

alt_32 update(void *context) {

	//static int x = 0;
	//static int y = 5;
	bool wallmark = true;
	static int dir_x = 1;

	int floor = 0;
	colour col = { 0x1F, 0x00, 0x1F };

/*	if (x > (320 - bmp->bmp_info_header->width) || x < 0) {
		dir_x = -dir_x;
	} else {
		drawBackgroundSection(x - 1, y - 2 , x + bmp->bmp_info_header->width + 1, y - 1);
		drawBackgroundSection(x - 1, y + bmp->bmp_info_header->height, x + bmp->bmp_info_header->width + 1, y + bmp->bmp_info_header->height + 1);
		drawBackgroundSection(x - 1,y , x, y + bmp->bmp_info_header->height );
		drawBackgroundSection(x + bmp->bmp_info_header->width, y ,x + bmp->bmp_info_header->width + 1, y + bmp->bmp_info_header->height);
		swap_buffers();
		drawBackgroundSection(x - 1, y - 2 , x + bmp->bmp_info_header->width + 1, y - 1);
		drawBackgroundSection(x - 1, y + bmp->bmp_info_header->height, x + bmp->bmp_info_header->width + 1, y + bmp->bmp_info_header->height + 1);
		drawBackgroundSection(x - 1,y , x, y + bmp->bmp_info_header->height );
		drawBackgroundSection(x + bmp->bmp_info_header->width, y ,x + bmp->bmp_info_header->width + 1, y + bmp->bmp_info_header->height);
	}*/

/*	draw_bmp(bmp, x, y, true, col, 1);
	swap_buffers();
	draw_bmp(bmp, x, y, true, col, 1);*/

	if (is_ladder(getMario().x,getMario().y)){
		moveMario(UP);
		//y--;
	}
	else {
		//x += dir_x;
		wallmark = (dir_x > 0)? moveMario(RIGHT) : moveMario(LEFT);

		if (wallmark == false) dir_x = -dir_x;
		floor = find_floor(getMario().x, getMario().y) - getCurrentHeight(); // 12 is height of mario image
		if (getMario().y < floor)
			moveMario(DOWN);
			// y++
		if (getMario().y > floor)
			moveMario(UP);
			//y --;
	}

	drawMario();

	return 1;
}
