#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#define _height 80
#define SIZE 64
#define PI 3.14159265
#define ARCS 19
#define _width 80
#define _bitsperpixel 24
#define _planes 1
#define _compression 0
#define _pixelbytesize _height*_width*_bitsperpixel/8
#define _filesize _pixelbytesize+sizeof(bitmap)
#define _xpixelpermeter 0x130B //2835 , 72 DPI
#define _ypixelpermeter 0x130B //2835 , 72 DPI
#define pixel 0x6F
#pragma pack(push,1)

typedef struct{
    uint8_t signature[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t fileoffset_to_pixelarray;
} fileheader;
typedef struct{
    uint32_t dibheadersize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpixel;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t ypixelpermeter;
    uint32_t xpixelpermeter;
    uint32_t numcolorspallette;
    uint32_t mostimpcolor;
} bitmapinfoheader;
typedef struct {
    fileheader fileheader;
    bitmapinfoheader bitmapinfoheader;
} bitmap;
#pragma pack(pop)

void set_pixel(uint8_t *pixelbuffer, int x, int y, uint8_t r, uint8_t g, uint8_t b ) {
	int i=(y*_width+x)*3;
        pixelbuffer[i]=r;
        pixelbuffer[i+1]=g;
        pixelbuffer[i+2]=b;
}

int write_bmp() {
    FILE *fp = fopen("test.bmp","wb");
    bitmap *pbitmap  = (bitmap*)calloc(1,sizeof(bitmap));
    uint8_t *pixelbuffer = (uint8_t*)malloc(_pixelbytesize);
    strcpy(pbitmap->fileheader.signature,"BM");
    pbitmap->fileheader.filesize = _filesize;
    pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
    pbitmap->bitmapinfoheader.dibheadersize =sizeof(bitmapinfoheader);
    pbitmap->bitmapinfoheader.width = _width;
    pbitmap->bitmapinfoheader.height = _height;
    pbitmap->bitmapinfoheader.planes = _planes;
    pbitmap->bitmapinfoheader.bitsperpixel = _bitsperpixel;
    pbitmap->bitmapinfoheader.compression = _compression;
    pbitmap->bitmapinfoheader.imagesize = _pixelbytesize;
    pbitmap->bitmapinfoheader.ypixelpermeter = _ypixelpermeter ;
    pbitmap->bitmapinfoheader.xpixelpermeter = _xpixelpermeter ;
    pbitmap->bitmapinfoheader.numcolorspallette = 0;
    fwrite (pbitmap, 1, sizeof(bitmap),fp);
    memset(pixelbuffer,pixel,_pixelbytesize);
    set_pixel(pixelbuffer, 3,3,255,255,255);
    fwrite(pixelbuffer,1,_pixelbytesize,fp);
    fclose(fp);
    free(pbitmap);
    free(pixelbuffer);
}


struct Arc {
   double internal_radius;  // 0..1 if 1 eq to SIZE
   double external_radius;
   double start_radiant;
   double end_radiant;
   double inc_radiant;
};

void print_matrix(int m[SIZE][SIZE]) {
    for (int i=0;i<SIZE;i++) {
        for (int j=0;j<SIZE;j++) {
             if (m[i][j])
                 printf("█");
             else  
                 printf(" ");
        }
        printf("\n");
    } 
}

void draw_circle(int m[SIZE][SIZE], struct Arc *c[ARCS]) {
    int half = SIZE / 2;
    int d;
    double theta;
    for (int i=-half;i<half;i++) {
        for (int j=-half;j<half;j++) {
            d = (int) sqrt( (double) i*i+j*j );
            theta = atan2((double)i, (double)j);
            for (int k=0;k<ARCS*2;k++) {
                int a = (int) (c[k]->internal_radius * SIZE);
                int b = (int) (c[k]->external_radius * SIZE);
        
                //printf("k:%d i:%d j:%d d:%d min:%f theta:%f max:%f\n",k,i,j,d,c[k]->start_radiant,theta,c[k]->end_radiant);
                if ( a<d && b>d && 
                      ( (c[k]->start_radiant < theta && c[k]->end_radiant > theta) ||
                        (c[k]->start_radiant < (theta - 2 * PI) && c[k]->end_radiant > (theta - 2 * PI) ) ||
                        (c[k]->start_radiant < (theta + 2 * PI) && c[k]->end_radiant > (theta + 2 * PI) ) )
                   )  {
                    m[i+half][j+half]=1;
                }
            }
        } 
    }
}

int reset(int m[SIZE][SIZE]) {
    for (int i=0;i<SIZE;i++) {
        for (int j=0;j<SIZE;j++) {
             m[i][j]=0;  
        }
    } 
} 

int inc_rad(struct Arc *c) {
    c->start_radiant+=c->inc_radiant;
    c->end_radiant+=c->inc_radiant;
    if (c->end_radiant > PI && c->start_radiant > PI) {
        c->end_radiant-= 2*PI;
        c->start_radiant-= 2*PI;
    }
    if (c->end_radiant < 0 && c->start_radiant < 0) {
        c->end_radiant+= 2*PI;
        c->start_radiant+= 2*PI;
    }
}

void print_arc(int i, struct Arc *a) {
    
    printf("%d start:%.2f end:%.2f min:%.3f max:%.3f\n",i, a->internal_radius, a->external_radius, a->start_radiant, a->end_radiant);
}

int main()
{
    int m[SIZE][SIZE];
    struct Arc *c[ARCS*2];
    double carc = 2 * PI / ARCS;
    double space = carc / 6;
    printf("carc:%f space:%f\n",carc, space);
    for (int i=0;i<ARCS;i++) {
        struct Arc *t = (struct Arc *) malloc(sizeof(struct Arc));
        t->internal_radius=0.45;
        t->external_radius=0.50;
        t->start_radiant=i * carc;
        t->end_radiant=(i+1) * carc - space;
	t->inc_radiant=0.05;
        c[i]=t;
    }
    for (int i=0;i<ARCS;i++) {
        struct Arc *t = (struct Arc *) malloc(sizeof(struct Arc));
        t->internal_radius=0.30;
        t->external_radius=0.37;
        t->start_radiant=i * carc;
        t->end_radiant=(i+1) * carc - space;
	t->inc_radiant=-0.05;
        c[i+ARCS]=t;
    }
    c[0]->start_radiant=-0.2;
    c[ARCS]->start_radiant=-0.2;
    c[1]->internal_radius-=0.07;
    c[1]->external_radius-=0.05;

    write_bmp();
    while(1) {
        reset(m);
        for (int i=0;i<ARCS*2;i++) {
            inc_rad(c[i]);
        }
        //inc_rad(c[1],-0.05);
        draw_circle(m,c);
        //printf("\e[1;1H\e[2J");
        print_matrix(m);
        usleep(100000);
   }

    return 0;
}

