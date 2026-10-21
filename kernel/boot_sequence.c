#include <stdint.h>
#include "include/boot_sequence.h"
#include "include/console.h"

/*
 * Native, freestanding C version of the os-ios-macos-alike boot concept.
 * The desktop prototype uses SDL2, which cannot run inside this kernel.
 * This version uses the kernel console and has no hosted-library dependency.
 */
static void short_pause(void)
{
    for (volatile uint32_t i = 0; i < 350000u; ++i) {
        __asm__ volatile ("" ::: "memory");
    }
}

void boot_sequence_run(void)
{
    static const char *const stages[] = {
        "INITIALIZING FIRMWARE",
        "CHECKING MEMORY",
        "STARTING KERNEL",
        "INITIALIZING DISPLAY",
        "PREPARING SYSTEM",
        "STARTUP COMPLETE"
    };
    static const char *const bars[] = {
        ">",
        ">>>>",
        ">>>>>>>>",
        ">>>>>>>>>>>>",
        ">>>>>>>>>>>>>>>>",
        ">>>>>>>>>>>>>>>>>>>>"
    };

    for (uint32_t step = 0; step < 6; ++step) {
        console_clear();
        console_write("\n\n");
        console_write("       CUSTOM OS\n");
        console_write("       ----------\n\n");
        console_write("       SYSTEM STARTUP\n\n");
        console_write("       ");
        console_write(bars[step]);
        console_write("\n\n       ");
        console_write(stages[step]);
        console_write("\n");
        short_pause();
    }
}
