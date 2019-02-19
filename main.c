#include <stdio.h>
#include <math.h>
#define SIZE 64
#define PI 3.14159265
#define ARCS 19

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
            for (int k=0;k<ARCS;k++) {
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
    struct Arc *c[ARCS];
    double carc = 2 * PI / ARCS;
    double space = carc / 6;
    printf("carc:%f space:%f\n",carc, space);
    for (int i=0;i<ARCS;i++) {
        struct Arc *t = (struct Arc *) malloc(sizeof(struct Arc));
        t->internal_radius=0.39;
        t->external_radius=0.48;
        t->start_radiant=i * carc;
        t->end_radiant=(i+1) * carc - space;
	t->inc_radiant=0.05;
        c[i]=t;
    }

    while(1) {
        reset(m);
        for (int i=0;i<ARCS;i++) {
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

