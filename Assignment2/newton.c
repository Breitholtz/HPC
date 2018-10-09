#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>      

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
  //    struct newton_arguments *arg = args;
	
    //float ** initial = initial; //initial value pointer  
    //int ** iterations = iterations; // iteration pointer
	//*arg->power; //polynomial degree
	//int n_loc = arg->num_rows; //numrows_first ; how many rows this thread is handling
	//	float ** root_exact = roots_exact;
	//*arg->rowsize; // how long each row is
	//int rownumber = arg->index; // index of the first row in the memory block so we know where to put it after computation
	//	int * rows_done = rows_done;  // pointer to keep track of calculated roots
	//int ** result = arg->result;
        
	int * I=(int *)args;
	Index=*I;
        
	float z_re;
	float z_im;
	float arg_z;
	float abs_z;
	float abs_d;
	int True;

	int iterations_loc;
	int which_root = POWER;
	
	
    while(Index < SIZE){
      printf("index: %d\n",Index);
	for(int jx = 0; jx < SIZE; jx++){ 
		True = 0;
		iterations_loc=0; //keep track of iterations
		//which_root=0;
		printf("Initial %f %f\n",initial[Index][2*jx],initial[Index][2*jx+1]);
		z_re=initial[Index][2*jx];
		z_im=initial[Index][2*jx+1];
		//Newtons method
		//printf("index: %d\n",Index);
		while((True==0) && ( abs(z_re) < MAX) && (abs(z_im) < MAX)){
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
			//printf("iteration continues\n");
			iterations_loc++;
		}
	       
		//  save result
		result[Index][jx]=which_root;
		iterations[Index][jx] = iterations_loc;

	}
 	    pthread_mutex_lock(&mutex_write); 
		rows_done[Index] = 1;
		Index++;
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

  	
  struct write_arguments *arguments =args;
  int * power= arguments->power;
  int * size = arguments->size;
  int * rows_done=arguments->rows_done;
  int ** iterations= arguments->iterations;
  int ** result = arguments->result;
  
  const int SIZE =*size;
  int rows_written[SIZE]; // initialise??? 
   
   
   //necessary?
   u_char * iterations_mat=(u_char *)malloc(SIZE*SIZE*3);
   u_char * result_mat= (u_char *)malloc(SIZE*SIZE*3);
   u_char ** result_loc=(u_char **)malloc(SIZE*sizeof(u_char *));
   u_char ** iterations_loc=(u_char **)malloc(SIZE*sizeof(u_char *));
	
	//results and rows_done should be global according to 
	char * rows_done_loc = (char*)calloc(SIZE, sizeof(char)); 
	
	int ix=0;
	
	// create filenames
	char str[26];
	char str2[26];
	sprintf(str, "newton_attractors_x%i.ppm", *power);
	sprintf(str2, "newton_convergence_x%i.ppm", *power);
	FILE * fp;
	FILE * fp2;
	
	while(ix < size){
		if(row_done[ix] != 0){
			
			//write to file
			
			xi++
		}
	}
   
   
   /* for(size_t ix=0, jx=0; ix<SIZE;ix++, jx+=SIZE){
     result_loc[ix]=result_mat + 3*jx;
     iterations_loc[ix]=iterations_mat + 3*jx;
   }
   int current_index;
   printf("----------Starting looping in writeppm------------\n");
   while(sum_array<SIZE){
    sum_array=0;
    // pause to let threads finish rows?
    //pthread_mutex_lock(&mutex_write);
    for(size_t index=0;index<SIZE;index++){
      sum_array+=rows_written[index];
      if(rows_done[index] && !rows_written[index]){
	// if found entry:  note that it is done write that row to file
	rows_written[index]=1;
	current_index=index;
	break;
      }
    }    
     //pthread_mutex_unlock(&mutex_write);
  	 if(sum_array<1){
	   continue;
	 }
	 if(sum_array==SIZE){
	   break;
	   }  
               
     // fill the data array, Mutex necessary???
	 //printf("result; current_index: %d; rows_done %d; result: %d\n",current_index,rows_done[current_index],result[current_index][0]);
	  //pthread_mutex_lock(&mutex_data);
     for (size_t y = 0; y < SIZE; y++) { // what do we do here?
       for (size_t x=0;x<3;x++){
	 // printf("result[current_index][y]:%d\n",result[current_index][y]);
	 result_loc[current_index][3*y+x] = (u_char) result[current_index][y];
	 iterations_loc[current_index][3*y+x] = (u_char) iterations[current_index][y]; // bitwise operation & ok????
       }
     }    
       //pthread_mutex_unlock(&mutex_data);
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
      colors[]
      grayscale[]
      //if row j done
      for( size_t i=0; i<SIZE;i++){
      string=concat(string,colors[j][i])
}
fwrite(string)
     fwrite(result_mat, 1,SIZE*SIZE*3, fp); // works??
     fclose(fp);

     
     fp2 = fopen(str2, "w");
     fprintf(fp2,"P3\n%d %d\n%d\n",size,size, 255); // header

     //write image data bytes to the file 
     fwrite(iterations_mat, 1,SIZE*SIZE*3, fp2); // works??
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

    
  parse_args(argv); 
  printf("Arguments: 1:%d 2:%d 3:%d\n",THREADS, SIZE, POWER); // remove later

  

   
  // calculating amount of rows that will be available to each thread
   int numrest=SIZE%THREADS;
   int numrows=SIZE/THREADS;


   float * initial_mat =(float *)malloc(2*sizeof(float*)*SIZE*SIZE);
   initial = (float**) malloc(2*sizeof(float*)*SIZE); // initial values to newton
   int * iterations_mat= (int *)malloc(sizeof(int*)*SIZE);
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
 

    printf("-----------Start writing thread----------\n");
// do writing thread
    
    if (ret = pthread_create(threads+THREADS, NULL, writeppm, NULL)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
    //    printf("power: %d",power);
 printf("-----------Start Newton threads----------\n");
 
	for(int tx = 0; tx < THREADS; tx++){
	  Index = tx;
	  printf("address %d", &index);
		if (ret = pthread_create(threads + tx, NULL, threaded_newton, (void*)&Index)) {
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
   pthread_join(threads[THREADS],NULL);
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
  
  return 0;  
}
