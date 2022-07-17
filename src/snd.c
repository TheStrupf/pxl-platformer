#include "snd.h"
#include "raylib.h"
#include "shared.h"
#include <stdbool.h>

static Sound sounds[64];
static int soundID = -1;
static bool snd_okay = false;

void snd_init()
{
        InitAudioDevice();
        snd_okay = IsAudioDeviceReady();
}

int snd_load(const char *filename)
{
        if (!snd_okay)
                return -1;
        sounds[++soundID] = LoadSound(filename);
        return soundID;
}

void snd_play_sound(int ID)
{
        if (snd_okay && (uint)ID <= soundID)
                PlaySound(sounds[ID]);
}

void snd_destroy()
{
        if (!snd_okay)
                return;
        for (int n = soundID; n >= 0; n--)
                UnloadSound(sounds[n]);
        CloseAudioDevice();
}
