#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define DEAD_MIL    0
#define BROKEN_MIL  1
#define UNTOUCH_MIL 2
#define DEAD_CIV    3
#define BROKEN_CIV  4
#define UNTOUCH_CIV 5


int* radToArea(int *bomb){
    int * res = (int *) malloc( sizeof(int) * 5 );

    res[0] = bomb[0]-bomb[2];
    res[1] = bomb[2]-bomb[2];
    res[2] = bomb[0]+bomb[2];
    res[3] = bomb[2]+bomb[2];
    res[4] = bomb[3];

    return res;
}

int isInBombingArea(int area[],int target[]){
    return (area[0] <= target[0]  && target[0] <= area[2]) && 
           (area[1] <= target[1]  && target[1] <= area[3]);
}

int * process(int **targets, int **attacks, int n_targets, int n_bombs){

    int i, j,touched = 1,isCivil,alive;
    static int res[6];

    for (i = 0; i < n_targets; ++i) {
        touched = 0;
        isCivil = targets[i][2] > 0;
        alive   = 1;

        for (j = 0; j < n_bombs; ++j) {
            printf("Ciclo \n");
            if (isInBombingArea(attacks[j],targets[i])) {
                touched = 1;
                if (isCivil) {
                    targets[i][2] -= attacks[j][4];
                    alive = targets[i][2] > 0;
                }
                else {
                    targets[i][2] += attacks[j][4];
                    alive = targets[i][2] < 0;
                }
            }

            if (! alive) break;
        }

        if (! touched)
            if (isCivil) res[UNTOUCH_CIV] += 1;
            else         res[UNTOUCH_MIL] += 1;
        else
            if (isCivil) 
                if (alive) res[BROKEN_CIV] += 1;
                else       res[DEAD_CIV]   += 1;
            else
                if (alive) res[BROKEN_MIL] += 1;
                else       res[DEAD_MIL]   += 1;
    }

    return res;

}

int main(int argc, char const *argv[])
{
    int i,j;
    int n_targets;
    int n_bombs;
    int map_size; // unused
    int **bombs  ;
    int **b_areas;
    int **targets;
    int *res;

    struct timeval t0,tf,t;

    /* Start reading data */
    scanf("%d",&map_size);

    /* Reading targets */
    scanf("%d",&n_targets);
    targets = (int **) malloc(sizeof(int **) * n_targets);
    for (i = 0; i < n_targets; ++i)
    {   
        targets[i] = (int *) malloc(sizeof(int) * 3);
        scanf("%d %d %d\n",&targets[i],&targets[i][1],&targets[i][2]);
    }

    /* Reading bombs */
    scanf("%d",&n_bombs);
    bombs   = (int **) malloc(sizeof(int **) * n_bombs);
    b_areas = (int **) malloc(sizeof(int **) * n_bombs);
    for (i = 0; i < n_bombs; ++i)
    {
        bombs[i] = (int *) malloc(sizeof(int) * 3);
        scanf("%d %d %d",bombs[i],&bombs[i][1],&bombs[i][2]);
    }

    gettimeofday(&t0,NULL);

    // Bomb square areas to limit coordinates
    for (i = 0; i < n_bombs; ++i)
        b_areas[i] = radToArea(bombs[i]);

    // printing new bomb areas
    for (i = 0; i < n_bombs; ++i)
        for (j = 0; j < 5; ++j) {
            printf("%3d", b_areas[i][j]);
            if (j == 4) printf("\n");
        }

    printf("\n\n\n\n");

    res = process(targets,b_areas,n_targets,n_bombs);
    printf("Pretiempo\n");

    printf("%p \n",res);

    gettimeofday(&tf,NULL);
    timersub(&tf,&t0,&t);
    printf("tiempo %ld:%ld \n",t.tv_sec,t.tv_usec);

    free(res);
    for (i = 0; i < n_bombs; ++i)
        free(b_areas[i]);



    return 0;
}