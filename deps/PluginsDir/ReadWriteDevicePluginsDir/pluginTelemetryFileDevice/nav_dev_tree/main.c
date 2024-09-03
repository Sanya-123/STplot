#include "DV/inc/PEK_DV_IntEn.h"
#include "DV/inc/PEK_DV_NS.h"
#include "DV/inc/PEK_DV_inclinometer.h"
#include "DV/inc/PEK_DV_mower.h"
#include "DV/inc/PEK_DV_HE.h"
#include "DV/inc/PEK_DV_TeraAir.h"
#include "DV/inc/PEK_DV_power_unit.h"
#include "DV/inc/PEK_DV_TeraAir.h"

typedef struct __attribute__ ((__packed__))
{
    DV_IntEn_t inten_traction;
    DV_IntEn_t inten_cargo;
    DV_inclin_t inclin_body;
    DV_NS_t 	navigation_unit;
    DV_inclin_t inclin_platform;
    DV_TeraAir_t teroair;
    DV_HE_t		HE_front;
    DV_HE_t		HE_rear;
    DV_PU_t		power_unit;
}Nav_SH_Dev_tree_t;

__attribute__ ((__unused__,section(".nav_dev_tree"))) Nav_SH_Dev_tree_t SH_dev_tree;


int main()
{
    return 0;
}
