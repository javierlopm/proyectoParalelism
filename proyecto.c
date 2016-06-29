#include "stdio.h"
#include "stdlib.h"
#include <mpi.h>

#define NUM_TARGETS 4
#define NUM_BOMBS   5
#define AREA_SIZE   100 // Meh

#define DEAD_MIL    0
#define BROKEN_MIL  1
#define UNTOUCH_MIL 2
#define DEAD_CIV    3
#define BROKEN_CIV  4
#define UNTOUCH_CIV 5

#define TARGET_SIZE 3

//  Basic test case
int targets[NUM_TARGETS][3] = {
    {0,0,8},
    {5,5,100},
    {1,1,-2},
    {7,7,-6}
};

int bombs[NUM_BOMBS][4] = {
    {2,1,2,3},
    {1,1,1,4},
    {7,7,0,3},
    {6,6,4,8},
    {9,9,8,1}
};

/* 
	radToArea transforma el formato de los bombardeos a 
	otro más eficiente de manejar
*/
int* radToArea(int bomb[]){
    int * res = (int *) malloc( sizeof(int) * 5 );

    res[0] = bomb[0]-bomb[2];
    res[1] = bomb[2]-bomb[2];
    res[2] = bomb[0]+bomb[2];
    res[3] = bomb[2]+bomb[2];
    res[4] = bomb[3];

    return res;
}

/*
	isInBombingArea dado un target y un área bombardeada
	verifica que el target haya sido afectado por la bomba
*/
int isInBombingArea(int area[],int target[]){
    return (area[0] <= target[0]  && target[0] <= area[2]) && 
           (area[1] <= target[1]  && target[1] <= area[3]);
}

/*
	process función que dada una lista de targets y una lista de attacks
	calcula los daños ocasionados por el bombardeo en dicha lista de objetivos
*/
int * process(int **targets, int **attacks)
{
    int i, j,touched = 1,isCivil,alive;
    static int res[6];

    for (i = 0; i < NUM_TARGETS; ++i) {
        touched = 0;
        isCivil = targets[i][2] > 0;
        alive   = 1;

        for (j = 0; j < NUM_BOMBS; ++j) {
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
    int *b_areas[NUM_BOMBS];
    int *res;
    int world_rank;
    int world_size;
    int *res_arr;
    int res_aux[6];



    
    // Inicializar
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Datatype target_type;
    MPI_Type_contiguous(TARGET_SIZE,MPI_INT,&target_type);
    MPI_Type_commit(&target_type);

    if (world_rank == 0)
    {
        // Read

        // Bomb square areas to limit coordinates
        for (i = 0; i < NUM_BOMBS; ++i)
            b_areas[i] = radToArea(bombs[i]);

        // printing new bomb areas
        for (i = 0; i < NUM_BOMBS; ++i)
            for (j = 0; j < 5; ++j) {
                printf("%3d", b_areas[i][j]);
                if (j == 4) printf("\n");
            }

        printf("===========================\n");
    }


    int num_elements_per_proc = 1 ; // ntargets / world_size

    int ** parallel_targets = (int **) malloc(sizeof(int) 
                                              * num_elements_per_proc 
                                              * TARGET_SIZE);

    for (i = 0; i < 6; ++i)
        res_aux[i] = 0;

    MPI_Scatter(targets          ,  num_elements_per_proc  , target_type, 
                parallel_targets ,  num_elements_per_proc  , target_type, 
                0, MPI_COMM_WORLD);



    /* Process each target and accumulate */
    for (i = 0; i < num_elements_per_proc; ++i)
    {
        res = process(parallel_targets,b_areas);
        for (j = 0; i < 6; ++j)
            res_aux[i] += res[i];
    }
    free(res);


    if (world_rank == 0) {
        res_arr = (int *) malloc(sizeof(int) * 4 * world_size );
    }

    MPI_Gather( res_aux, 6, MPI_INT, 
                res_arr, 6, MPI_INT, 
                0, MPI_COMM_WORLD);

    if (world_rank == 0) 
        for (i = 0; i < 6 * num_elements_per_proc; ++i)
            printf("%d \n",res_aux[i]);

    for (i = 0; i < NUM_BOMBS; ++i)
        free(b_areas[i]);

    MPI_Type_free(&target_type);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}