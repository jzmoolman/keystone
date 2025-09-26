#include <sbi/sbi_platform.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/fdt/fdt_driver.h>


const struct sbi_platform generic = {
    .name       = "dummy",
    .features   = SBI_PLATFORM_DEFAULT_FEATURES,
    .hart_count = 1,
};

static int my_platform_init(const void *fdt, int nodeoff,
                         const struct fdt_match *match)
{
    sbi_printf("MyBoard: custom final_init override (OpenSBI 1.7 style)\n");

    if (match && match->compatible)
        sbi_printf("FDT compatible: %s\n", match->compatible);

    return 0;
}


static const struct fdt_match my_platform_match[] = {
    { .compatible = "riscv-virt" },
    { /* sentinel */ }
};

/* --- FDT driver --- */
struct fdt_driver my_platform_driver = {
    .match_table = my_platform_match,
    .init        = my_platform_init,   /* <-- replaces old final_init */
};

