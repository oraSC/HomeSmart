#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "../lib/lcd/LCD.h"
#include "../lib/jpg/JPG.h"
#include "./album.h"




int music(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand);
void play_music(int index);

#endif
