#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "acb_poly.h"  //   http://arblib.org/acb_poly.html#acb-poly     - necessary?
#include <math.h>      
#include "acb.h"
#include "arb_fmpz_poly.h"
#include "flint/arith.h" // necessary?

struct arguments{
  int threads;
  int length;
  int power;

};


struct newton_arguments{
  float ** roots;
  int  ** iterations;
  int * power;
  float ** roots_exact;
  int * index;
  int * rowsize;
  int * num_rows;
  int * rows_done;
};
  struct write_arguments{
    int * power;
    int * size;
    int * rows_done;
  };



//struct arguments is what we use to pass out multiple return values
struct arguments parse_args(char * args[]){
  
  char * rest1;
  char * rest2;
  char * rest3;



  // argument 1
  long res1;
  if(strncmp(args[1], "-t",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[1])-1];
   strncpy(T,args[1]+2,strlen(args[1])-2);
   T[strlen(args[1])-2]='\0';
   res1 = strtol(T,&rest1,10);
   if(strcmp(rest1,"")!=0 || res1<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 1!\n",res1,rest1);
     exit(1);
   }else{
   printf(" arg1 is %d\n",(int)res1);
   }
  }else{
  printf("Missing -t flag for first argument!!!\n");
  exit(1);
 }


  // argument 2
  long res2;
 if(strncmp(args[2], "-l",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[2])-1];
   strncpy(T,args[2]+2,strlen(args[2])-2);
   T[strlen(args[2])-2]='\0';
    res2 = strtol(T,&rest2,10);
   if(strcmp(rest2,"")!=0 || res2<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 2!\n",res2,rest2);
     exit(1);
   }else{
   printf(" arg2 is %d\n",(int)res2);
   }
  }else{
  printf("Missing -l flag for second argument!!!\n");
  exit(1);
 }

 // argument 3    
   long res3 = strtol(args[3],&rest3,10);
   if(strcmp(rest3,"")!=0 || res3<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 3!\n",res3,rest3);
     exit(1);
   }else{
   printf(" arg3 is %d\n",(int)res3);
   }
  


   struct arguments A;
   A.threads=(int)res1;
   A.length=(int)res2;
   A.power=(int)res3;
  //free(args); // we should free something here probably...
   return A;  
}

void * threaded_newton(void * args){  // void * since we want it to work with threads

	// Just change memory in the function and return nothing.
	// plan: break up square (-2,2)^2 into size intervals for x and y using cos(2pi*(n/d)) and sin(2pi*(n/d)), where n=0,...,d-1 ; 
	// use newtons to approximate the root to 10^-3 precision (in absolute value); 
	//if divergent (absolute value larger than 1e10) or very close to origin(1e-3) say that it converged to 0; 
	//note which root and how many iterations was necessary;
	// actual iterative formula (newton)
	// double sin(double x) is math lib function syntax
  
	//access args from struct
    struct newton_arguments *arg = args;
	
	float ** root_loc = arg->roots; //root pointer  
	int ** iterations_loc = arg->iterations; // iteration pointer
	int * d_loc = arg->power; //polynomial degree
	int * n_loc = arg->num_rows; //numrows_first ; how many rows this thread is handling
	float ** root_exact_loc = arg->roots_exact;
	int * rowsize = arg->rowsize; // how long each row is
	int * rownumber = arg->index // index of the first row in the memory block so we know where to put it after computation
	int * rows_done = arg->rows_done  // pointer to keep track of calculated roots

   
   
	for(/* loop over rowsize of initaial values */){
	//Size represents number of rows each thread will calculate, size = *n_loc! 
		float *z_re; 
		float *z_im;
		int True = 0;
		*iterations_loc=0; //keep track of iteratians, supposed to be assignd to iterations_loc
		
		//Newtons method
		
		while((!True) || ( abs(z_re) < MAX) || (abs(z_im) < MAX)){
			float arg_z = atan2(*z_im, *z_re);
			float abs_z = *z_re* *z_re + *z_im* *z_im;
			float abs_d = 1;
			
			for(int i=1; i < *d_loc-1; i+=2)
				abs_d *= abs_z;
			if(*d_loc %2 == 0)
				abs_d *= sqrt(abs_z);
			
			*z_re = (*z_re*(*d_loc - 1) + abs_d*cos(arg_z*(1-*d_loc)))/ *d_loc;
			*z_im = (*z_im*(*d_loc -1) +abs_d*sin(arg_z*(1-*d_loc)))/ *d_loc;
			
			//distance check from exact root;
			for(int ix=0 ; ix < *d_loc -1; ix++){
				if((*z_re-**root_exact[ix][0])*(*z_re-**root_exact[ix][0]) + (*z_im-**root_exact[ix][1])*(*z_im-**root_exact[ix][1]) < DIST*DIST);
					True = 1;
			}
			*iterations_loc++;
			return 0;
		}

		
		pthread_mutex_lock(&mutex_data);
		//root[*rownumber][2*ix] = *z_re;
		//root[*rownumber][2*ix+1] = *z_im;
		iterations[*rownumber][ix] = *iterations_loc;
		//rows_done[ix] = 1;
		pthread_mutex_unlock(&mutex_data);
		return NULL;
	}
}

void * writeppm(void * args) { // void * since we want it to work with threads

  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in _xd.pmm is the power of x
  
  struct write_arguments *arguments =args;
  int * power= arguments->power;
  int * size = arguments->size;
  char str[26];
  sprintf(str, "newton_attractors_x%i.ppm", *power);
  FILE * fp  =fopen(str,"w");
  fprintf(fp,"P3\n%d %d\n%d\n",size,size,*power);
  fclose(fp);
  sprintf(str, "newton_convergence_x%i.ppm", *power);  
  FILE * fp2  =fopen(str,"w");
  fprintf(fp2,"P3\n%d %d\n%d\n",size,size, *power); 
  fclose(fp2);

  
  //fwrite the info that is available from the newton function
   int sum_array=0;
   while(sum_array<2*size){
    sum_array=0;
    // pause to let threads finish rows?
    mutex_lock(&mutex_write);
    for(size_t index=0;index<size;index++){
      sum_array+=rows_done[index];
      if(rows_done[index]){
	// - if found entry: set entry to 2 and write that row to file
	rows_done[index]=2;
	mutex_unlock(&mutex_write);
	break;
      }
    }
    // write to file here
     }
 
  return NULL;
}

int main(int argc, char * argv[] ){
  // Grupp: hpcgp017

    
  int size;
  int thread_count;
  int power;
  
  // Parse command line arguments
  printf("No. args %d; Arg %s, %s, %s, %s\n",argc, argv[0],argv[1],argv[2],argv[3]);
  struct arguments A;
  A=parse_args(argv);
  printf("-------------------Arguments parsed----------------\n");
  printf("Arguments: 1:%d 2:%d 3:%d\n",A.threads, A.length, A.power); // remove later

  // take our parsed arguments
  size=A.length;
  thread_count=A.threads;
  power=A.power;

   
  // calculating amount of rows that will be available to each thread
   int numrest=size%thread_count;
   int numrows=size/thread_count;


   float * roots_mat =(float *)malloc(2*sizeof(float*)*size*size);
   float ** roots = (float**) malloc(2*sizeof(float*)*size);
   int * iterations= (int *)malloc(sizeof(int*)*size*size);
   int ** iter = (int**) malloc(sizeof(int*) * size);
  

   
   for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=2*size ){ // setting pointers to every row in memory
       iter[ix] = iterations + jx;
       roots[ix] = roots_mat+jx;
   }
   
   for(size_t ix=0;ix<size;ix++){
     for (size_t jx =0;jx<size;jx+=2){
       roots[ix][jx]=-2+(2*ix)/(float)size; // calculating 
       roots[ix][jx+1]=2-(2*jx)/(float)size;
     }
   }
   int * rows_done =malloc(sizeof(int*)*size);
   for (size_t i=0;i<size;i++){
     rows_done[i]=0;
   }
   
   int ret;
   pthread_t threads[thread_count+1]; // create one extra for writing to file
   pthread_mutex_t mutex_data; 
   pthread_mutex_init(&mutex_data, NULL); // mutex for accessing the data arrays

   //probably not necessary
   pthread_mutex_t mutex_write;
   pthread_mutex_init(&mutex_write, NULL); // mutex for accessing the rows_done array
 

// precalculate the roots to the chosen polynomial
   
   float * all_roots_exact = malloc(2*power*sizeof(float));
   float ** roots_exact = malloc(2*power*sizeof(float*));
   for (size_t i=0, j=0;i<2;i++ ,j+=power){
     roots_exact[i]= all_roots_exact + j;
   }
   
   /* One class of polynom: x^d - 1 = 0; which means that one root is always Re(root) = 1; end the other are complex roots 2*pi*j/d where j=1,...,d-1  */
   roots_exact[0][0] = 1.0f;
   roots_exact[0][1] = 0.0f;
   for(size_t i=1; i<power; i++ ){
     float theta = i*2*M_PI/power;
     roots_exact[i][0] = cos(theta);
     roots_exact[i][1] = sin(theta);
   } 
// do writing thread
   struct write_arguments arg;
    arg.power=&power; // exponent
    arg.size=&size; // length
    if (ret = pthread_create(threads+thread_count, NULL, writeppm, &arg)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
    
// do first thread with the extra rows that may exist if #rows is not divisible by #threads
 int numrows_first = numrows+numrest;
 int index=1;
 struct newton_arguments args;
  args.roots = roots;
  args.iterations = iter;
  args.rowsize=&size;
  args.power = &power;
  args.num_rows = &numrows_first;
  args.index=&index;
  args.roots_exact = roots_exact;
  args.rows_done=rows_done;
  if (ret = pthread_create(threads, NULL, threaded_newton, &args)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }


  
// do the rest of the threads
  for (int tx=1, ix=numrows_first; tx < thread_count; ++tx, ix+=numrows){ 
struct newton_arguments args2;
  args2.roots = roots+ix;
  args2.iterations = iter+ix;
  args2.power = &power;
  args2.rowsize=&size;
  args2.index=&ix;
  args2.num_rows= &numrows;
  args2.roots_exact = roots_exact;
  args2.rows_done=rows_done;
    if (ret = pthread_create(threads+tx, NULL, threaded_newton, &arg)) {
      printf("Error creating thread: %\n", ret);
      exit(1);
    }
  }

  // joining threads
  for (size_t tx=0; tx < thread_count+1; ++tx) {
    if (ret = pthread_join(threads[tx], NULL)) {
      printf("Error joining thread: %d\n", ret);
      exit(1);
    }
  }

  pthread_mutex_destroy(&mutex_data);
  //pthread_mutex_destroy(&mutex_iter);
  //  pthread_mutex_destroy(&mutex_write)
  
  return 0;
  
}
  
