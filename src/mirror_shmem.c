#include "kpmodule.h"

static int on_load(void)
{
    kp_log("mirror_shmem loaded");
    return 0;
}

static void on_unload(void)
{
    kp_log("mirror_shmem unloaded");
}

KPM_MODULE("mirror_shmem", "1.0.0", "MIT", "you", "demo", on_load, on_unload);
