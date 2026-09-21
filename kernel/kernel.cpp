#include <stdint.h>
struct MultibootTag { uint32_t type; uint32_t size; };
struct FramebufferTag { uint32_t type; uint32_t size; uint64_t framebuffer_addr; uint32_t framebuffer_pitch; uint32_t framebuffer_width; uint32_t framebuffer_height; uint8_t framebuffer_bpp; uint8_t framebuffer_type; uint16_t reserved; };
static volatile uint16_t* const vga=reinterpret_cast<volatile uint16_t*>(0xB8000);
static uint32_t cursor=80;
static volatile uint32_t* framebuffer=nullptr; static uint32_t fb_width=0,fb_height=0,fb_pitch=0;
static void log(const char* s){while(*s&&cursor<2000){vga[cursor++]=static_cast<uint16_t>(0x0F00u|static_cast<uint8_t>(*s++));}cursor++;}
static void put_pixel(uint32_t x,uint32_t y,uint32_t c){if(!framebuffer||x>=fb_width||y>=fb_height)return;framebuffer[(y*fb_pitch/4)+x]=c;}
static void fill(uint32_t c){for(uint32_t y=0;y<fb_height;++y)for(uint32_t x=0;x<fb_width;++x)put_pixel(x,y,c);}
static void rect(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t c){for(uint32_t yy=y;yy<y+h&&yy<fb_height;++yy)for(uint32_t xx=x;xx<x+w&&xx<fb_width;++xx)put_pixel(xx,yy,c);}
extern "C" void kernel_main(uint32_t multiboot_info){
 log("[1] kernel_main entered");
 if(!multiboot_info){log("[X] Multiboot2 info NULL");for(;;)asm volatile("hlt");}
 log("[2] Multiboot2 info received");
 auto* tags=reinterpret_cast<MultibootTag*>(static_cast<uintptr_t>(multiboot_info)+8);
 uint32_t guard=0;
 while(tags->type!=0&&guard++<1024){
  if(tags->type==8&&tags->size>=sizeof(FramebufferTag)){auto* fb=reinterpret_cast<FramebufferTag*>(tags);framebuffer=reinterpret_cast<volatile uint32_t*>(static_cast<uintptr_t>(fb->framebuffer_addr));fb_width=fb->framebuffer_width;fb_height=fb->framebuffer_height;fb_pitch=fb->framebuffer_pitch;break;}
  if(tags->size<8)break;
  tags=reinterpret_cast<MultibootTag*>(reinterpret_cast<uintptr_t>(tags)+((tags->size+7u)&~7u));
 }
 if(!framebuffer||!fb_width||!fb_height||!fb_pitch){log("[X] No framebuffer");for(;;)asm volatile("hlt");}
 log("[3] Framebuffer received");
 fill(0x101318);
 uint32_t cx=fb_width/2,cy=fb_height/2;
 rect(cx-260,cy-100,520,200,0x202630);rect(cx-180,cy-8,360,16,0x5B8CFF);rect(cx-180,cy+24,120,8,0x8EA7FF);
 log("[4] Renderer OK");
 for(;;)asm volatile("hlt");
}
