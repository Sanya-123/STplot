#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stlink.h"
// #include <chipid.h>
// #include <helper.h>
#include <usb.h>

int main(){
    stlink_t* sl = NULL;
    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(0, connect, NULL, freq);
    if (sl == NULL){ 
        return(-1); 
    }
    stlink_enter_swd_mode(sl);
    uint32_t test = stlink_read_mem32(sl, 0x20000000,4); //first data read will return 00 00 00 00
    printf("0x%.4x\n", sl->chip_id);
    sl->verbose = 90;

    test = stlink_read_mem32(sl, 0x20000000,4);
    
    if (sl) {
        stlink_exit_debug_mode(sl);
        stlink_close(sl);
    }
    return(0);
}