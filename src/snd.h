#ifndef SND_H
#define SND_H

void snd_init();
int snd_load(const char *);
void snd_play_sound(int ID);
void snd_destroy();

#endif