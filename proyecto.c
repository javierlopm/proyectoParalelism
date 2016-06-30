#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define NUM_TARGETS 4
#define NUM_BOMBS   5
#define AREA_SIZE   100 

#define DEAD_MIL    0
#define BROKEN_MIL  1
#define UNTOUCH_MIL 2
#define DEAD_CIV    3
#define BROKEN_CIV  4
#define UNTOUCH_CIV 5

#define TARGET_SIZE   3
#define RAW_BOMB_SIZE 4
#define BOMB_SIZE     5

//  Basic test case
int targets[12] = {
    0,0,  8,
    5,5,100,
    1,1, -2,
    7,7, -6
};

int bombs[20] = {
    2,1,2,3,
    1,1,1,4,
    7,7,0,3,
    6,6,4,8,
    9,9,8,1
};

/* 
	radToArea transforma el formato de los bombardeos a 
	otro más eficiente de manejar
*/
void radToArea(int bomb[],int res[]){
    res[0] = bomb[0]-bomb[2];
    res[1] = bomb[2]-bomb[2];
    res[2] = bomb[0]+bomb[2];
    res[3] = bomb[2]+bomb[2];
    res[4] = bomb[3];
}

/*
	isInBombingArea dado un target y un área bombardeada
	verifica que el target haya sido afectado por la bomba
*/
int isInBombingArea(int *area,int x_pos, int y_pos){
    return (area[0] <= x_pos  && x_pos <= area[2]) && 
           (area[1] <= y_pos  && y_pos <= area[3]);
}

/*
	process función que dada una lista de targets y una lista de attacks
	calcula los daños ocasionados por el bombardeo en dicha lista de objetivos
*/
int * process(int *targets, int *attacks, int num_elements_per_proc)
{


    int i, j,touched = 1,isCivil,alive;
    static int res[6];
    int *act_bomb;


    for (i = 0; i < num_elements_per_proc; ++i) {
        touched = 0;
        isCivil = targets[3*i+2] > 0;
        alive   = 1;

        for (j = 0; j < NUM_BOMBS; j++) {
            act_bomb = attacks + j*BOMB_SIZE;
            if (isInBombingArea(act_bomb,targets[3*i],targets[3*i+1])) {
                touched = 1;
                if (isCivil) {
                    targets[3*i+2] -= act_bomb[4];
                    alive = targets[3*i+2] > 0;
                }
                else {
                    targets[3*i+2] += act_bomb[4];
                    alive = targets[3*i+2] < 0;
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
    int *b_areas;
    int *res;
    int world_rank;
    int world_size;
    int *res_arr;
    int res_total[6];

    
    // Inicializar
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Datatype target_type;
    MPI_Type_contiguous(3,MPI_INT,&target_type);
    MPI_Type_commit(&target_type);

    b_areas = calloc(NUM_BOMBS*5,sizeof(int));

    if (world_rank == 0)
    {
        // Read

        // Bomb square areas to limit coordinates
        for (i = 0; i < NUM_BOMBS; ++i)
            radToArea(&bombs[i*RAW_BOMB_SIZE],&b_areas[i*BOMB_SIZE]);

        // printing new bomb areas
        for (i = 0; i < NUM_BOMBS; ++i)
            for (j = 0; j < 5; ++j) {
                printf("%3d", b_areas[i*BOMB_SIZE + j]);
                if (j == 4) printf("\n");
            }

        printf("===========================\n");
    }


    int num_elements_per_proc = 1 ; // ntargets / world_size


    //int parallel_targets[num_elements_per_proc][TARGET_SIZE];
    int *parallel_targets = (int *)malloc(num_elements_per_proc*sizeof(int *)*3);
    /*int **parallel_targets = (int **)malloc(num_elements_per_proc*sizeof(int));
    for (i = 0; i < num_elements_per_proc; ++i)
    {
        parallel_targets[i] = (int *) malloc(sizeof(int)*3);
    }*/



    /* n_elements_per_proc arrays of 3 ints*/
    //int **parallel_targets = (int **) calloc(num_elements_per_proc,sizeof(int*));

    /*for (i = 0; i < num_elements_per_proc; ++i)
        parallel_targets[i] = (int *) calloc(TARGET_SIZE,sizeof(int));

    printf("Aqui\n");
    if (world_rank == 0){
        for (i = 0; i < num_elements_per_proc; ++i)
            printf("%p\n",parallel_targets[i]);
    }*/


    /* Number of bombs arrays of size 5 (after conversion) */
    // int **bombs            = (int **) calloc(num_bombs,sizeof(int)* 5);
    // HACER E BROADCAST



    MPI_Scatter(targets          ,  num_elements_per_proc  , target_type, 
                parallel_targets ,  num_elements_per_proc  , target_type, 
                0, MPI_COMM_WORLD);

    MPI_Bcast(b_areas, BOMB_SIZE*5, MPI_INT,
              0,MPI_COMM_WORLD);



    /* Process each target and accumulate */ 
    res = process(parallel_targets,b_areas,num_elements_per_proc);

    if (world_rank == 0) {
        res_arr = (int *) malloc(sizeof(int) * 6 * world_size );
    }

    MPI_Gather( res    , 6, MPI_INT, 
                res_arr, 6, MPI_INT, 
                0, MPI_COMM_WORLD);

    if (world_rank == 0) {

        for (i = 0; i < 6; ++i) res_total[i] = 0;

        for (i = 0; i < world_size; ++i){
            for (j = 0; j < 6; ++j)
            {
               res_total[j] = res_total[j] + res_arr[6*i + j];
            }
        }
        printf("\n");
        printf("Military Targets totally destroyed: %d\n", res_total[0]);
        printf("Military Targets partially destroyed: %d\n", res_total[1]);
        printf("Military Targets not affected: %d\n", res_total[2]);
        printf("Civilian Targets totally destroyed: %d\n", res_total[3]);
        printf("Civilian Targets partially destroyed: %d\n", res_total[4]);
        printf("Civilian Targets not affected: %d\n", res_total[5]);
        printf("\n");
    }
/*
    for (i = 0; i < NUM_BOMBS; ++i)
        free(b_areas[i]);

#define DEAD_MIL    0
#define BROKEN_MIL  1
#define UNTOUCH_MIL 2
#define DEAD_CIV    3
#define BROKEN_CIV  4
#define UNTOUCH_CIV 5
*/
    MPI_Type_free(&target_type);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}