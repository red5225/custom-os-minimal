#include <stdint.h>
#include "include/boot_sequence.h"
#include "include/console.h"
#define BOOT_BLACK 0xFF000000u
#define BOOT_WHITE 0xFFFFFFFFu
static void emit(const char *s){ console_write(s); console_write("\n"); }
static void pause_brief(void){ for(volatile uint32_t i=0;i<70000u;++i) __asm__ volatile("" ::: "memory"); }
void boot_sequence_run(void){
 static const char *const logs[]={
 "[BOOT] CUSTOM OS MINIMAL KERNEL ENTRY",
 "[BOOT] MULTIBOOT2 HANDOFF RECEIVED",
 "[KERNEL] ENTERED FREESTANDING C RUNTIME",
 "[SERIAL] EARLY DIAGNOSTIC OUTPUT ENABLED",
 "[MEMORY] READING BOOT INFORMATION TAGS",
 "[VIDEO] CHECKING FRAMEBUFFER METADATA",
 "[VIDEO] CONSOLE RENDERER INITIALIZED",
 "[BOOT] STARTUP LOG STREAM ACTIVE",
 "[KERNEL] PREPARING COMMAND SHELL",
 "[DIAG] DISPLAY MODE PROVIDED BY BOOTLOADER",
 "[DIAG] KERNEL RUNNING WITHOUT HOSTED LIBRARIES",
 "[CONSOLE] MONOSPACE TEXT OUTPUT READY",
 "[BOOT] KEEPING SHELL HIDDEN DURING STARTUP",
 "[KERNEL] CONTROL REMAINS IN KERNEL CONTEXT",
 "[DIAG] HARDWARE STATUS IS NOT ASSUMED",
 "[BOOT] INITIALIZATION TRACE CONTINUES",
 "[KERNEL] STARTUP TRACE IS A DIAGNOSTIC VIEW",
 "[BOOT] HANDOFF TO INTERACTIVE SHELL PENDING"
 };
 console_set_colors(BOOT_BLACK,BOOT_WHITE); console_clear();
 emit("CUSTOM OS MINIMAL - LIVE STARTUP LOG");
 emit("------------------------------------");
 for(uint32_t pass=0;pass<5;++pass)
  for(uint32_t i=0;i<sizeof(logs)/sizeof(logs[0]);++i){emit(logs[i]);pause_brief();}
 emit("[BOOT] STARTUP TRACE COMPLETE"); pause_brief();
}
