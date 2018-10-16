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
    /*
      - there is 24 characters in every row; 7 for each number, 2 spaces and a newline, i.e one cell =24 bytes 
      - we can assume that there is less than 2^32 cells; so since we may use at most 1024^3 bytes at any one time we can use 1024^3/(2^32) =1/4 bytes per cell
        however, since every row is 24 bytes we will have to process at most 1/96th of the maximum of all cells at any one time to have enough bytes to work with
	, i.e we may have at most 2^32/96 cell processing at any one time. This is about 44 million.
     */
    // memory allocation ?
    // take in only certain bit of the file?



    // calculate distances and frequencies
    
    // what happens when we have to reload in another batch of points if the amount of points is larger than what we can accommodate?
    // improve memory locality?

  // test correctness by writing to stdout and comparing to cell_validate
    /*
int numcorrect=0;
for(size_t i=1;i<amount_cells;i+=2'){ // check both distance and frequency
if(result(i)==validation(i)&&result(i+1)==validation(i+1)){
printf("Result: %s %c==%s %c '%s'\n",result(i),result(i+1),validation(i),validation(i+1),"CORRECT!");
numcorrect++;
}else{
printf("Result: %s %c==%s %c '%s'\n",result(i),result(i+1),validation(i),validation(i+1),"INCORRECT!");
}
} 
printf("Correctness %d/%d",numcorrect,amount_cells);
    */
    printf("Result: thread count= %d\n",(int)thread_count);

  return 0;
}
