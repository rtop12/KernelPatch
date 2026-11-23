#include "kpmodule.h"
#include <linux/types.h>

static int load(void)
{
    kp_log("mirror shmem loaded!");
    return 0;
}

static void unload(void)
{
    kp_log("mirror shmem unloaded");
}

KPM_MODULE("mirror_shmem", "1.0.0", "MIT", "rtop12", "shared memory demo", load, unload);
