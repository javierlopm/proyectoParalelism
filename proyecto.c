#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>

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
int ttargets[15] = {
    0,0,  8,
    5,5,100,
    1,1, -2,
    7,7, -6,
    90,90,90
};

int tbombs[20] = {
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
    int map_size;
    int n_targets;
    int n_bombs;
    int *targets;
    int *bombs;
    
    // Inicializar
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Datatype target_type;
    MPI_Type_contiguous(3,MPI_INT,&target_type);
    MPI_Type_commit(&target_type);


    if (world_rank == 0)
    {
        // Read
        /* Start reading data */
        scanf("%d",&map_size);

        /* Reading targets */
        scanf("%d",&n_targets);
        targets = (int *) malloc(sizeof(int) * n_targets * 3);
        for (i = 0; i < n_targets; ++i)  
            scanf("%d %d %d\n",&targets[i*3],&targets[i*3 + 1],&targets[i*3 + 2]);
        

        /* Reading bombs */
        scanf("%d",&n_bombs);
        bombs   = (int *) malloc(sizeof(int) * n_bombs * 4);
        b_areas = (int *) malloc(sizeof(int) * n_bombs * 5);
        for (i = 0; i < n_bombs; ++i)
            scanf("%d %d %d %d\n"
                 ,&bombs[i*4]
                 ,&bombs[i*4 + 1]
                 ,&bombs[i*4 + 2]
                 ,&bombs[i*4 + 3]);
        


        // Bomb square areas to limit coordinates
        for (i = 0; i < n_bombs; ++i)
            radToArea(&bombs[i*RAW_BOMB_SIZE],&b_areas[i*BOMB_SIZE]);
        // printing new bomb areas
        /*
        for (i = 0; i < NUM_BOMBS; ++i)
            for (j = 0; j < 5; ++j) {
                printf("%3d", b_areas[i*BOMB_SIZE + j]);
                if (j == 4) printf("\n");
            }

        printf("===========================\n");*/
    }

    MPI_Bcast(&n_bombs, 1, MPI_INT,
              0,MPI_COMM_WORLD);
    MPI_Bcast(&n_targets, 1, MPI_INT,
              0,MPI_COMM_WORLD);

    if (world_rank != 0){
        b_areas = (int *) malloc(sizeof(int) * n_bombs   * 5);
        targets = (int *) malloc(sizeof(int) * n_targets * 3);
    }
    int num_elements_per_proc = n_targets / world_size ; // ntargets / world_size

    printf("%d\n", num_elements_per_proc);


    int *parallel_targets = (int *)malloc(num_elements_per_proc*sizeof(int)*3);


    MPI_Scatter(targets          ,  num_elements_per_proc  , target_type, 
                parallel_targets ,  num_elements_per_proc  , target_type, 
                0, MPI_COMM_WORLD);



    MPI_Bcast(b_areas, BOMB_SIZE*n_bombs, MPI_INT,
              0,MPI_COMM_WORLD);


    int missing_targets = n_targets % world_size;

    /* Process each target and accumulate */ 
    // printf("Procesando %d desde %d\n",num_elements_per_proc,world_rank);
    res = process(parallel_targets,b_areas,num_elements_per_proc);

    if (world_rank == 0)
        res_arr = (int *) malloc(sizeof(int) * 6 * (world_size + missing_targets));

    

    MPI_Gather( res    , 6, MPI_INT, 
                res_arr, 6, MPI_INT, 
                0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        /* Process missing targets in master */
        if (missing_targets > 0){
            int first_missing   = (n_targets - missing_targets) * 3;

            for ( i = first_missing; i < first_missing + 3; ++i)
                printf("%d ",targets[i]);
            printf("\n");

            printf("First missing %d and missing %d \n",first_missing,missing_targets);

            int * missing_elems = process(&targets[first_missing]
                                         ,bombs
                                         ,missing_targets);
            int i;
            /* Now we assign extra elements to results */
            printf("Asignando excedentes\n");
            for (i = 0; i < 6; ++i){
                printf("%d ",missing_elems[i]);
                res_arr[world_size*6 + i] = missing_elems[i];
            }
            printf("\n");

        } 

        for (i = 0; i < 6; ++i) res_total[i] = 0;

        printf("%d\n",world_size + (missing_targets > 0));

        /*Imprimiendo resultados*/

        for (i = 0; i < world_size + (missing_targets > 0); ++i){
            for (j = 0; j < 6; ++j){
                printf("%d ",res_arr[ i * 6 + j]);
            }
            printf("\n");
        }

        /* Iterate through stored results and maybe missing elements*/
        for (i = 0; i < world_size + (missing_targets > 0) ; ++i)
            for (j = 0; j < 6; ++j)
               res_total[j] = res_total[j] + res_arr[6*i + j];
            
        

        printf("\n");
        printf("Military Targets totally destroyed: %d\n", res_total[0]);
        printf("Military Targets partially destroyed: %d\n", res_total[1]);
        printf("Military Targets not affected: %d\n", res_total[2]);
        printf("Civilian Targets totally destroyed: %d\n", res_total[3]);
        printf("Civilian Targets partially destroyed: %d\n", res_total[4]);
        printf("Civilian Targets not affected: %d\n", res_total[5]);
        printf("\n");
    }

    MPI_Type_free(&target_type);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}