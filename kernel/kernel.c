#include <stdint.h>

#include "include/bootinfo.h"
#include "include/console.h"
#include "include/serial.h"
#include "include/boot_sequence.h"

void shell_run(void);

/* C entry point called by kernel/boot.S. */
void kernel_main(uint32_t boot_magic, uint32_t boot_info_address)
{
    /* Emit an early marker before any subsystem initialization. */
    __asm__ volatile (
        "movb $'C', %%al\n\t"
        "movw $0x3F8, %%dx\n\t"
        "outb %%al, %%dx\n\t"
        : : : "rax", "rdx", "memory"
    );

    struct boot_framebuffer framebuffer = {0};
    const struct boot_framebuffer *screen = 0;

    serial_init();
    serial_write("Kernel entered C code.\n");

    if (bootinfo_find_framebuffer(boot_magic, boot_info_address,
                                  &framebuffer)) {
        screen = &framebuffer;
        serial_write("Framebuffer information found.\n");
    } else {
        serial_write("No compatible framebuffer; using VGA fallback.\n");
    }

    console_init(screen);
    serial_write("KERNEL_READY\nCUSTOM OS MINIMAL\n");

    /* Freestanding C port of the macOS-alike boot animation design.
       SDL2 is intentionally not linked into the kernel. */
    boot_sequence_run();

    console_clear();
    console_write("CUSTOM OS MINIMAL\n");
    console_write("-----------------\n");
    console_write("Kernel ready. Type HELP for commands.\n\n");
    shell_run();
}
