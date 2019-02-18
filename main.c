#include <stdio.h>
#include <math.h>
#define SIZE 64
#define PI 3.14159265
#define ARCS 19

struct Arc {
   double start_radius;  // 0..1 if 1 eq to SIZE
   double end_radius;
   double min_radiant;
   double max_radiant;
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
                int a = (int) (c[k]->start_radius * SIZE);
                int b = (int) (c[k]->end_radius * SIZE);
        
                //printf("k:%d i:%d j:%d d:%d min:%f theta:%f max:%f\n",k,i,j,d,c[k]->min_radiant,theta,c[k]->max_radiant);
                if ( a<d && b>d && 
                      ( (c[k]->min_radiant < theta && c[k]->max_radiant > theta) ||
                        (c[k]->min_radiant < (theta - 2 * PI) && c[k]->max_radiant > (theta - 2 * PI) ) ||
                        (c[k]->min_radiant < (theta + 2 * PI) && c[k]->max_radiant > (theta + 2 * PI) ) )
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

int inc_rad(struct Arc *c, double inc) {
    
    c->min_radiant+=inc;
    c->max_radiant+=inc;
    if (c->max_radiant > PI && c->min_radiant > PI) {
        c->max_radiant-= 2*PI;
        c->min_radiant-= 2*PI;
    }
    if (c->max_radiant < -PI && c->min_radiant < -PI) {
        c->max_radiant+= 2*PI;
        c->min_radiant+= 2*PI;
    }
}

void print_arc(int i, struct Arc *a) {
    
    printf("%d start:%.2f end:%.2f min:%.3f max:%.3f\n",i, a->start_radius, a->end_radius, a->min_radiant, a->max_radiant);
}

int main()
{
    int m[SIZE][SIZE];
    /*struct Arc c2;
    c2.start_radius=0.35;
    c2.end_radius=0.45;
    c2.min_radiant=0.0;
    c2.max_radiant=PI/4;
    
    struct Arc c1;
    c1.start_radius=0.15;
    c1.end_radius=0.25;
    c1.min_radiant=0.0;
    c1.max_radiant=PI/4;
    */
    //struct Arc *c[ARCS] = {&c1,&c2};
    struct Arc *c[ARCS];
    double carc = 2 * PI / ARCS;
    double space = carc / 6;
    printf("carc:%f space:%f\n",carc, space);
    for (int i=0;i<ARCS;i++) {
        struct Arc *t = (struct Arc *) malloc(sizeof(struct Arc));
        t->start_radius=0.39;
        t->end_radius=0.48;
        t->min_radiant=i * carc;
        t->max_radiant=(i+1) * carc - space;
        c[i]=t;
    }

    while(1) {
        reset(m);
        for (int i=0;i<ARCS;i++) {
            inc_rad(c[i],0.05);
        }
        //inc_rad(c[1],-0.05);
        draw_circle(m,c);
        //printf("\e[1;1H\e[2J");
        print_matrix(m);
        usleep(100000);
   }

    return 0;
}

