#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "camera/dcmi_camera.h"
#include "memory_protection.h"
#include <main.h>
#include "camera/camera.h"
#include "i2c_bus.h"
#include "selector.h"

#define RGB_PANEL_ADDR 88

static THD_WORKING_AREA(selector_thd_wa, 2048);

static THD_FUNCTION(selector_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    systime_t time;
    uint8_t r_val = 0, g_val = 0, b_val = 0;
    uint8_t *img_buff_ptr;
    uint32_t r_sum, g_sum, b_sum;
    uint16_t x, y, i;
    uint16_t cam_width = 20, cam_height = 20;

    srand(chSysGetRealtimeCounterX());

	cam_advanced_config(FORMAT_COLOR, (640-cam_width)/2, (480-cam_height)/2, cam_width, cam_height, SUBSAMPLING_X1, SUBSAMPLING_X1);
	cam_set_exposure(512, 0);
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1) {
    	time = chVTGetSystemTime();

    	switch(get_selector()) {
    		case 0: // Generate a random color for each cell @ 1 Hz.
    			write_reg(RGB_PANEL_ADDR, 0x00, (rand()%129));	// R0
    			write_reg(RGB_PANEL_ADDR, 0x08, (rand()%129));	// B0
    			write_reg(RGB_PANEL_ADDR, 0x10, (rand()%129));	// G0
    			write_reg(RGB_PANEL_ADDR, 0x01, (rand()%129));	// R1
    			write_reg(RGB_PANEL_ADDR, 0x09, (rand()%129));	// B1
    			write_reg(RGB_PANEL_ADDR, 0x11, (rand()%129));	// G1
    			write_reg(RGB_PANEL_ADDR, 0x02, (rand()%129));	// R2
    			write_reg(RGB_PANEL_ADDR, 0x0A, (rand()%129));	// B2
    			write_reg(RGB_PANEL_ADDR, 0x12, (rand()%129));	// G2
    			write_reg(RGB_PANEL_ADDR, 0x03, (rand()%129));	// R3
    			write_reg(RGB_PANEL_ADDR, 0x0B, (rand()%129));	// B3
    			write_reg(RGB_PANEL_ADDR, 0x13, (rand()%129));	// G3
    			write_reg(RGB_PANEL_ADDR, 0x04, (rand()%129));	// R4
    			write_reg(RGB_PANEL_ADDR, 0x0C, (rand()%129));	// B4
    			write_reg(RGB_PANEL_ADDR, 0x14, (rand()%129));	// G4
    			write_reg(RGB_PANEL_ADDR, 0x05, (rand()%129));	// R5
    			write_reg(RGB_PANEL_ADDR, 0x0D, (rand()%129));	// B5
    			write_reg(RGB_PANEL_ADDR, 0x15, (rand()%129));	// G5
    			write_reg(RGB_PANEL_ADDR, 0x06, (rand()%129));	// R6
    			write_reg(RGB_PANEL_ADDR, 0x0E, (rand()%129));	// B6
    			write_reg(RGB_PANEL_ADDR, 0x16, (rand()%129));	// G6
    			write_reg(RGB_PANEL_ADDR, 0x07, (rand()%129));	// R7
    			write_reg(RGB_PANEL_ADDR, 0x0F, (rand()%129));	// B7
    			write_reg(RGB_PANEL_ADDR, 0x17, (rand()%129));	// G7
    			write_reg(RGB_PANEL_ADDR, 0x92, (rand()%129));	// R8
    			write_reg(RGB_PANEL_ADDR, 0x93, (rand()%129));	// B8
    			write_reg(RGB_PANEL_ADDR, 0x94, (rand()%129));	// G8
    			write_reg(RGB_PANEL_ADDR, 0x91, 0x01); // Update colors
    			chThdSleepUntilWindowed(time, time + MS2ST(1000)); // Refresh @ 1 Hz.
    			break;

    		case 1: // "Chamaleon" behavior: take a small picture and reflect the main color on the RGB panel.
    			dcmi_capture_start();
    			wait_image_ready();
    			img_buff_ptr = cam_get_last_image_ptr();

    			// Take the average of the pixels color to be the resulting color shown on the RGB panel.
    			i = 0;
    			r_sum = 0;
    			g_sum = 0;
    			b_sum = 0;;
				for(y=0; y<cam_height; y++) {
					for(x=0; x<cam_width; x++) {
						r_sum += (int)img_buff_ptr[i*2]&0xF8;
						g_sum += (int)(img_buff_ptr[i*2]&0x07)<<5 | (img_buff_ptr[i*2+1]&0xE0)>>3;
						b_sum += (int)(img_buff_ptr[i*2+1]&0x1F)<<3;
		                i++;
					}
				}
				r_val = r_sum/(cam_height*cam_width);
				b_val = b_sum/(cam_height*cam_width);
				g_val = g_sum/(cam_height*cam_width);

    			write_reg(RGB_PANEL_ADDR, 0x00, r_val);	// R0
    			write_reg(RGB_PANEL_ADDR, 0x08, b_val);	// B0
    			write_reg(RGB_PANEL_ADDR, 0x10, g_val);	// G0
    			write_reg(RGB_PANEL_ADDR, 0x01, r_val);	// R1
    			write_reg(RGB_PANEL_ADDR, 0x09, b_val);	// B1
    			write_reg(RGB_PANEL_ADDR, 0x11, g_val);	// G1
    			write_reg(RGB_PANEL_ADDR, 0x02, r_val);	// R2
    			write_reg(RGB_PANEL_ADDR, 0x0A, b_val);	// B2
    			write_reg(RGB_PANEL_ADDR, 0x12, g_val);	// G2
    			write_reg(RGB_PANEL_ADDR, 0x03, r_val);	// R3
    			write_reg(RGB_PANEL_ADDR, 0x0B, b_val);	// B3
    			write_reg(RGB_PANEL_ADDR, 0x13, g_val);	// G3
    			write_reg(RGB_PANEL_ADDR, 0x04, r_val);	// R4
    			write_reg(RGB_PANEL_ADDR, 0x0C, b_val);	// B4
    			write_reg(RGB_PANEL_ADDR, 0x14, g_val);	// G4
    			write_reg(RGB_PANEL_ADDR, 0x05, r_val);	// R5
    			write_reg(RGB_PANEL_ADDR, 0x0D, b_val);	// B5
    			write_reg(RGB_PANEL_ADDR, 0x15, g_val);	// G5
    			write_reg(RGB_PANEL_ADDR, 0x06, r_val);	// R6
    			write_reg(RGB_PANEL_ADDR, 0x0E, b_val);	// B6
    			write_reg(RGB_PANEL_ADDR, 0x16, g_val);	// G6
    			write_reg(RGB_PANEL_ADDR, 0x07, r_val);	// R7
    			write_reg(RGB_PANEL_ADDR, 0x0F, b_val);	// B7
    			write_reg(RGB_PANEL_ADDR, 0x17, g_val);	// G7
    			write_reg(RGB_PANEL_ADDR, 0x92, r_val);	// R8
    			write_reg(RGB_PANEL_ADDR, 0x93, b_val);	// B8
    			write_reg(RGB_PANEL_ADDR, 0x94, g_val);	// G8
    			write_reg(RGB_PANEL_ADDR, 0x91, 0x01); // Update colors
    			chThdSleepUntilWindowed(time, time + MS2ST(200)); // Refresh @ 5 Hz.
    			break;

    		case 2: // Generate a random color (the same for all cells) @ 1 Hz.
    			r_val = rand() % 129;
    			g_val = rand() % 129;
    			b_val = rand() % 129;
    			write_reg(RGB_PANEL_ADDR, 0x00, r_val);	// R0
    			write_reg(RGB_PANEL_ADDR, 0x08, b_val);	// B0
    			write_reg(RGB_PANEL_ADDR, 0x10, g_val);	// G0
    			write_reg(RGB_PANEL_ADDR, 0x01, r_val);	// R1
    			write_reg(RGB_PANEL_ADDR, 0x09, b_val);	// B1
    			write_reg(RGB_PANEL_ADDR, 0x11, g_val);	// G1
    			write_reg(RGB_PANEL_ADDR, 0x02, r_val);	// R2
    			write_reg(RGB_PANEL_ADDR, 0x0A, b_val);	// B2
    			write_reg(RGB_PANEL_ADDR, 0x12, g_val);	// G2
    			write_reg(RGB_PANEL_ADDR, 0x03, r_val);	// R3
    			write_reg(RGB_PANEL_ADDR, 0x0B, b_val);	// B3
    			write_reg(RGB_PANEL_ADDR, 0x13, g_val);	// G3
    			write_reg(RGB_PANEL_ADDR, 0x04, r_val);	// R4
    			write_reg(RGB_PANEL_ADDR, 0x0C, b_val);	// B4
    			write_reg(RGB_PANEL_ADDR, 0x14, g_val);	// G4
    			write_reg(RGB_PANEL_ADDR, 0x05, r_val);	// R5
    			write_reg(RGB_PANEL_ADDR, 0x0D, b_val);	// B5
    			write_reg(RGB_PANEL_ADDR, 0x15, g_val);	// G5
    			write_reg(RGB_PANEL_ADDR, 0x06, r_val);	// R6
    			write_reg(RGB_PANEL_ADDR, 0x0E, b_val);	// B6
    			write_reg(RGB_PANEL_ADDR, 0x16, g_val);	// G6
    			write_reg(RGB_PANEL_ADDR, 0x07, r_val);	// R7
    			write_reg(RGB_PANEL_ADDR, 0x0F, b_val);	// B7
    			write_reg(RGB_PANEL_ADDR, 0x17, g_val);	// G7
    			write_reg(RGB_PANEL_ADDR, 0x92, r_val);	// R8
    			write_reg(RGB_PANEL_ADDR, 0x93, b_val);	// B8
    			write_reg(RGB_PANEL_ADDR, 0x94, g_val);	// G8
    			write_reg(RGB_PANEL_ADDR, 0x91, 0x01); // Update colors
    			chThdSleepUntilWindowed(time, time + MS2ST(1000)); // Refresh @ 1 Hz.
    			break;

    		default:
    			break;
    	}
    }

}

int main(void) {

    halInit();
    chSysInit();
    mpu_init();
    dcmi_start();
    cam_start();

    chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void) {
    chSysHalt("Stack smashing detected");
}
