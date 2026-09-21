#include <stdint.h>

struct MultibootTag { uint32_t type; uint32_t size; };
struct FramebufferTag {
    uint32_t type; uint32_t size; uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch, framebuffer_width, framebuffer_height;
    uint8_t framebuffer_bpp, framebuffer_type; uint16_t reserved;
};

static volatile uint32_t* fb=nullptr;
static uint32_t W=0,H=0,P=0;
static uint32_t cursor=0;

static void pixel(uint32_t x,uint32_t y,uint32_t c){
    if(!fb||x>=W||y>=H)return;
    fb[(y*P/4)+x]=c;
}
static void box(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t c){
    for(uint32_t yy=y;yy<y+h&&yy<H;++yy)
        for(uint32_t xx=x;xx<x+w&&xx<W;++xx) pixel(xx,yy,c);
}
static void clear(uint32_t c){ box(0,0,W,H,c); }

static uint8_t glyph(char c,uint32_t row){
    static const uint8_t digits[10][7]={
        {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
        {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
        {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
        {14,17,17,15,1,1,14}
    };
    if(c>='0'&&c<='9') return digits[c-'0'][row];
    switch(c){
        case 'A':{static const uint8_t a[7]={14,17,17,31,17,17,17};return a[row];}
        case 'B':{static const uint8_t a[7]={30,17,17,30,17,17,30};return a[row];}
        case 'C':{static const uint8_t a[7]={14,17,16,16,16,17,14};return a[row];}
        case 'D':{static const uint8_t a[7]={30,17,17,17,17,17,30};return a[row];}
        case 'E':{static const uint8_t a[7]={31,16,16,30,16,16,31};return a[row];}
        case 'F':{static const uint8_t a[7]={31,16,16,30,16,16,16};return a[row];}
        case 'G':{static const uint8_t a[7]={14,17,16,23,17,17,15};return a[row];}
        case 'H':{static const uint8_t a[7]={17,17,17,31,17,17,17};return a[row];}
        case 'I':{static const uint8_t a[7]={31,4,4,4,4,4,31};return a[row];}
        case 'K':{static const uint8_t a[7]={17,18,20,24,20,18,17};return a[row];}
        case 'L':{static const uint8_t a[7]={16,16,16,16,16,16,31};return a[row];}
        case 'M':{static const uint8_t a[7]={17,27,21,21,17,17,17};return a[row];}
        case 'N':{static const uint8_t a[7]={17,25,21,19,17,17,17};return a[row];}
        case 'O':{static const uint8_t a[7]={14,17,17,17,17,17,14};return a[row];}
        case 'P':{static const uint8_t a[7]={30,17,17,30,16,16,16};return a[row];}
        case 'R':{static const uint8_t a[7]={30,17,17,30,20,18,17};return a[row];}
        case 'S':{static const uint8_t a[7]={15,16,16,14,1,1,30};return a[row];}
        case 'T':{static const uint8_t a[7]={31,4,4,4,4,4,4};return a[row];}
        case 'U':{static const uint8_t a[7]={17,17,17,17,17,17,14};return a[row];}
        case 'X':{static const uint8_t a[7]={17,17,10,4,10,17,17};return a[row];}
        case 'Y':{static const uint8_t a[7]={17,17,10,4,4,4,4};return a[row];}
        case 'Z':{static const uint8_t a[7]={31,1,2,4,8,16,31};return a[row];}
        case ':':{static const uint8_t a[7]={0,4,4,0,4,4,0};return a[row];}
        case '[':{static const uint8_t a[7]={14,8,8,8,8,8,14};return a[row];}
        case ']':{static const uint8_t a[7]={14,2,2,2,2,2,14};return a[row];}
        case '-':{static const uint8_t a[7]={0,0,0,31,0,0,0};return a[row];}
        case '.':{static const uint8_t a[7]={0,0,0,0,0,6,6};return a[row];}
        case '/':{static const uint8_t a[7]={1,2,2,4,8,8,16};return a[row];}
        case '(': {static const uint8_t a[7]={2,4,8,8,8,4,2};return a[row];}
        case ')': {static const uint8_t a[7]={8,4,2,2,2,4,8};return a[row];}
        case ' ': return 0;
        default: return 0;
    }
}
static void text(uint32_t x,uint32_t y,const char* s,uint32_t scale,uint32_t color){
    while(*s){
        char c=*s++;
        for(uint32_t r=0;r<7;++r){
            uint8_t bits=glyph(c,r);
            for(uint32_t col=0;col<5;++col)
                if(bits&(1u<<(4-col))) box(x+col*scale,y+r*scale,scale,scale,color);
        }
        x+=6*scale;
    }
}
static void status(uint32_t y,bool ok,const char* name){
    box(80,y+2,16,16,ok?0x35D07F:0xE05252);
    text(83,y+5,ok?"":"X",2,0xFFFFFF);
    text(115,y,"[",2,0xAAB2C0);
    text(127,y,ok?"OK":"FAIL",2,ok?0x35D07F:0xE05252);
    text(175,y,"]",2,0xAAB2C0);
    text(205,y,name,2,0xE8ECF2);
}
extern "C" void kernel_main(uint32_t info){
    if(!info) for(;;) asm volatile("hlt");
    auto* tags=reinterpret_cast<MultibootTag*>(static_cast<uintptr_t>(info)+8);
    uint32_t guard=0;
    while(tags->type!=0&&guard++<1024){
        if(tags->type==8&&tags->size>=sizeof(FramebufferTag)){
            auto* f=reinterpret_cast<FramebufferTag*>(tags);
            if(f->framebuffer_bpp==32&&f->framebuffer_type==1){
                fb=reinterpret_cast<volatile uint32_t*>(static_cast<uintptr_t>(f->framebuffer_addr));
                W=f->framebuffer_width; H=f->framebuffer_height; P=f->framebuffer_pitch;
            }
            break;
        }
        if(tags->size<8) break;
        tags=reinterpret_cast<MultibootTag*>(reinterpret_cast<uintptr_t>(tags)+((tags->size+7u)&~7u));
    }
    if(!fb||!W||!H||!P) for(;;) asm volatile("hlt");

    clear(0x0D1017);
    box(0,0,W,74,0x171D27);
    box(48,22,8,30,0x5B8CFF);
    text(76,22,"IMUXOS BOOT",4,0xFFFFFF);
    text(80,92,"KERNEL STARTUP DIAGNOSTICS",2,0x8EA7FF);

    status(140,true,"CPU / KERNEL ENTRY");
    status(182,true,"STACK INITIALIZATION");
    status(224,true,"MULTIBOOT2 INFORMATION");
    status(266,true,"FRAMEBUFFER");
    status(308,true,"RENDERER");

    box(64,360,W-128,2,0x303847);
    text(80,390,"STATUS: KERNEL RUNNING",3,0x35D07F);

    text(80,445,"ARCH: X86_64",2,0xAAB2C0);
    text(80,475,"FRAMEBUFFER: ",2,0xAAB2C0);
    char dims[32]; dims[0]=char('0'+((W/1000)%10)); dims[1]=char('0'+((W/100)%10)); dims[2]=char('0'+((W/10)%10)); dims[3]=char('0'+(W%10)); dims[4]='X';
    dims[5]=char('0'+((H/1000)%10)); dims[6]=char('0'+((H/100)%10)); dims[7]=char('0'+((H/10)%10)); dims[8]=char('0'+(H%10)); dims[9]=0;
    text(240,475,dims,2,0xE8ECF2);
    text(80,505,"KERNEL: RUNNING",2,0xAAB2C0);

    for(;;) asm volatile("hlt");
}
