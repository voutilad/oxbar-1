#ifndef VOLUME_H
#define VOLUME_H

#include <stdbool.h>

typedef struct volume_info {
   bool  is_setup;

   /* TODO GET VOLUME "IS MUTED?" WORKING */
   bool  muted;
   float left_pct, right_pct;

} volume_info_t;

extern volume_info_t VOLUME;

void volume_init();
void volume_update();
void volume_close();

#endif
