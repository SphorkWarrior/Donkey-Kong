/*
 * mainMenu.c
 *
 *  Created on: Oct 8, 2013
 *      Author: singla
 */

#include "mainMenu.h"
#include "bitmap.h"
#include "state_machine.h"
#include "input.h"
#include "background.h"

static BitmapHandle* background_bmp;
static BitmapHandle* cursor_bmp;
static colour back_alpha = { 0x00, 0x00, 0x00 };
extern unsigned char button_states[4];
extern controller_buttons controller_state;
extern controller_buttons prev_controller_state;

void updateMainMenu(void)
{
	// draw the cursor.
    draw_bmp(cursor_bmp, 100, 240 - cursor_bmp->bmp_info_header->height,
    		false, back_alpha, 1);

	// draw it black
	if (!button_states[0] ||
			(!prev_controller_state.START && controller_state.START))
	{
		close_bmp(background_bmp);
		changeState(LOADING_SCREEN);
	}
}

void draw_main_menu(void)
{
	short int ret = load_bmp("MM1.BMP", &background_bmp);
	//short int ret1 = load_bmp("CURSOR.BMP", &cursor_bmp);

	clear_display();
	// Draw the background to both buffers.
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
	swap_buffers();
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
}

