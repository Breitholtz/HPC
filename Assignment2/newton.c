#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
//#include <acb_poly.h>   //   http://arblib.org/acb_poly.html#acb-poly
#include <math.h>

struct arguments{
  int threads;
  int length;
  int power;

};

//struct arguments
struct arguments parse_args(char * args[]){
  
  char * rest1;
  char * rest2;
  char * rest3;



  // argument 1
  //  printf(" args1 %s first %c second %c  \n",args[1],*args[1],  *(args[1]+1));
  long res1;
  if(strncmp(args[1], "-t",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[1])-1];
   strncpy(T,args[1]+2,strlen(args[1])-2);
   T[strlen(args[1])-2]='\0';
   res1 = strtol(T,&rest1,10);
   if(strcmp(rest1,"")!=0 || res1<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 1!\n",res1,rest1);
   }else{
   printf(" arg1 is %d\n",(int)res1);
   }
  }else{
  printf("Missing -t flag for first argument!!!\n");
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
   }else{
   printf(" arg2 is %d\n",(int)res2);
   }
  }else{
  printf("Missing -l flag for second argument!!!\n");
 }

 // argument 3    
   long res3 = strtol(args[3],&rest3,10);
   if(strcmp(rest3,"")!=0 || res3<1){
     printf("Invalid argument '%ld' or Junk '%s' at end of argument 3!\n",res3,rest3);
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

void threaded_newton(int threads, int d, int size){


  
  // plan: break up square (-2,2)^2 into size intervals for x and y using cos(2pi*(n/d)) and sin(2pi*(n/d)), where n=0,...,d-1 ; compute which roots the function has with some library function;
  // use newtons to approximate the root to 10^-3 precision (in absolute value)?; if divergent or very close to origin say that it converged to 0; note which root and how many iterations was necessary;
  // pass info back as pointer(s)?
  // actual iterative formula
  //
  // x_(k+1)=x_k-f(x_k)/f'(x_k), where f=x^d-1
  //
  

  return;
}

int main(int argc, char * argv[] ){
  // Grupp: hpcgp017
  
  int size;
  int threads;
  int power;
  
  // Parse command line arguments
  printf("No. args %d; Arg %s, %s, %s, %s\n",argc, argv[0],argv[1],argv[2],argv[3]);
  struct arguments A;
   A=parse_args(argv);
  printf("-------------------Arguments parsed----------------\n");
  printf("Arguments: 1:%d 2:%d 3:%d\n",A.threads, A.length, A.power); // remove later
  A.length=size;
  A.threads=threads;
  A.power=power;

  int * coeffs[power+1];
    //acb_poly_t poly;
    //poly.coeffs=coeffs;
  threaded_newton(threads, power, size); // return type? *double (for which roots we conv to) and *int (for number of iterations) maybe?
  // pass pointer to a row to thread? pass array of the roots of the function?

  // write the information into the desired .ppm format and output the file, call the one with colours corresponding to roots newton_attractor_xd.ppm and the other newton_convergence_xd.ppm
  // where d in ..xd.pmm is the power of x

  return 0;
}
