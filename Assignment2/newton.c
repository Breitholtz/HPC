#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>      
#include <time.h>
#define DIST 0.001
#define MAX 10000000000
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
int MAX_ITER =1000;

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
     //   printf(" thread argument is %d\n",(int)res1);
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
     //printf(" size argument is %d\n",(int)res2);
   }  
  }else{
  printf("Unknown first argument!!!\n");
  exit(1);
 }


  // argument 2
 
 if(strncmp(args[2], "-l",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[2])-1];
   strncpy(T,args[2]+2,strlen(args[2])-2);
   T[strlen(args[2])-2]='\0';
    res2 = strtol(T,&rest2,10);
   if(strcmp(rest2,"")!=0 || res2<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 2!\n",res2,rest2);
     exit(1);
   }else{
     // printf(" length argument is %d\n",(int)res2);
   }
 }else if(strncmp(args[2], "-t",2)==0){
 char  T[strlen(args[2])-1];
   strncpy(T,args[2]+2,strlen(args[2])-2);
   T[strlen(args[2])-2]='\0';
   res1 = strtol(T,&rest1,10);
   if(strcmp(rest1,"")!=0 || res1<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 1!\n",res1,rest1);
     exit(1);
   }else{
     //printf(" thread argument is %d\n",(int)res1);
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
     // printf(" exponent is %d\n",(int)res3);
   }
  

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
	int which_root = POWER;
	
	
    while(Row < SIZE){
	for(int jx = 0; jx < SIZE; jx++){ 
		True = 0;
		iterations_loc=0; //keep track of iterations
		z_re=initial[Row][2*jx];
		z_im=initial[Row][2*jx+1];
		//Newtons method
		while((True==0) && ( fabs(z_re) < MAX) && (fabs(z_im) < MAX)&&(iterations_loc<MAX_ITER)){
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
 	    pthread_mutex_lock(&mutex_write); 
		rows_done[Row] = 1;
		Row=Index;
		Index++;
	    pthread_mutex_unlock(&mutex_write);
	     
     }
    //    printf("-------------DONE WITH THREAD-------------\n");
	return NULL;
}

void * writeppm(void * args) { // void * since we want it to work with threads

  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in _xd.pmm is the power of x
  char * colours[10];     // hardcoded colors for different results, on the stack
  char * grayscale;     // 
  for (size_t i=0;i<9;i++){
    sprintf(colours[i], "%d 0 0", 20*(9-i));
	
  }
  	
	// create filenames
	char str[26];
	char str2[26];
	sprintf(str, "newton_attractors_x%i.ppm", POWER);
	sprintf(str2, "newton_convergence_x%i.ppm", POWER);
	FILE * fp;
	FILE * fp2;
	fprintf(fp, "P3\n %d %d\n %d", SIZE,SIZE,255); // colour header 
	fprintf(fp2, "P2\n %d %d\n %d", SIZE,SIZE,MAX_ITER); // grayscale header
        /*
	size_t ix=0;
	while(ix < SIZE){
		if(rows_done[ix] != 0){
			//write to file
		  for (size_t i=0;i<SIZE;i++){
		  fprintf(fp, "%d %d %d ", COLOR_VALUE0, COLOR_VALUE1, COLOR_VALUE2);
                  fprintf(fp2, "%d %d %d ", GRAY_VALUE, GRAY_VALUE, GRAY_VALUE);
		  }
			ix++
		}
		// continue waiting for next row
	}
	*/
	fclose(fp);
        fclose(fp2);
   
     /*   
      for( size_t i=0; i<SIZE;i++){
      string=concat(string,colors[j][i])
}
fwrite(string)
     printf("-----------OK - file %s and %s saved\n------------", str,str2);
     
  */
  return NULL;
}


int main(int argc, char * argv[] ){
  // Grupp: hpcgp017

  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  long sec1=ts.tv_sec;
  long nsec1=ts.tv_nsec;
  parse_args(argv); 
  //printf("Arguments: 1:%d 2:%d 3:%d\n",THREADS, SIZE, POWER); // remove later

  

   
  // calculating amount of rows that will be available to each thread
   int numrest=SIZE%THREADS;
   int numrows=SIZE/THREADS;


   float * initial_mat =(float *)malloc(2*sizeof(float*)*SIZE*SIZE);
   initial = (float**) malloc(2*sizeof(float*)*SIZE); // initial values to newton
   int * iterations_mat= (int *)malloc(sizeof(int*)*SIZE*SIZE);
   iterations = (int**) malloc(sizeof(int*) * SIZE);
   int * result_mat= (int *)malloc(sizeof(int*)*SIZE*SIZE);
   result = (int**) malloc(sizeof(int*) * SIZE);
   
  

   
   for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE ){ // setting pointers to every row in memory
       result[ix]= result_mat + jx;
       iterations[ix] = iterations_mat + jx;
   }
   for(size_t i=0, j = 0; i<SIZE;i++,j+=2*SIZE ){
     initial[i]= initial_mat + j;
   }
   
   for(size_t ix=0;ix<SIZE;ix++){// row
     for (size_t jx =0;jx<2*SIZE;jx+=2){//column
       initial[ix][jx]=-2+(2*jx)/(float)(SIZE-1); // initial x
       initial[ix][jx+1]=2-(2*jx)/(float)(SIZE-1); // initial y
     }
   }

   rows_done =malloc(sizeof(int*)*SIZE);
   for (size_t i=0;i<SIZE;i++){
     rows_done[i]=0;
   }

   
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

   // printf("----------------After memory allocation---------------\n");
   int ret;
   pthread_t threads[THREADS+1]; // create one extra for writing to file
   
   pthread_mutex_init(&mutex_write, NULL); // mutex for accessing the rows_done array
 

   // printf("-----------Start writing thread----------\n");
// do writing thread
    
    if (ret = pthread_create(threads+THREADS, NULL, writeppm, NULL)) {
      // printf("Error creating thread: %\n", ret);
    exit(1);
  }
    //    printf("power: %d",power);
    // printf("-----------Start Newton threads----------\n");
 Index=THREADS;
 int Row; // starting row for each of the threads
	for(int tx = 0; tx < THREADS; tx++){
	  Row = tx;
		if (ret = pthread_create(threads + tx, NULL, threaded_newton, (void*)&Row)) {
		printf("Error creating thread: %\n", ret);
		exit(1);
		  }
	}
 
	//  printf("---------------All threads created-----------\n");
  // joining threads if done
    
  for (size_t tx=0; tx < THREADS; ++tx) { 
    if (ret = pthread_join(threads[tx], NULL)) {
      printf("Error joining thread: %d\n", ret);
      exit(1);
    }
  }
  //printf("-------------------JOINED THREADS-----------------\n");
   pthread_join(threads[THREADS],NULL);
   // printf("-----------JOINED WRITE THREAD---------------\n");
 
  pthread_mutex_destroy(&mutex_write);
  free(result);
  free(result_mat);
  free(iterations);
  free(iterations_mat);
  free(initial);
  free(initial_mat);
  free(roots_exact);
  free(all_roots_exact);
  free(rows_done);

  timespec_get(&ts, TIME_UTC);
  printf("secs: %ld nsec: %ld \n",(ts.tv_sec-sec1), ts.tv_nsec-nsec1);
  return 0;  
}
