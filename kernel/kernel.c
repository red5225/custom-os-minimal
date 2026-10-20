#include <stdint.h>

#include "include/bootinfo.h"
#include "include/console.h"
#include "include/serial.h"

void shell_run(void);

/* The C entry point called by kernel/boot.S. */
void kernel_main(uint32_t boot_magic, uint32_t boot_info_address)
{
    /* Direct COM1 marker before serial_init or other C work. */
    __asm__ volatile (
        "movb $'F', %%al\n\t"
        "movw $0x3F8, %%dx\n\t"
        "outb %%al, %%dx\n\t"
        : : : "rax", "rdx", "memory"
    );

    struct boot_framebuffer framebuffer = {0};
    const struct boot_framebuffer *screen = 0;

    serial_init();
    serial_write("Kernel entered C code.\\n");

    if (bootinfo_find_framebuffer(boot_magic, boot_info_address,
                                  &framebuffer)) {
        screen = &framebuffer;
        serial_write("Framebuffer information found.\\n");
    } else {
        serial_write("No compatible framebuffer; using VGA fallback.\\n");
    }

    console_init(screen);
    serial_write("KERNEL_READY\\nCUSTOM OS MINIMAL\\n");
    shell_run();
}
