#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <egb.h>
#include <snd.h>
#include <msdos.cf>

char work[EgbWorkSize];
char    para[16];

#define FIRE_WIDTH 320
#define FIRE_HEIGHT 160
#define NUM_COLORS 37

unsigned char rgb_pallete[NUM_COLORS][3] = {
    { 0x07,0x07,0x07 },
    { 0x1F,0x07,0x07 },
    { 0x2F,0x0F,0x07 },
    { 0x47,0x0F,0x07 },
    { 0x57,0x17,0x07 },
    { 0x67,0x1F,0x07 },
    { 0x77,0x1F,0x07 },
    { 0x8F,0x27,0x07 },
    { 0x9F,0x2F,0x07 },
    { 0xAF,0x3F,0x07 },
    { 0xBF,0x47,0x07 },
    { 0xC7,0x47,0x07 },
    { 0xDF,0x4F,0x07 },
    { 0xDF,0x57,0x07 },
    { 0xDF,0x57,0x07 },
    { 0xD7,0x5F,0x07 },
    { 0xD7,0x5F,0x07 },
    { 0xD7,0x67,0x0F },
    { 0xCF,0x6F,0x0F },
    { 0xCF,0x77,0x0F },
    { 0xCF,0x7F,0x0F },
    { 0xCF,0x87,0x17 },
    { 0xC7,0x87,0x17 },
    { 0xC7,0x8F,0x17 },
    { 0xC7,0x97,0x1F },
    { 0xBF,0x9F,0x1F },
    { 0xBF,0x9F,0x1F },
    { 0xBF,0xA7,0x27 },
    { 0xBF,0xA7,0x27 },
    { 0xBF,0xAF,0x2F },
    { 0xB7,0xAF,0x2F },
    { 0xB7,0xB7,0x2F },
    { 0xB7,0xB7,0x37 },
    { 0xCF,0xCF,0x6F },
    { 0xDF,0xDF,0x9F },
    { 0xEF,0xEF,0xC7 },
    { 0xFF,0xFF,0xFF },
};

unsigned char fire_pixels[FIRE_HEIGHT * FIRE_WIDTH];

int chatter = 0;

int rand_num = 1;

void start_fire() {
    memset(&fire_pixels[(FIRE_HEIGHT - 1) * FIRE_WIDTH], NUM_COLORS - 1, FIRE_WIDTH);
}

void stop_fire() {
    for (int i = 0; i < FIRE_WIDTH; i++) {
        if (fire_pixels[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] > 0)
            fire_pixels[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] -= 2;
    }
}

void spread_fire() {
    int from, random;
    from = FIRE_WIDTH;
    for (int i = 1; i < FIRE_HEIGHT; i++) {
        for (int y = 0; y < FIRE_WIDTH; y++) {
            if (fire_pixels[from] == 0) {
                fire_pixels[from - FIRE_WIDTH] = 0;
            } else {
                rand_num = rand_num * 1103515245 + 12345;
                random = (rand_num & 2147483647) % 3;
                fire_pixels[(from - random + 1) - FIRE_WIDTH] = fire_pixels[from] - (random & 1);
            }
            from++;
        }
    }
}

void setup() {

    //Screen set Mode12(640*480,256 Colors, 1 Page only)
    EGB_resolution( work, 0, 12 );

    ///zoom, 640*480->320*160
    EGB_displayStart( work, 2, 2, 3 );
    EGB_displayStart( work, 0, 0, 0 );

    EGB_displayPage( work, 0, 3 );
    EGB_writePage( work, 0 );

    EGB_color( work, 0, 0x00 );
    EGB_clearScreen( work );

    // pallete
    for (int i = 0;i < NUM_COLORS;i++)
    {
        _outb( 0xfd90, i );
        _outb( 0xfd94, rgb_pallete[i][0] );
        _outb( 0xfd96, rgb_pallete[i][1] );
        _outb( 0xfd92, rgb_pallete[i][2] );
    }

    // clear fire map
    memset(fire_pixels, 0, sizeof(fire_pixels));

    start_fire();

    DWORD( para+0  ) = (unsigned int)fire_pixels;
    WORD( para+4  ) = getds();
    WORD( para+6  ) = 0;
    WORD( para+8  ) = 0;
    WORD( para+10 ) = FIRE_WIDTH - 1;
    WORD( para+12 ) = FIRE_HEIGHT - 1;
}

int doomfire_input() {
    int joy;

    SND_joy_in_1(0, &joy);
    joy &= 0x3f;

    if(joy == 0x3f || chatter == 1)
    {
        chatter = 0;
        return 1;
    }

    switch(joy)
    {
        case 0x1f:
            start_fire();
            break;
        case 0x2f:
            stop_fire();
            break;
        case 0x33:
            return 0;
            break;
     }

    chatter = 1;

    return 1;
}

void draw_fire() {
    //char    para[16];

    //DWORD( para+0  ) = (unsigned int)fire_pixels;
    //WORD( para+4  ) = getds();
    //WORD( para+6  ) = 0;
    //WORD( para+8  ) = 0;
    //WORD( para+10 ) = FIRE_WIDTH - 1;
    //WORD( para+12 ) = FIRE_HEIGHT - 1;
    EGB_putBlock( work, 0, para );
}

int main(int argc, char** argv) {
    setup();

    while(doomfire_input())
    {
         spread_fire();
         draw_fire();
    }

    EGB_init( work, EgbWorkSize );

    return 0;
}