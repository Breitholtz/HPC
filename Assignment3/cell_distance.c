#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
size_t thread_count;
#define num_points 3 // point amount in a cell
#define max_cells 1024 // set to a random number for now, what should this be?
#define max_num_freq 3466 //there is 4 numbers that decide the length(since we output with 2 decimal places)
// we have a maximum distance of sqrt(3)*20<34.66, i.e we may have at most between 00.00 and 34.66 roughly speaking
// and if we take one place per number we get 3466 different places in our vector to increment

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



  float cells[linecount][num_points]; //ok? This is the array we use to store the cells coordinates
  
  unsigned short dist[linecount][linecount]; // we store the distances as short integers as we may use the trick described above at max_num_freq

    /*
      Note: we are be able to just parse the coordinates as short integers aswell since we only need to scale by a factor and cast to get the necessary precision
      although the cast when we need to do the sqrt might be expensive; although I do not know how expensive...
    */
  
  //#pragma omp parallel for shared(fp,linecount,cells) schedule(static, linecount/thread_count) //
      for(size_t i=0;i<linecount;i++){
	fscanf(fp, "%f %f %f",&cells[i][0],&cells[i][1],&cells[i][2]); // ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
	for(size_t j=0;j<3;j++){
	  printf("cells: %f\n",cells[i][j]); // check correctness
	  }
    }
      fclose(fp); // close the file


      //    
      //    do the computation of the distance and frequency
      //
      
 unsigned int *num_dist=(unsigned int *)calloc(max_num_freq,sizeof(int)); // vector to keep track of frequency
 
#pragma omp parallel shared(linecount, cells) // parse the file in parallel; variables in the shared() is shared between threads
 {
  unsigned long i,j;

#pragma omp parallel for schedule(static,5) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
    for(i=0;i<linecount;i++){
      for(j=i+1;j<linecount;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
#pragma omp critical
	{
	dist[i][j]=(unsigned short)roundf(sqrtf(((cells[i][0]-cells[j][0])*(cells[i][0]-cells[j][0])+
				   (cells[i][1]-cells[j][1])*(cells[i][1]-cells[j][1])+
				   (cells[i][2]-cells[j][2])*(cells[i][2]-cells[j][2])))*100);
	  num_dist[dist[i][j]]++;
	}
	  printf("dist %hu\n", dist[i][j]);
      }
    }
}


 // OPTIONAL THINGS TO IMPLEMENT: having a validation for the test data we have, the parsing as short integers

 //
 // test correctness by writing to stdout and comparing to cell_validate
 //

  
 /*  PSEUDOCODE
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
    /*
Observations:
      - there is 24 characters in every row; for each number, 2 spaces and a newline, i.e one cell =24 bytes in characters
      - we can assume that there is less than 2^32 cells; so since we may use at most 1024^3 bytes at any one time we can use 1024^3/(2^32) =1/4 bytes per cell
        however, since every row is 24 bytes we will have to process at most 1/96th of the maximum of all cells at any one time to have enough bytes to work with
	, i.e we may have at most 2^32/96 cells processing at any one time. This is about 44 million.

      - ignoring the above and just doing the trick described in the start of the document, parsing the points as short integers and then just cast when we want to do a square root.
     */

int main(int argc, char * argv[]){
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

     omp_set_num_threads(thread_count);  //set number of threads
   
    parsefile();  // parse file and calculate distances
    
    printf("Result: thread count= %d\n",(int)thread_count);

  return 0;
}
