#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>      

#define DIST 0.001
#define MAX 10000000000
/*
struct arguments{
  int threads;
  int length;
  int power;

};
*/
struct newton_arguments{
  //  float ** initial;
  //int  ** iterations;
  //  int * power;
  //float ** roots_exact;
  int index;
  //  int * rowsize;
  int num_rows;
  //  int * rows_done;
  int ** result;
};
/*
  struct write_arguments{
    // int * power;
    //  int * size;
    //int * rows_done;
    int ** result;
    //int ** iterations;
  };
*/
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
   printf(" thread argument is %d\n",(int)res1);
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
   printf(" size argument is %d\n",(int)res2);
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
   printf(" length argument is %d\n",(int)res2);
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
   printf(" thread argument is %d\n",(int)res1);
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
   printf(" exponent is %d\n",(int)res3);
   }
  

   POWER=(int)res3;
   SIZE=(int) res2;
   THREADS=(int) res1;
   return;
}

void * threaded_newton(void * args){  // void * since we want it to work with threads
  
	//access args from struct
    struct newton_arguments *arg = args;
	
    //float ** initial = initial; //initial value pointer  
    //int ** iterations = iterations; // iteration pointer
	int d_loc = POWER;//*arg->power; //polynomial degree
	//int n_loc = arg->num_rows; //numrows_first ; how many rows this thread is handling
	//	float ** root_exact = roots_exact;
	int rowsize = SIZE;//*arg->rowsize; // how long each row is
	//int rownumber = arg->index; // index of the first row in the memory block so we know where to put it after computation
	//	int * rows_done = rows_done;  // pointer to keep track of calculated roots
	//int ** result = arg->result;
      
	
	float z_re;
	float z_im;
	float arg_z;
	float abs_z;
	float abs_d;
	int True;
	int iterations_loc;
	int which_root;
	
	printf("number of rows %d; initial x1 %f; initial y1 %f; rownumber %d; root real %f; root imag %f\n",n_loc,initial[n_loc-1][2*n_loc+1],initial[n_loc-1][2*n_loc+1],rownumber,root_exact[0][0],root_exact[0][1]);
     for(int ix = rownumber; ix < n_loc; ix++){
	for(int jx = 0; jx < rowsize; jx++){ 
		True = 0;
		iterations_loc=0; //keep track of iterations
		which_root=0;

		z_re=initial[ix][2*jx];
		z_im=initial[ix][2*jx+1];
		//Newtons method
	      
		while((True==0) && ( abs(z_re) < MAX) && (abs(z_im) < MAX)){
			arg_z = atan2(z_im, z_re);
			abs_z = z_re* z_re + z_im* z_im;
			abs_d=1;
   
			for(int i=1; i < d_loc-1; i+=2){
			    abs_d *= abs_z;
			    if(d_loc %2 == 0){
			      abs_d *= sqrt(abs_z);
			    }
			}
			z_re = (z_re*(d_loc - 1) + 1/abs_d*cos(arg_z*(1-d_loc)))/ d_loc;
			z_im = (z_im*(d_loc -1) +1/abs_d*sin(arg_z*(1-d_loc)))/ d_loc;
						
		       
						
			//distance check from exact roots;
			for(int kx=0 ; kx < d_loc; kx++){
				if((z_re-root_exact[kx][0])*(z_re-root_exact[kx][0]) + (z_im-root_exact[kx][1])*(z_im-root_exact[kx][1]) < DIST*DIST){
					True = 1;
					which_root=kx;
					break;
				}
			}
			//printf("iteration continues\n");
			iterations_loc++;
		}
		if(True){
		  // printf("------------------Found a root!!-------------------\n");
		}else{
		  printf("--------------------Diverged-------------------\n");
		}
		
		//pthread_mutex_lock(&mutex_data);
	        result[ix][jx]=which_root;
		iterations[ix][jx] = iterations_loc;
		//printf("res: %d\n",result[ix][jx]);
		//pthread_mutex_unlock(&mutex_data);
	}
 	        pthread_mutex_lock(&mutex_write); // unnecessary? just keep it in the data mutex?
		//printf("index: %d;\n",ix);
		rows_done[ix] = 1;
		pthread_mutex_unlock(&mutex_write);
	     
     }
     printf("-------------DONE WITH THREAD-------------\n");
	return NULL;
}

void * writeppm(void * args) { // void * since we want it to work with threads

  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in _xd.pmm is the power of x
  /*
  struct write_arguments *arguments =args;
  int * power= arguments->power;
  int * size = arguments->size;
  int * rows_done=arguments->rows_done;
  int ** iterations= arguments->iterations;
  int ** result = arguments->result;
  
  int sum_array=0;
  const int SIZE =*size;
  int rows_written[SIZE]; // initialise???
   
   // creating local data arrays;
   
   u_char * iterations_mat=(u_char *)malloc(SIZE*SIZE*3);
   u_char * result_mat= (u_char *)malloc(SIZE*SIZE*3);
   u_char ** result_loc=(u_char **)malloc(SIZE*sizeof(u_char *));
   u_char ** iterations_loc=(u_char **)malloc(SIZE*sizeof(u_char *));

   for(size_t ix=0, jx=0; ix<SIZE;ix++, jx+=SIZE){
     result_loc[ix]=result_mat + 3*jx;
     iterations_loc[ix]=iterations_mat + 3*jx;
   }
   int current_index;
   printf("----------Starting looping in writeppm------------\n");
   while(sum_array<SIZE){
    sum_array=0;
    // pause to let threads finish rows?
    pthread_mutex_lock(&mutex_write);
    for(size_t index=0;index<SIZE;index++){
      sum_array+=rows_written[index];
      if(rows_done[index] && !rows_written[index]){
	// if found entry:  note that it is done write that row to file
	rows_written[index]=1;
	current_index=index;
	break;
      }
    }    
      pthread_mutex_unlock(&mutex_write);
  	 if(sum_array<1){
	   continue;
	 }
	 if(sum_array==SIZE){
	   break;
	   }  
               
     // fill the data array, Mutex necessary???
	 //printf("result; current_index: %d; rows_done %d; result: %d\n",current_index,rows_done[current_index],result[current_index][0]);
	 pthread_mutex_lock(&mutex_data);
     for (size_t y = 0; y < SIZE; y++) { // what do we do here?
       for (size_t x=0;x<3;x++){
	 // printf("result[current_index][y]:%d\n",result[current_index][y]);
	 result_loc[current_index][3*y+x] = (u_char) result[current_index][y];
	 iterations_loc[current_index][3*y+x] = (u_char) iterations[current_index][y]; // bitwise operation & ok????
       }
     }    
       pthread_mutex_unlock(&mutex_data);
   }
        printf("-----------Got out of while loop and going to writing stage------------\n");
   // create filenames
   char str[26];
  char str2[26];
  sprintf(str, "newton_attractors_x%i.ppm", *power);
  sprintf(str2, "newton_convergence_x%i.ppm", *power);
  FILE * fp;
  FILE * fp2;
  
   // write the whole data array to ppm file in one step
     fp = fopen(str, "w");
     fprintf(fp,"P3\n%d %d\n%d\n",size,size,255); // header

     //write image data bytes to the file 
     //fwrite(result_mat, SIZE*SIZE*3 , 1, fp); // works??
     fclose(fp);

     
     fp2 = fopen(str2, "w");
     fprintf(fp2,"P3\n%d %d\n%d\n",size,size, 255); // header

     //write image data bytes to the file 
     //fwrite(iterations_mat, SIZE*SIZE*3, 1, fp); // works??
     fclose(fp2);
     
     printf("-----------OK - file %s and %s saved\n------------", str,str2);
     free(result_loc);
     free(result_mat);
     free(iterations_loc);
     free(iterations_mat);
  */
  return NULL;
}


int main(int argc, char * argv[] ){
  // Grupp: hpcgp017

    
   // Parse command line arguments
  //printf("No. args %d; Arg %s, %s, %s, %s\n",argc, argv[0],argv[1],argv[2],argv[3]);
  // struct arguments A;
  parse_args(argv);
  //int size=SIZE;
  int thread_count=THREADS;
  //int power=POWER;
  
 
  //printf("-------------------Arguments parsed----------------\n");
  printf("Arguments: 1:%d 2:%d 3:%d\n",THREADS, SIZE, POWER); // remove later

  

   
  // calculating amount of rows that will be available to each thread
   int numrest=SIZE%thread_count;
   int numrows=SIZE/thread_count;


   float * initial_mat =(float *)malloc(2*sizeof(float*)*SIZE*SIZE);
   initial = (float**) malloc(2*sizeof(float*)*SIZE); // initial values to newton
   int * iterations_mat= (int *)malloc(sizeof(int*)*SIZE);
   iterations = (int**) malloc(sizeof(int*) * SIZE);
   int * result_mat= (int *)malloc(sizeof(int*)*SIZE*SIZE);
   int ** result = (int**) malloc(sizeof(int*) * SIZE);
   
  

   
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
     printf("woop %f %f\n", roots_exact[i][0],roots_exact[i][1]);
   }
   
   //   printf("initial x %f, initital y %f \n", initial[size-1][2*(size-1)+1],initial[size-1][2*(size-1)+1]);
   // printf("----------------After memory allocation---------------\n");
   int ret;
   pthread_t threads[THREADS+1]; // create one extra for writing to file
   
   pthread_mutex_init(&mutex_write, NULL); // mutex for accessing the rows_done array
 

// precalculate the roots to the chosen polynomial
    printf("-----------Start writing thread----------\n");
// do writing thread
    
    //  struct write_arguments arg;
   // arg.power=&power; // exponent
    //  arg.size=&size; // length
    //arg.rows_done=rows_done;
    //arg.result=result;
    //arg.iterations=iterations;
    if (ret = pthread_create(threads+thread_count, NULL, writeppm, (void*) &arg)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
    //    printf("power: %d",power);
 printf("-----------Start Newton threads----------\n");
// do first thread with the extra rows that may exist if #rows is not divisible by #threads
 //int numrows_first = numrows+numrest;
 //int *index=malloc(4*sizeof(int));
 //index[0]=0;
 //for(size_t i=1;i<THREADS;i++){
   //index[i]=numrows_first+(i-1)*numrows;
   // }
 struct newton_arguments args[thread_count];
 //args[0].initial = initial;
 //  args[0].result = result;
  //args[0].iterations = iterations;
  //args[0].rowsize=&size;
  // args[0].power = &power;
  //args[0].num_rows = numrows_first;
  // args[0].index=index[0];
  //args[0].roots_exact = roots_exact;
  //args[0].rows_done=rows_done;
  if (ret = pthread_create(threads, NULL, threaded_newton, (void*) &args[0])) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }


  
// do the rest of the threads
    for (size_t tx=1, ix=numrows_first; tx < THREADS; ++tx, ix+=numrows){ 
      // printf("numrows %d; index %d\n",numrows,ix);
 //args[tx].initial = initial;
 // args[tx].iterations = iterations;
 // args[tx].result = result;
   //args[tx].power = &power;
   //args[tx].rowsize=&size;
   //args[tx].index=index[tx]; 
   //args[tx].num_rows= numrows_first+numrows*tx;
   //args[tx].roots_exact = roots_exact;
   //args[tx].rows_done=rows_done;
   if (ret = pthread_create(threads+tx, NULL, threaded_newton, (void *)&args[tx])) {
      printf("Error creating thread: %\n", ret);
      exit(1);
    }
  }
    printf("---------------All threads created-----------\n");
  // joining threads if done
    
  for (size_t tx=0; tx < THREADS; ++tx) { // +1 for write thread
    if (ret = pthread_join(threads[tx], NULL)) {
      printf("Error joining thread: %d\n", ret);
      exit(1);
    }
  }
  printf("-------------------JOINED THREADS-----------------\n");
   pthread_join(threads[thread_count],NULL);
   printf("-----------JOINED WRITE THREAD---------------\n");
 
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
  free(index);
  
  return 0;  
}
