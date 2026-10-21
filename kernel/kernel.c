#include <stdint.h>
#include "include/bootinfo.h"
#include "include/console.h"
#include "include/serial.h"
#include "include/boot_sequence.h"
void shell_run(void);
void kernel_main(uint32_t boot_magic,uint32_t boot_info_address){
 __asm__ volatile("movb $'C', %%al\n\t movw $0x3F8, %%dx\n\t outb %%al, %%dx" : : : "rax","rdx","memory");
 struct boot_framebuffer framebuffer={0}; const struct boot_framebuffer *screen=0;
 serial_init(); serial_write("Kernel entered C code.\n");
 if(bootinfo_find_framebuffer(boot_magic,boot_info_address,&framebuffer)){screen=&framebuffer;serial_write("Framebuffer information found.\n");}
 else serial_write("No compatible framebuffer; using VGA fallback.\n");
 console_init(screen); serial_write("KERNEL_READY\nCUSTOM OS MINIMAL\n");
 boot_sequence_run();
 console_set_colors(0xFFF28C28u,0xFF000000u); console_clear();
 console_write("CUSTOM OS MINIMAL\n-----------------\n");
 console_write("Kernel ready. Type HELP for commands.\n\n"); shell_run();
}
