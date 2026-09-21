#include <stdint.h>
struct MultibootTag { uint32_t type; uint32_t size; };
struct FramebufferTag { uint32_t type; uint32_t size; uint64_t framebuffer_addr; uint32_t framebuffer_pitch; uint32_t framebuffer_width; uint32_t framebuffer_height; uint8_t framebuffer_bpp; uint8_t framebuffer_type; uint16_t reserved; };
static volatile uint32_t* framebuffer=nullptr; static uint32_t fb_width=0,fb_height=0,fb_pitch=0;
static void put_pixel(uint32_t x,uint32_t y,uint32_t color){if(!framebuffer||x>=fb_width||y>=fb_height)return;framebuffer[(y*fb_pitch/4)+x]=color;}
static void fill(uint32_t color){if(!framebuffer)return;for(uint32_t y=0;y<fb_height;++y)for(uint32_t x=0;x<fb_width;++x)put_pixel(x,y,color);}
static void rectangle(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t color){if(!framebuffer)return;for(uint32_t yy=y;yy<y+h&&yy<fb_height;++yy)for(uint32_t xx=x;xx<x+w&&xx<fb_width;++xx)put_pixel(xx,yy,color);}
static void vga_text(const char* text){volatile uint16_t* vga=reinterpret_cast<volatile uint16_t*>(0xB8000);for(uint32_t i=0;text[i]&&i<2000;++i)vga[i]=static_cast<uint16_t>(0x0F00u|static_cast<uint8_t>(text[i]));}
extern "C" void kernel_main(uint32_t multiboot_info){
 if(!multiboot_info){vga_text("ImuxOS: Multiboot2 info pointer is null.");for(;;)asm volatile("hlt");}
 auto* tags=reinterpret_cast<MultibootTag*>(static_cast<uintptr_t>(multiboot_info)+8);
 while(tags->type!=0){
  if(tags->type==8&&tags->size>=sizeof(FramebufferTag)){auto* fb=reinterpret_cast<FramebufferTag*>(tags);framebuffer=reinterpret_cast<volatile uint32_t*>(static_cast<uintptr_t>(fb->framebuffer_addr));fb_width=fb->framebuffer_width;fb_height=fb->framebuffer_height;fb_pitch=fb->framebuffer_pitch;break;}
  tags=reinterpret_cast<MultibootTag*>(reinterpret_cast<uintptr_t>(tags)+((tags->size+7u)&~7u));
 }
 if(!framebuffer||!fb_width||!fb_height||!fb_pitch){vga_text("ImuxOS kernel started, but no framebuffer was provided.");for(;;)asm volatile("hlt");}
 fill(0x101318);uint32_t cx=fb_width/2,cy=fb_height/2;rectangle(cx-260,cy-100,520,200,0x202630);rectangle(cx-180,cy-8,360,16,0x5B8CFF);rectangle(cx-180,cy+24,120,8,0x8EA7FF);for(;;)asm volatile("hlt");
}
