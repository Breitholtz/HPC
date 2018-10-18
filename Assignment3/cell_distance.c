#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
size_t thread_count;
#define num_points 3 // point amount in a cell
//#define max_cells 10000000 // set to ten million for now, what should this be?
#define max_num_freq 3466 //there is 4 numbers that decide the length(since we output with 2 decimal places)
// we have a maximum distance of sqrt(3)*20<34.66, i.e we may have at most between 00.00 and 34.66 roughly speaking
// and if we take one place per number we get 3466 different places in our vector to increment

unsigned int num_dist[max_num_freq];// global vector to keep track of frequency for distances
    
/*
  Note: we are be able to just parse the coordinates as short integers aswell since we only need to scale by a factor and cast to get the necessary precision
  although the cast when we need to do the sqrt might be expensive; although I do not know how expensive...
*/

void parsefile(){
  
  //char *filename="cell_e5";
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
  fclose(fp);

  // Now, we want to parse as many cells as possible but not use more than 1GiB memory at any one time, so since we now know how many lines there are to parse we want to split them up
  // into manageable chunks and pairwise compare them to calculate distances. If we do this for a certain chunksize we can guarantee that we are not breaking the memory requirement
  float chunksize=5.0;//100.0; // chosen so that we meet memory requirements
  
  size_t rest=0;
  int chunks=ciel(linecount/chunksize);
  if(linecount%chunksize!=0) rest=linecount%chunksize;    //take the rest in a separate chunk
  int chunksize_loc1;
  int chunksize_loc;
  
  for(size_t ix=0;ix<chunks;ix++){

    // take chunk ix


    chunksize_loc1=chunksize;
      if(ix==chunks-1){
       chunksize_loc1+=rest;
      }
      // READ IN CHUNK 1
      fp = fopen(filename,"r"); // open the file to read

      //jump to correct location in file
      fseek(fp,24*chunksize*ix,SEEK_SET);
      float cells1[chunksize_loc1][num_points]; // stack allocated cells
      // parallellize?
      for(size_t i=0;i<chunksize_loc1;i++){
	// ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
	fscanf(fp, "%f %f %f",&cells1[i][0],&cells1[i][1],&cells1[i][2]); 
	for(size_t j=0;j<3;j++){
	  printf("cells: %f\n",cells1[i][j]); // check correctness
	  }
      }
      fclose(fp);
      
      if(ix==0&&jx==0){
	//#pragma omp for schedule(static,chunksize/2) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
    for(size_t i=1;i<chunksize_loc1;i++){
      for(size_t j=i+1;j<chunksize_loc1;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
	//#pragma omp critical
	{
	  dist=(unsigned short)roundf(sqrtf((cells1[i][0]-cells1[j][0])*(cells1[i][0]-cells1[j][0])+
				   (cells1[i][1]-cells1[j][1])*(cells1[i][1]-cells1[j][1])+
					    (cells1[i][2]-cells1[j][2])*(cells1[i][2]-cells1[j][2])))*100;
	num_dist[dist]++;
	}
	  printf("dist %hu\n", dist);
      }
    }
      }
  
    for(size_t jx=0;jx<chunks;jx++){
      // take chunk jx
  
      chunksize_loc=chunksize;
      if(jx==chunks-1){
       chunksize_loc+=rest;
      }

      // READ IN CHUNK 2
      fp = fopen(filename,"r"); // open the file to read

      //jump to correct location in file
      fseek(fp,24*chunksize*ix,SEEK_SET);
  
      float cells[chunksize_loc][num_points]; // stack allocated cells
      // parallellize?
      for(size_t i=0;i<chunksize_loc;i++){
	// ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
	fscanf(fp, "%f %f %f",&cells[i][0],&cells[i][1],&cells[i][2]); 
	for(size_t j=0;j<3;j++){
	  printf("cells: %f\n",cells[i][j]); // check correctness
	  }
    }

      //    
      //    COMPUTATION OF DISTANCE AND FREQUENCY BETWEEN CHUNKS
      //
     
      //#pragma omp parallel shared(linecount, cells) // parse the file in parallel; variables in the shared() is shared between threads
 {
  unsigned long i,j;
  unsigned short dist;
  //#pragma omp for schedule(static,chunksize/2) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
    for(i=1;i<chunksize_loc1;i++){
      for(j=1;j<chunksize_loc;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
	//#pragma omp critical
	{
	  dist=(unsigned short)roundf(sqrtf((cells1[i][0]-cells[j][0])*(cells1[i][0]-cells[j][0])+
				   (cells1[i][1]-cells[j][1])*(cells1[i][1]-cells[j][1])+
					     (cells1[i][2]-cells[j][2])*(cells1[i][2]-cells[j][2]))*100);
	num_dist[dist]++;
	}
	  printf("dist %hu\n", dist);
      }
    }
 }

    } // end of inner chunk loop here
  }

  fclose(fp); // close the file

 

 
  // WRITE DISTANCES WITH NONZERO FREQUENCIES
  for(unsigned short i=0;i<max_num_freq;i++){
    if(num_dist[i]!=0){
      printf("%.2f %d\n",i/100.0,num_dist[i]); // write in desired format
    }
 }
 // OPTIONAL THINGS TO IMPLEMENT: having a validation for the test data we have, do the parsing as short integers

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
