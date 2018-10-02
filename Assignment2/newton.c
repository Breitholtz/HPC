#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


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
  printf(" args1 %s first %c second %c  \n",args[1],*args[1],  *(args[1]+1));
  long res1;
  if(strncmp(args[1], "-t",2)==0){ // then take the rest of the string and convert to long
    
    char  T[strlen(args[1])-1];
   strncpy(T,args[1]+2,strlen(args[1])-2);
   T[strlen(args[1])-2]='\0';
   res1 = strtol(T,&rest1,10);
   if(strcmp(rest1,"")!=0){
     printf("Junk %s at end of argument 1!\n",rest1);
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
   if(strcmp(rest2,"")!=0){
     printf("Junk %s at end of argument 2!\n",rest2);
   }else{
   printf(" arg2 is %d\n",(int)res2);
   }
  }else{
  printf("Missing -l flag for second argument!!!\n");
 }

 // argument 3    
   long res3 = strtol(args[3],&rest3,10);
   if(strcmp(rest3,"")!=0){
     printf("Junk %s at end of argument 3!\n",rest3);
   }else{
   printf(" arg3 is %d\n",(int)res3);
   }
  


   struct arguments A;
   A.threads=(int)res1;
   A.length=(int)res2;
   A.power=(int)res3;
  //free(args);
   //return;
   return A;  
}



int main(int argc, char * argv[] ){

  int size;
  int threads;
  int power;
  
  // Parse command line arguments
  printf("No. args %d; Arg %s, %s, %s, %s\n",argc, argv[0],argv[1],argv[2],argv[3]);
  struct arguments A;
   A=parse_args(argv);
  printf("-------------------Arguments parsed----------------\n");
  printf("Arguments: 1:%d 2:%d 3:%d\n",A.threads, A.length, A.power);
  return 0;
}
