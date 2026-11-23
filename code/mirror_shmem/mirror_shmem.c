#include "kpmodule.h"
#include <linux/types.h>

static int module_load(void)
{
    kp_log("mirror_shmem load success");
    return 0;
}

static void module_unload(void)
{
    kp_log("mirror_shmem unload");
}

KPM_MODULE(
    "mirror_shmem",
    "1.0.0",
    "MIT",
    "rtop12",
    "memory mirror demo",
    module_load,
    module_unload
);
