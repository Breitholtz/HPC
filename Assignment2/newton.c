
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "acb_poly.h"  //   http://arblib.org/acb_poly.html#acb-poly
#include <math.h>      // void arb_fmpz_poly_complex_roots(acb_ptr roots, const fmpz_poly_t poly, int flags, slong prec)
#include "acb.h"
#include "arb_fmpz_poly.h"
struct arguments{
  int threads;
  int length;
  int power;

};

//struct arguments is what we use to pass out multiple returnvalues
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
 


  // plan: break up square (-2,2)^2 into size intervals for x and y using cos(2pi*(n/d)) and sin(2pi*(n/d)), where n=0,...,d-1 ; compute which roots the function has with some library function;
  // use newtons to approximate the root to 10^-3 precision (in absolute value)?; if divergent or very close to origin say that it converged to 0; note which root and how many iterations was necessary;
  // pass info back as pointer(s)?
  // actual iterative formula
  //
  // x_(k+1)=x_k-f(x_k)/f'(x_k), where f=x^d-1, f'=d*x^(d-1)
  //
  
  // double sin(double x) is math lib function syntax
  return NULL;
}

void * writeppm(void * args) { // void * since we want it to work with threads
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
  int numrows;
  int numrest;
  
  // calculating amount of rows that will be available to each thread
  if(size%thread_count==0){
     numrows = size/thread_count;
  }else{
     numrest=size%thread_count;
     numrows=size/thread_count;
  }

int roots[size*size];
int iterations[size*size]; // ok or malloc here?
int ret;
pthread_t threads[thread_count];
pthread_mutex_t mutex_root;
pthread_mutex_t mutex_iter;

pthread_mutex_init(&mutex_root, NULL); // mutex for accessing the root array
pthread_mutex_init(&mutex_iter,NULL); //  mutex for accessing the iteration array

// do first thread with the extra rows that may exist if #rows is not divisible by #threads
  int ** arg = malloc(3*sizeof(int*));
  arg[0] = roots;
  arg[1] = iterations;
  arg[2] = &power;
  //  arg[3] = roots_exact;
  if (ret = pthread_create(threads, NULL, threaded_newton, (void*)arg)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }

// do the rest of the threads
  for (size_t tx=1, ix=(numrows+numrest); tx < thread_count; ++tx, ix+=numrows){ 
  int ** arg = malloc(3*sizeof(int*));
  arg[0] = roots+ix;
  arg[1] = iterations+ix;
  arg[2] = &power;
  //  arg[3] = roots_exact;
  if (ret = pthread_create(threads+tx, NULL, threaded_newton, (void*)arg)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
  }

  // joining threads
  for (size_t tx=0; tx < thread_count; ++tx) {
    if (ret = pthread_join(threads[tx], NULL)) {
      printf("Error joining thread: %d\n", ret);
      exit(1);
    }
  }
  pthread_mutex_destroy(&mutex_root);
  pthread_mutex_destroy(&mutex_iter);
  
   //  threaded_newton(power, size); // return type? None, and just change memory in the function and return nothing. pass array of the roots of the function so we don't compute roots for every thread?

  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in ..xd.pmm is the power of x

  // TODO: factor out into function ppmwrite so it may be threaded, also make better assembly of filename

  const char *s1="newton_convergence_x";
  const char *s3=".ppm";
  
  char s2[12];
  sprintf(s2, "%d", power);
 char * result = malloc(strlen(s1)+strlen(s2)+1);
  strcpy(result, s1);  // memory
  strcat(result, s2);
  char * filename = malloc(strlen(result)+strlen(s3)+1);
  strcpy(filename, result); //memory
  strcat(filename, s3);
  printf("final string: %s\n",filename);
  FILE * fp  =fopen(filename,"w");
  fprintf(fp,"P3\n%d %d\n255\n",size,size);
  //fwrite
  fclose(fp);
  free(result);
  free(filename);
  
  return 0;
  
}
