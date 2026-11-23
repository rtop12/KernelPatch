#include <kpm.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/pid.h>
#include <asm/page.h>

#define MAX_PAGES 1024

typedef struct task_struct* (*find_task_by_vpid_t)(pid_t vnr);
typedef struct mm_struct* (*get_task_mm_t)(struct task_struct *task);
typedef void (*mmput_t)(struct mm_struct *mm);
typedef void (*mmap_read_lock_t)(struct mm_struct *mm);
typedef void (*mmap_read_unlock_t)(struct mm_struct *mm);
typedef void (*mmap_write_lock_t)(struct mm_struct *mm);
typedef void (*mmap_write_unlock_t)(struct mm_struct *mm);

typedef struct page* (*follow_page_t)(struct vm_area_struct *vma,
                                      unsigned long address,
                                      unsigned int foll_flags);
typedef struct vm_area_struct* (*find_vma_t)(struct mm_struct *mm, unsigned long addr);
typedef unsigned long (*do_mmap_t)(struct file *file, unsigned long addr,
                                   unsigned long len, unsigned long prot,
                                   unsigned long flags, unsigned long pgoff);
typedef int (*do_munmap_t)(struct mm_struct *mm, unsigned long start,
                           size_t len, void *uf);
typedef int (*remap_pfn_range_t)(struct vm_area_struct *vma, unsigned long addr,
                                 unsigned long pfn, unsigned long size, pgprot_t prot);
typedef void (*put_page_t)(struct page *page);

static find_task_by_vpid_t   k_find_task_by_vpid;
static get_task_mm_t         k_get_task_mm;
static mmput_t               k_mmput;
static mmap_read_lock_t      k_mmap_read_lock;
static mmap_read_unlock_t    k_mmap_read_unlock;
static mmap_write_lock_t     k_mmap_write_lock;
static mmap_write_unlock_t   k_mmap_write_unlock;

static follow_page_t         k_follow_page;
static find_vma_t            k_find_vma;
static do_mmap_t             k_do_mmap;
static do_munmap_t           k_do_munmap;
static remap_pfn_range_t     k_remap_pfn_range;
static put_page_t            k_put_page;

static struct page *g_page_array[MAX_PAGES];
static int g_page_num = 0;
static unsigned long g_mirror_addr = 0;
static unsigned long g_mirror_size = 0;

static int resolve_symbols(void)
{
    k_find_task_by_vpid = (find_task_by_vpid_t)kallsyms_lookup_name("find_task_by_vpid");
    k_get_task_mm       = (get_task_mm_t)kallsyms_lookup_name("get_task_mm");
    k_mmput             = (mmput_t)kallsyms_lookup_name("mmput");

    k_mmap_read_lock    = (mmap_read_lock_t)kallsyms_lookup_name("mmap_read_lock");
    k_mmap_read_unlock  = (mmap_read_unlock_t)kallsyms_lookup_name("mmap_read_unlock");
    k_mmap_write_lock   = (mmap_write_lock_t)kallsyms_lookup_name("mmap_write_lock");
    k_mmap_write_unlock = (mmap_write_unlock_t)kallsyms_lookup_name("mmap_write_unlock");
    if (!k_mmap_read_lock) {
        k_mmap_read_lock    = (mmap_read_lock_t)kallsyms_lookup_name("down_read");
        k_mmap_read_unlock  = (mmap_read_unlock_t)kallsyms_lookup_name("up_read");
        k_mmap_write_lock   = (mmap_write_lock_t)kallsyms_lookup_name("down_write");
        k_mmap_write_unlock = (mmap_write_unlock_t)kallsyms_lookup_name("up_write");
    }

    k_follow_page       = (follow_page_t)kallsyms_lookup_name("follow_page");
    k_find_vma          = (find_vma_t)kallsyms_lookup_name("find_vma");
    k_do_mmap           = (do_mmap_t)kallsyms_lookup_name("do_mmap");
    k_do_munmap         = (do_munmap_t)kallsyms_lookup_name("do_munmap");
    k_remap_pfn_range   = (remap_pfn_range_t)kallsyms_lookup_name("remap_pfn_range");
    k_put_page          = (put_page_t)kallsyms_lookup_name("put_page");

    if (!k_find_task_by_vpid || !k_get_task_mm || !k_mmput ||
        !k_mmap_read_lock || !k_mmap_read_unlock ||
        !k_mmap_write_lock || !k_mmap_write_unlock ||
        !k_follow_page || !k_find_vma || !k_do_mmap ||
        !k_do_munmap || !k_remap_pfn_range || !k_put_page) {
        return -1;
    }
    return 0;
}

static void free_mirror(void)
{
    if (g_mirror_addr != 0 && g_mirror_size != 0) {
        k_mmap_write_lock(current->mm);
        k_do_munmap(current->mm, g_mirror_addr, g_mirror_size, NULL);
        k_mmap_write_unlock(current->mm);
        g_mirror_addr = 0;
        g_mirror_size = 0;
    }

    for (int i = 0; i < g_page_num; i++) {
        if (g_page_array[i]) {
            k_put_page(g_page_array[i]);
            g_page_array[i] = NULL;
        }
    }
    g_page_num = 0;
}

static long create_mirror(pid_t pid, unsigned long target_addr, unsigned long size)
{
    if (pid <= 0 || size == 0)
        return -EINVAL;

    free_mirror();

    unsigned long start = PAGE_ALIGN_DOWN(target_addr);
    unsigned long end   = PAGE_ALIGN(target_addr + size);
    unsigned long len   = end - start;
    int nr_pages = len / PAGE_SIZE;

    if (nr_pages > MAX_PAGES)
        return -ENOMEM;

    struct task_struct *task = k_find_task_by_vpid(pid);
    if (!task) return -ESRCH;

    struct mm_struct *target_mm = k_get_task_mm(task);
    if (!target_mm) return -EINVAL;

    int ret = -ENOMEM;
    struct vm_area_struct *vma;

    k_mmap_read_lock(target_mm);
    for (int i = 0; i < nr_pages; i++) {
        unsigned long addr = start + i * PAGE_SIZE;
        vma = k_find_vma(target_mm, addr);
        if (!vma) goto out_target_read;

        struct page *page = k_follow_page(vma, addr, FOLL_GET);
        if (!page) goto out_target_read;

        g_page_array[i] = page;
    }
    g_page_num = nr_pages;
    k_mmap_read_unlock(target_mm);
    k_mmput(target_mm);

    k_mmap_write_lock(current->mm);
    unsigned long mirror_addr = k_do_mmap(NULL, 0, len, PROT_READ,
                                          MAP_PRIVATE | MAP_ANONYMOUS, 0);
    if (IS_ERR_VALUE(mirror_addr)) {
        ret = (int)mirror_addr;
        goto out_free_pages;
    }

    vma = k_find_vma(current->mm, mirror_addr);
    if (!vma) {
        k_do_munmap(current->mm, mirror_addr, len, NULL);
        goto out_free_pages;
    }

    for (int i = 0; i < nr_pages; i++) {
        unsigned long pfn = page_to_pfn(g_page_array[i]);
        ret = k_remap_pfn_range(vma, mirror_addr + i * PAGE_SIZE,
                                pfn, PAGE_SIZE, PAGE_READONLY);
        if (ret < 0) {
            k_do_munmap(current->mm, mirror_addr, len, NULL);
            goto out_free_pages;
        }
    }

    g_mirror_addr = mirror_addr;
    g_mirror_size = len;
    ret = 0;
    k_mmap_write_unlock(current->mm);

    return (long)mirror_addr;

out_free_pages:
    k_mmap_write_unlock(current->mm);
    free_mirror();
    return ret;

out_target_read:
    k_mmap_read_unlock(target_mm);
    k_mmput(target_mm);
    free_mirror();
    return -EFAULT;
}

static long mirror_ctl0(const char *args, const char *event, void __user *reserved)
{
    if (!args) return -EINVAL;

    if (strncmp(args, "map ", 4) == 0) {
        char *p = (char *)args + 4;
        pid_t pid = (pid_t)simple_strtol(p, &p, 10);
        unsigned long addr = simple_strtoul(p, &p, 16);
        unsigned long size = simple_strtoul(p, &p, 10);
        return create_mirror(pid, addr, size);
    }
    else if (strcmp(args, "unmap") == 0) {
        free_mirror();
        return 0;
    }

    return -EINVAL;
}

static long mirror_init(const char *args, const char *event, void __user *reserved)
{
    if (resolve_symbols() != 0)
        return -ENODEV;

    g_mirror_addr = 0;
    g_mirror_size = 0;
    g_page_num = 0;
    memset(g_page_array, 0, sizeof(g_page_array));
    return 0;
}

static long mirror_exit(void __user *reserved)
{
    free_mirror();
    return 0;
}

KPM_INIT(mirror_init);
KPM_EXIT(mirror_exit);
KPM_CTL0(mirror_ctl0);
