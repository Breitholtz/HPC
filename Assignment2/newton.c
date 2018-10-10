#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>      
#include <time.h>
#define DIST 0.001
#define MAX 10000000000
#define MAX_ITER 100

pthread_mutex_t mutex_write; 

// GLOBAL VARIABLES
int THREADS;
int POWER;
int SIZE;
float ** initial;
int ** iterations;
float ** roots_exact;
int * rows_done;
int ** result;
int Index;

char color[10][13] = {"255 204 153\t", "255 255 153\t", "204 255 150\t", "153 255 204\t", "153 255 255\t", "153 204 255\t", "153 153 255\t", "204 153 255\t", "255 204 255\t", "255 153 204\t"};
char greyscale[MAX_ITER+1][13];

void parse_args(char * args[]){
  
  char * rest1;
  char * rest2;
  char * rest3;


  // argument 1
  long res1;
  long res2;
  if(strncmp(args[1], "-t",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[1])-1];
   strncpy(T,args[1]+2,strlen(args[1])-2);
   T[strlen(args[1])-2]='\0';
   res1 = strtol(T,&rest1,10);
   if(strcmp(rest1,"")!=0 || res1<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 1!\n",res1,rest1);
     exit(1);
   }else{
     // successfully parsed
  }
  }else if(strncmp(args[1], "-l",2)==0){
  char  T[strlen(args[1])-1];
   strncpy(T,args[1]+2,strlen(args[1])-2);
   T[strlen(args[1])-2]='\0';
    res2 = strtol(T,&rest2,10);
   if(strcmp(rest2,"")!=0 || res2<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 2!\n",res2,rest2);
     exit(1);
   }else{
     // successfully parsed
   }  
  }else{
    printf("Unknown first argument!!!\n"); // unknown flag
  exit(1);
 }


  // argument 2
 
 if(strncmp(args[2], "-l",2)==0){ // then take the rest of the string and convert to long
    
    char  T2[strlen(args[2])-1];
   strncpy(T2,args[2]+2,strlen(args[2])-2);
   T2[strlen(args[2])-2]='\0';
    res2 = strtol(T2,&rest2,10);
   if(strcmp(rest2,"")!=0 || res2<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 2!\n",res2,rest2);
     exit(1);
   }else{
     // successfully parsed
   }
 }else if(strncmp(args[2], "-t",2)==0){
 char  T2[strlen(args[2])-1];
   strncpy(T2,args[2]+2,strlen(args[2])-2);
   T2[strlen(args[2])-2]='\0';
   res1 = strtol(T2,&rest1,10);
   if(strcmp(rest1,"")!=0 || res1<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 1!\n",res1,rest1);
     exit(1);
   }else{
     // successfully parsed
   }
 }else{
  printf("Unknown second argument!!!\n");
  exit(1);
 }

 // argument 3    
   long res3 = strtol(args[3],&rest3,10);
   if(strcmp(rest3,"")!=0 || res3<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 3!\n",res3,rest3);
     exit(1);
   }else{
     // successfully parsed
   }
  
   // Set parsed arguments to global variables
   POWER=(int)res3;
   SIZE=(int) res2;
   THREADS=(int) res1;
   return;
}

void * threaded_newton(void * args){  // void * since we want it to work with threads
          
	int * I=(int *)args;
	int Row=*I;
	float z_re;
	float z_im;
	float arg_z;
	float abs_z;
	float abs_d;
	int True;

	int iterations_loc;
	int which_root;
	
	while(Row < SIZE){
	  for(int jx = 0; jx < SIZE; jx++){ 
		True = 0;
		iterations_loc=0; //keep track of iterations
		z_re=initial[Row][2*jx];
		z_im=initial[Row][2*jx+1];
		which_root =POWER;
		//Newtons method
		while((True==0) && ( fabs(z_re) < MAX) && (fabs(z_im) < MAX)){
			arg_z = atan2(z_im, z_re);
			abs_z = z_re* z_re + z_im* z_im;
			abs_d=1;
   
			for(int i=1; i < POWER-1; i+=2){
			    abs_d *= abs_z;
			    if(POWER %2 == 0){
			      abs_d *= sqrt(abs_z);
			    }
			}
			z_re = (z_re*(POWER - 1) + 1/abs_d*cos(arg_z*(1-POWER)))/ POWER;
			z_im = (z_im*(POWER -1) +1/abs_d*sin(arg_z*(1-POWER)))/ POWER;
						
		       
			//distance check from exact roots;
			for(int kx=0 ; kx < POWER; kx++){
				if((z_re-roots_exact[kx][0])*(z_re-roots_exact[kx][0]) + (z_im-roots_exact[kx][1])*(z_im-roots_exact[kx][1]) < DIST*DIST){
					True = 1;
					which_root=kx;
					break;
				}
			}
			iterations_loc++;
		}
	       	result[Row][jx]=which_root;
		iterations[Row][jx] = iterations_loc;
		// set colours here?
	}
		rows_done[Row] = 1;
		pthread_mutex_lock(&mutex_write);
        Row=Index;
		Index++;
	    pthread_mutex_unlock(&mutex_write);
    }
  	return NULL;
}

void * writeppm(void * args) { // void * since we want it to work with threads
    
  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in _xd.pmm is the power of x
  
  //struct timespec sleep_ts;
  //sleep_ts.tv_nsec=100000;
  //sleep_ts.tv_sec=0;
  
  int cap;
  for (size_t ix = 0; ix < MAX_ITER + 1; ++ix) {
    sprintf(greyscale[ix], "%ld %ld %ld\t", ix, ix, ix);
  }
	// create filenames
	char str[26];
	char str2[26];
	sprintf(str, "newton_attractors_x%i.ppm", POWER);
	sprintf(str2, "newton_convergence_x%i.ppm", POWER);
	FILE * fp;
	FILE * fp2;
	fp=fopen(str,"w");
	fp2=fopen(str2,"w");
	fprintf(fp, "P3\n %d %d\n %d\n", SIZE,SIZE,255); // colour header  
	fprintf(fp2, "P3\n %d %d\n %d\n", SIZE,SIZE,MAX_ITER); // grayscale header

	size_t ix=0;
	
	while(ix < SIZE){
		if(rows_done[ix] != 0){
		  //write to file
		  
		  for (size_t jx = 0; jx < SIZE; ++jx) { //prepare string?
		    fwrite(color[result[ix][jx]],1,strlen(color[result[ix][jx]]),fp);
		    cap = iterations[ix][jx] > MAX_ITER ? MAX_ITER : iterations[ix][jx];
		    fwrite(greyscale[cap],1,strlen(greyscale[cap]),fp2);
		  }
		  fwrite("\n",1,1,fp); 
		  fwrite("\n",1,1,fp2);
		  ix++;
		  
		}
		  
		//nanosleep(&sleep_ts,NULL);
		// continue waiting for next row
	}
       
       	fclose(fp);
        fclose(fp2);
	//printf("-----------OK - file %s and %s saved\n------------", str,str2);
	
  return NULL;
}


int main(int argc, char * argv[] ){
  // Grupp: hpcgp017

  /*struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  long sec1=ts.tv_sec;
  long nsec1=ts.tv_nsec; */
  parse_args(argv); 

    //allocate memory for matrices in order to store results
   float * initial_mat =(float *)malloc(2*sizeof(float*)*SIZE*SIZE);
   initial = (float**) malloc(2*sizeof(float*)*SIZE); // initial values to newton
   int * iterations_mat= (int *)malloc(sizeof(int*)*SIZE*SIZE);
   iterations = (int**) malloc(sizeof(int*) * SIZE);
   int * result_mat= (int *)malloc(sizeof(int*)*SIZE*SIZE);
   result = (int**) malloc(sizeof(int*) * SIZE);
   
	// setting pointers to every row in memory
   for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE ){
       result[ix]= result_mat + jx;
       iterations[ix] = iterations_mat + jx;
   }
   for(size_t i=0, j = 0; i<SIZE;i++,j+=2*SIZE ){
     initial[i]= initial_mat + j;
   }
   
   for(size_t ix=0;ix<SIZE;ix++){// row
     for (size_t jx =0;jx<2*SIZE;jx+=2){//column
       initial[ix][jx]=-2+(2*jx)/(float)(SIZE-1); // initial x
       initial[ix][jx+1]=2-(4*ix)/(float)(SIZE-1); // initial y
     }
   }
	
	//keep track of calculated rows
   rows_done =calloc(SIZE, sizeof(int));
   
   //calculate exact roots 
   float * all_roots_exact = malloc(2*POWER*sizeof(float));
   roots_exact = malloc(POWER*sizeof(float*));
   
   for (size_t i=0, j=0;i<POWER;i++ ,j+=2){
     roots_exact[i]= all_roots_exact + j;
   }
   
   /* One class of polynomial: x^d - 1 = 0; which means that one root is always 1; and the other are complex roots 2*pi*j/d where j=1,...,d-1  */
   roots_exact[0][0] = 1.0f;
   roots_exact[0][1] = 0.0f;
   for(size_t i=1; i<POWER; i++ ){
     float theta = i*2*M_PI/POWER;
     roots_exact[i][0] = cos(theta);
     roots_exact[i][1] = sin(theta);
   }

   //Treading
   pthread_t threads[THREADS+1]; // create one extra for writing to file   
   pthread_mutex_init(&mutex_write, NULL); // mutex for accessing the rows_done array
 
   //-------------CREATING THREADS-------------
	int ret;
    Index=THREADS;
    int  * Row=malloc(sizeof(int)*THREADS); // starting row for each of the threads
	
	//Start Newton threads
    for(int tx = 0; tx < THREADS; tx++){
      Row[tx] = tx;
      if (ret = pthread_create(threads + tx, NULL, threaded_newton, (void*)&Row[tx])) {
	printf("Error creating thread: %\n", ret);
	exit(1);
      }
    }
	
	//start writing thread
    if (ret = pthread_create(threads+THREADS, NULL, writeppm, NULL)) {
       printf("Error creating thread: %\n", ret);
    exit(1);
    }
	
    //-----------------JOINING THREADS--------------
    //Join 
	for (size_t tx=0; tx < THREADS + 1; ++tx) { 
		if (ret = pthread_join(threads[tx], NULL)) {
		printf("Error joining thread: %d\n", ret);
		exit(1);
		}
	}
  
	pthread_mutex_destroy(&mutex_write);

  // ------------ FREEING ALLOCATED MEMORY-----------
  free(result);
  free(result_mat);
  free(iterations);
  free(iterations_mat);
  free(initial);
  free(initial_mat);
  free(roots_exact);
  free(all_roots_exact);
  free(rows_done);
  free(Row);
  //timespec_get(&ts, TIME_UTC);
  //printf("secs: %ld nsec: %ld \n",(ts.tv_sec-sec1), ts.tv_nsec-nsec1);
  return 0;  
}
