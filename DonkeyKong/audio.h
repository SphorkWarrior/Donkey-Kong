/*
 * level1.h
 *
 *  Created on: Sep 24, 2013
 *      Author: Christopher (Viewtiful) Tan
 */

#ifndef AUDIO_H_
#define AUDIO_H_
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_sd_card_avalon_interface.h"
//#include "sdcard.c"

#define FIFO_SIZE 112
#define AUDIO_ERROR -1


int playMusic(char* audioFile);

#endif /* AUDIO_H_ */