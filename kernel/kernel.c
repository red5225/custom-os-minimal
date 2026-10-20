#include <stdint.h>
#include "include/bootinfo.h"
#include "include/console.h"
#include "include/serial.h"

void shell_run(void);

/* This is the kernel's entry point after boot.S switches to 64-bit mode. */
void kernel_main(uint32_t boot_magic, uint32_t boot_info_address) {
    struct boot_framebuffer framebuffer = {0};
    const struct boot_framebuffer *screen = 0;

    serial_init();
    if (bootinfo_find_framebuffer(boot_magic, boot_info_address, &framebuffer))
        screen = &framebuffer;

    console_init(screen);
    serial_write("KERNEL_READY\nCUSTOM OS MINIMAL\n");
    shell_run();
}
