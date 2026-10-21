#include <stdint.h>
#include "include/boot_sequence.h"
#include "include/console.h"

#define BOOT_BLACK 0xFF000000u
#define BOOT_WHITE 0xFFFFFFFFu

/* Freestanding startup UI: black background, white text. */
static void short_pause(void)
{
    for (volatile uint32_t i = 0; i < 18000000u; ++i) {
        __asm__ volatile ("" ::: "memory");
    }
}

void boot_sequence_run(void)
{
    static const char *const stages[] = {
        "INITIALIZING FIRMWARE...",
        "CHECKING MEMORY...",
        "STARTING KERNEL...",
        "INITIALIZING DISPLAY...",
        "PREPARING SYSTEM...",
        "STARTUP COMPLETE!"
    };
    static const char *const bars[] = {
        ">", ">>>> ", ">>>>>>>> ", ">>>>>>>>>>>> ",
        ">>>>>>>>>>>>>>>> ", ">>>>>>>>>>>>>>>>>>>>",
    };

    console_set_colors(BOOT_BLACK, BOOT_WHITE);
    for (uint32_t step = 0; step < 6; ++step) {
        console_clear();
        console_write("\n\n       CUSTOM OS\n");
        console_write("       ----------\n\n");
        console_write("       SYSTEM STARTUP\n\n");
        console_write("       PROGRESS: ");
        console_write(bars[step]);
        console_write("\n\n       ");
        console_write(stages[step]);
        console_write("\n");
        short_pause();
    }
}
