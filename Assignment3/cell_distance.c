#include<stdio.h>
#include<stdlib.h>


/*
suggestions for functions that we might write:
- a parser for the input file 
- function that computes distances between points, perhaps store as char* and cast when we want to compute lengths?
 This since char only uses 1 byte per character and float is 4 bytes and a double is 8 bytes so we must convert the ascii of the char to something small to get anything 
  that is better than just storing as a float
 */
int main(int argc, char * argv[]){

  int thread_count=0;
  // parse command line arguments
  if (argc != 2) {
    printf("The syntax for this function is '%s -t(amount of threads)'\n",argv[0]);
    return 1;
  }
    if (argv[1][0] == '-') {
      if (argv[1][1] == 't') {
	thread_count = strtol(&argv[1][2],NULL,10);
      }else{
	printf("Incorrect flag! Wanted: 't' Given: '%c'\n",argv[1][1]);
	return 1;
      }
    }else{
      printf("Incorrect flag prefix! Wanted: '-' Given: '%c'\n",argv[1][0]);
      return 1;
    }

  // parse file


  // test correctness by writing to stdout

  printf("Result: thread count= %d\n",(int)thread_count);
 
  
  return 0;
}
