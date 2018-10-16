#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
size_t thread_count;
#define num_points 3 // point amount in a cell
#define max_cells 1024 // set to a random number for now, what should this be?
#define max_num_freq 3500 //there is 4 numbers that decide the length(since we output with 2 decimal places)
// we have a maximum distance of sqrt(3)*20<35, i.e we may have at most between 00.00 and 35.00 roughly speaking
// and if we take one place per number we get 3500 different places in our vector to increment

void parsefile(){
  
  //  char *filename="cell_e5";
    char *filename="cell_test";
  FILE *fp = fopen(filename,"r"); // open the file to read

  // want to know how large the file; i.e how many cells are we dealing with?
  unsigned long linecount = 0;
  while(!feof(fp)){ //while we are not at the end of the file
    if(getc(fp)=='\n'){
      linecount++; // every newline means new line in the file
    }
  }
  printf("lines in file: %lu\n",linecount);
  rewind(fp); // go back to beginning of file
  float cells[linecount][num_points]; //ok?
  // parse the cells into memory
  float dist[linecount][linecount];
#pragma omp parallel shared(linecount, cells) // parse the file in parallel; variables in the shared() is shared between threads
  {
    float result[3][4];
    for(size_t i=0;i<linecount;i++){
      fscanf(fp, "%f %f %f",result[0],result[1],result[2]);
      for(size_t j=0;j<3;j++){
	cells[i][j]=*result[j];
	printf("cells: %f\n",cells[i][j]);
      }
    }
  }

  //do the computation of the distance and frequency
  
  unsigned int *num_dist=(unsigned int *)calloc(max_num_freq,sizeof(int)); // vector to keep track of frequency
  unsigned long i,j;
 //#pragma omp parallel shared(cells,i,j)
  float distx;
  float disty;
  float distz;
  for(i=0;i<linecount;i++){
    for(j=i;j<linecount;j++){ // j=i so we only calculate each distance once
      distx=(cells[i][0]+cells[j][0])*(cells[i][0]+cells[j][0]);
      disty=(cells[i][1]+cells[j][1])*(cells[i][1]+cells[j][1]);
      distz=(cells[i][2]+cells[j][2])*(cells[i][2]+cells[j][2]);
      float res = (distx+disty+distz);
      printf("Result squared %f\n", res);
      //      dist[i][j]=(float)sqrt(res);//(distx+disty+distz));
      //int freq=(int) 100*dist[i][j]; //to get the correct index for the length
      //num_dist[freq]++;
    }
  }
    // don't use sqrt for square root?

  // test correctness by writing to stdout and comparing to cell_validate

    /* can probably be in here
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

  
}

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

    //set number of threads
    omp_set_num_threads(thread_count);
  // parse file
    /*
      - there is 24 characters in every row; 7 for each number, 2 spaces and a newline, i.e one cell =24 bytes 
      - we can assume that there is less than 2^32 cells; so since we may use at most 1024^3 bytes at any one time we can use 1024^3/(2^32) =1/4 bytes per cell
        however, since every row is 24 bytes we will have to process at most 1/96th of the maximum of all cells at any one time to have enough bytes to work with
	, i.e we may have at most 2^32/96 cell processing at any one time. This is about 44 million.
     */
    parsefile();
    
    // memory allocation ?
    // take in only certain bit of the file?



    // calculate distances and frequencies
    
    // what happens when we have to reload in another batch of points if the amount of points is larger than what we can accommodate?
    // improve memory locality?

    
    printf("Result: thread count= %d\n",(int)thread_count);

  return 0;
}
