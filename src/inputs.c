#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lib_gpio.h"

#define READ_SW() (gpio_get_input(SUNXI_PORT_E_BASE, SUNXI_PIO_11) ? 1 : 0)
#define READ_A() (gpio_get_input(SUNXI_PORT_D_BASE, SUNXI_PIO_04) ? 1 : 0)
#define READ_B() (gpio_get_input(SUNXI_PORT_D_BASE, SUNXI_PIO_03) ? 1 : 0)

#define READ_B1() (gpio_get_input(SUNXI_PORT_B_BASE, SUNXI_PIO_04) ? 1 : 0)
#define READ_B2() (gpio_get_input(SUNXI_PORT_B_BASE, SUNXI_PIO_10) ? 1 : 0)
#define READ_B3() (gpio_get_input(SUNXI_PORT_G_BASE, SUNXI_PIO_12) ? 1 : 0)

#define READ_BP() (gpio_get_input(SUNXI_PORT_C_BASE, SUNXI_PIO_03) ? 1 : 0)

int8_t updateDial();
uint8_t readButtons();

int encoderPos = 0;

void setup() {
    setvbuf(stdout, NULL, _IOLBF, 0);

    gpio_init();
    gpio_cfg_input(SUNXI_PORT_E_BASE, SUNXI_PIO_11_IDX);
    gpio_cfg_input(SUNXI_PORT_D_BASE, SUNXI_PIO_03_IDX);
    gpio_cfg_input(SUNXI_PORT_D_BASE, SUNXI_PIO_04_IDX);

    gpio_cfg_input(SUNXI_PORT_B_BASE, SUNXI_PIO_04_IDX);
    gpio_cfg_input(SUNXI_PORT_B_BASE, SUNXI_PIO_10_IDX);
    gpio_cfg_input(SUNXI_PORT_G_BASE, SUNXI_PIO_12_IDX);

    gpio_cfg_input(SUNXI_PORT_C_BASE, SUNXI_PIO_03_IDX);
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int main(int argc, char **argv) {
    do {
        setup();
        usleep(100000);
    } while(readButtons());

    int16_t i, active;
    uint8_t dial, button, lastButton = 0;
    while(!kbhit()) {
        for(i = 0; i < 5000; i++)
        {
            if(dial = updateDial()) {
                if(dial == 1)
                    printf("D=D\n");
                else
                    printf("D=U\n");
            }
            button = readButtons();
            if(button != lastButton) {
                lastButton = button;
                active = 100;
            }
            if(active > 0)
            {
                active--;
                if(active <= 0) {
                    printf("B=%d\n", button);
                }
                usleep(100);
            }
            else
            {
                usleep(1500);
            }
        }
    }
    printf("Powering down...");
    char buffer[32];
    read(STDIN_FILENO, buffer, 32);
    printf("done.\n");
    return 0;
}

int8_t updateDial() {
    static int aPrev = -1, bPrev = -1;
    uint8_t update = 0;

    int aNow = (READ_A() == 0);
    int bNow = (READ_B() == 0);
    if((aNow != aPrev || bNow != bPrev) && bNow && aNow && aPrev >= 0)
    {
        if(!aPrev) {
            update = 1;
            encoderPos++;
        }
        else if(!bPrev) {
            update = -1;
            encoderPos--;     
        } 
    }
    aPrev = aNow;
    bPrev = bNow;
    return update;
}

uint8_t readButtons() {
    uint8_t status = 0;
    if(!READ_B1()) status |= 1<<0;
    if(!READ_B2()) status |= 1<<1;
    if(!READ_B3()) status |= 1<<2;
    if(!READ_SW()) status |= 1<<3;
    if(!READ_BP()) status |= 1<<4;
    return status;
}

