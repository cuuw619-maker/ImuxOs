#include <stdint.h>
struct MultibootTag { uint32_t type; uint32_t size; };
struct FramebufferTag {
  uint32_t type; uint32_t size; uint64_t framebuffer_addr; uint32_t framebuffer_pitch;
  uint32_t framebuffer_width; uint32_t framebuffer_height; uint8_t framebuffer_bpp;
  uint8_t framebuffer_type; uint16_t reserved;
};
static volatile uint32_t* framebuffer=nullptr;
static uint32_t fb_width=0,fb_height=0,fb_pitch=0;
static void put_pixel(uint32_t x,uint32_t y,uint32_t color){if(!framebuffer||x>=fb_width||y>=fb_height)return;framebuffer[(y*fb_pitch/4)+x]=color;}
static void fill(uint32_t color){for(uint32_t y=0;y<fb_height;++y)for(uint32_t x=0;x<fb_width;++x)put_pixel(x,y,color);}
static void rectangle(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t color){for(uint32_t yy=y;yy<y+h&&yy<fb_height;++yy)for(uint32_t xx=x;xx<x+w&&xx<fb_width;++xx)put_pixel(xx,yy,color);}
extern "C" void kernel_main(uint32_t multiboot_info){
  auto* tags=reinterpret_cast<MultibootTag*>(static_cast<uintptr_t>(multiboot_info)+8);
  while(tags->type!=0){
    if(tags->type==8){auto* fb=reinterpret_cast<FramebufferTag*>(tags);framebuffer=reinterpret_cast<volatile uint32_t*>(static_cast<uintptr_t>(fb->framebuffer_addr));fb_width=fb->framebuffer_width;fb_height=fb->framebuffer_height;fb_pitch=fb->framebuffer_pitch;break;}
    tags=reinterpret_cast<MultibootTag*>(reinterpret_cast<uintptr_t>(tags)+((tags->size+7)&~7u));
  }
  if(!framebuffer)for(;;)asm volatile("hlt");
  fill(0x101318); rectangle(fb_width/2-180,fb_height/2-70,360,140,0x202630); rectangle(fb_width/2-120,fb_height/2-8,240,16,0x5B8CFF);
  for(;;)asm volatile("hlt");
}