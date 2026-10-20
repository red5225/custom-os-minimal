#include <stddef.h>
#include <stdint.h>

#define VGA ((volatile uint16_t *)0xB8000)
#define VGA_W 80
#define VGA_H 25
#define COLOR 0x0F
#define MB2_MAGIC 0x36d76289u
#define MB2_TAG_END 0
#define MB2_TAG_FRAMEBUFFER 8

static size_t row, col;
static uint8_t color = COLOR;
static uint32_t *fb;
static uint64_t fb_addr;
static uint32_t fb_pitch, fb_width, fb_height, fb_bpp;
static uint8_t fb_type;

static inline void outb(uint16_t p, uint8_t v) { __asm__ volatile ("outb %0, %1" : : "a"(v), "Nd"(p)); }
static inline uint8_t inb(uint16_t p) { uint8_t v; __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(p)); return v; }
static void serial_init(void) { outb(0x3F9,0); outb(0x3FB,0x80); outb(0x3F8,3); outb(0x3F9,0); outb(0x3FB,3); outb(0x3FA,0xC7); outb(0x3FC,0x0B); }
static void serial_char(char c) { for(unsigned i=0;i<1000000;i++) if(inb(0x3FD)&0x20) break; outb(0x3F8,(uint8_t)c); }
static void serial_write(const char *s) { while(*s) serial_char(*s++); }

/* Compact 8x8 font for the characters used by the boot banner and prompt. */
static const uint8_t font[96][8] = {
[' ' - 32]={0,0,0,0,0,0,0,0}, ['!' - 32]={24,24,24,24,24,0,24,0},
['-' - 32]={0,0,0,126,0,0,0,0}, ['.' - 32]={0,0,0,0,0,24,24,0},
[':' - 32]={0,24,24,0,0,24,24,0}, ['>' - 32]={64,32,16,8,16,32,64,0},
['A' - 32]={24,36,66,66,126,66,66,0}, ['B' - 32]={124,66,66,124,66,66,124,0},
['C' - 32]={60,66,64,64,64,66,60,0}, ['D' - 32]={120,68,66,66,66,68,120,0},
['E' - 32]={126,64,64,120,64,64,126,0}, ['F' - 32]={126,64,64,120,64,64,64,0},
['G' - 32]={60,66,64,78,66,66,60,0}, ['H' - 32]={66,66,66,126,66,66,66,0},
['I' - 32]={60,24,24,24,24,24,60,0}, ['K' - 32]={66,68,72,112,72,68,66,0},
['L' - 32]={64,64,64,64,64,64,126,0}, ['M' - 32]={66,102,90,90,66,66,66,0},
['N' - 32]={66,98,82,74,70,66,66,0}, ['O' - 32]={60,66,66,66,66,66,60,0},
['P' - 32]={124,66,66,124,64,64,64,0}, ['R' - 32]={124,66,66,124,72,68,66,0},
['S' - 32]={60,66,64,60,2,66,60,0}, ['T' - 32]={126,24,24,24,24,24,24,0},
['U' - 32]={66,66,66,66,66,66,60,0}, ['V' - 32]={66,66,66,66,66,36,24,0},
['a' - 32]={0,0,60,2,62,66,62,0}, ['b' - 32]={64,64,92,98,66,98,92,0},
['c' - 32]={0,0,60,66,64,66,60,0}, ['d' - 32]={2,2,58,70,66,70,58,0},
['e' - 32]={0,0,60,66,126,64,60,0}, ['f' - 32]={12,18,16,56,16,16,16,0},
['g' - 32]={0,0,58,70,66,62,2,60}, ['h' - 32]={64,64,92,98,66,66,66,0},
['i' - 32]={24,0,56,24,24,24,60,0}, ['k' - 32]={64,64,68,72,112,72,68,0},
['l' - 32]={56,24,24,24,24,24,60,0}, ['m' - 32]={0,0,108,82,82,82,82,0},
['n' - 32]={0,0,92,98,66,66,66,0}, ['o' - 32]={0,0,60,66,66,66,60,0},
['p' - 32]={0,0,92,98,98,92,64,64}, ['r' - 32]={0,0,92,98,64,64,64,0},
['s' - 32]={0,0,62,64,60,2,124,0}, ['t' - 32]={16,16,56,16,16,18,12,0},
['u' - 32]={0,0,66,66,66,70,58,0}, ['v' - 32]={0,0,66,66,66,36,24,0},
['x' - 32]={0,0,66,36,24,36,66,0}, ['y' - 32]={0,0,66,66,70,58,2,60},
['0' - 32]={60,66,70,74,82,98,60,0}, ['1' - 32]={24,56,24,24,24,24,60,0},
['2' - 32]={60,66,2,12,48,64,126,0}, ['3' - 32]={124,2,2,28,2,2,124,0},
['4' - 32]={8,24,40,72,126,8,8,0}, ['5' - 32]={126,64,64,124,2,66,60,0},
['6' - 32]={28,32,64,124,66,66,60,0}, ['7' - 32]={126,2,4,8,16,16,16,0},
['8' - 32]={60,66,66,60,66,66,60,0}, ['9' - 32]={60,66,66,62,2,4,56,0}
};
static void pixel(uint32_t x,uint32_t y,uint32_t c){ if(fb && x<fb_width && y<fb_height) *(volatile uint32_t*)((uintptr_t)fb + (uint64_t)y*fb_pitch + x*4)=c; }
static void glyph(char ch,uint32_t x,uint32_t y,uint32_t fg,uint32_t bg){ if(ch<32||ch>127) ch='?'; for(int yy=0;yy<8;yy++){uint8_t bits=font[(unsigned char)ch-32][yy];for(int xx=0;xx<8;xx++)pixel(x+xx,y+yy,(bits&(0x80>>xx))?fg:bg);} }
static void clear_screen(void){ if(fb){for(uint32_t y=0;y<fb_height;y++)for(uint32_t x=0;x<fb_width;x++)pixel(x,y,0x00101828);} else {for(size_t i=0;i<VGA_W*VGA_H;i++)VGA[i]=((uint16_t)color<<8)|' ';} row=col=0; }
static void putc_term(char c){ if(c=='\r'){col=0;return;} if(c=='\n'){col=0;row++;} else { if(fb){glyph(c,(uint32_t)col*8,(uint32_t)row*16,0x00FFFFFF,0x00101828);glyph(c,(uint32_t)col*8,(uint32_t)row*16+8,0x00FFFFFF,0x00101828);} else VGA[row*VGA_W+col]=((uint16_t)color<<8)|(uint8_t)c; col++; if(col>= (fb?fb_width/8:VGA_W)){col=0;row++;}} if(row >= (fb?fb_height/16:VGA_H)){clear_screen();} }
static void write(const char*s){while(*s)putc_term(*s++);}
static void prompt(void){write("\nxc> ");}
static void execute(char*cmd){if(!cmd[0])return;if(cmd[0]=='h'&&cmd[1]=='e'&&cmd[2]=='l'&&cmd[3]=='p'&&!cmd[4])write("\nCommands: help, about, clear, reboot\n");else if(cmd[0]=='a'&&cmd[1]=='b'&&cmd[2]=='o'&&cmd[3]=='u'&&cmd[4]=='t'&&!cmd[5])write("\nCustom OS Minimal: x86-64 kernel and text terminal.\n");else if(cmd[0]=='c'&&cmd[1]=='l'&&cmd[2]=='e'&&cmd[3]=='a'&&cmd[4]=='r'&&!cmd[5])clear_screen();else if(cmd[0]=='r'&&cmd[1]=='e'&&cmd[2]=='b'&&cmd[3]=='o'&&cmd[4]=='o'&&cmd[5]=='t'&&!cmd[6])write("\nUse the firmware reset shortcut to reboot.\n");else write("\nUnknown command. Type help.\n");}
static char keymap[128]={[0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',[0x07]='6',[0x08]='7',[0x09]='8',[0x0A]='9',[0x0B]='0',[0x1E]='a',[0x30]='b',[0x2E]='c',[0x20]='d',[0x12]='e',[0x21]='f',[0x22]='g',[0x23]='h',[0x17]='i',[0x24]='j',[0x25]='k',[0x26]='l',[0x32]='m',[0x31]='n',[0x18]='o',[0x19]='p',[0x10]='q',[0x13]='r',[0x1F]='s',[0x14]='t',[0x16]='u',[0x2F]='v',[0x11]='w',[0x2D]='x',[0x15]='y',[0x2C]='z',[0x39]=' ',[0x1C]='\n',[0x0E]='\b',[0x33]=',',[0x34]='.'};
void kernel_main(uint32_t magic, uint32_t info_addr){
 serial_init();
 if(magic==MB2_MAGIC && info_addr){
  uint8_t *base=(uint8_t*)(uintptr_t)info_addr; uint32_t total=*(uint32_t*)base;
  for(uint32_t off=8;off+8<=total;){uint32_t type=*(uint32_t*)(base+off),size=*(uint32_t*)(base+off+4);if(size<8||off+size>total)break;
   if(type==MB2_TAG_FRAMEBUFFER && size>=32){uint64_t addr=*(uint64_t*)(base+off+8);uint32_t pitch=*(uint32_t*)(base+off+16),w=*(uint32_t*)(base+off+20),h=*(uint32_t*)(base+off+24);uint8_t bpp=base[off+28],kind=base[off+29];if(bpp==32&&kind==1&&w&&h&&pitch>=w*4){fb_addr=addr;fb=(uint32_t*)(uintptr_t)addr;fb_pitch=pitch;fb_width=w;fb_height=h;fb_bpp=bpp;fb_type=kind;}}
   if(type==MB2_TAG_END)break;off=(off+size+7)&~7u;
  }
 }
 clear_screen();write("CUSTOM OS MINIMAL\n64-bit kernel | framebuffer text terminal\nType help for commands.\n");serial_write("KERNEL_READY\nCUSTOM OS MINIMAL\n");prompt();char command[80];size_t len=0;
 for(;;){if(!(inb(0x64)&1)){__asm__ volatile("pause");continue;}uint8_t scan=inb(0x60);if(scan&0x80)continue;if(scan==0x1C){command[len]=0;execute(command);len=0;prompt();}else if(scan==0x0E){if(len){--len;putc_term('\b');}}else if(scan<128&&keymap[scan]&&keymap[scan]!='\t'&&len<sizeof(command)-1){command[len++]=keymap[scan];putc_term(keymap[scan]);}}
}
