#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
size_t thread_count;
#define max_mem_lines 22300000 //1024^3/(24*2) amount of allocated memory will not be larger than 1GiBi for the two chunks
#define num_points 3 // point amount in a cell
#define max_num_freq 3466 //there is 4 numbers that decide the length(since we output with 2 decimal places)
// we have a maximum distance of sqrt(3)*20<34.66, i.e we may have at most between 00.00 and 34.66 roughly speaking
// and if we take one place per number we get 3466 different places in our vector to increment

unsigned short num_dist[max_num_freq];// global vector to keep track of frequency for distances
float * cellmem;
float * cellmem2;    

/*
  Note: we are be able to just parse the coordinates as short integers aswell since we only need to scale by a factor and cast to get the necessary precision
  although the cast when we need to do the sqrt might be expensive; although I do not know how expensive...
*/

void parsefile(){
  //char *filename="cell_50";
  //char *filename="cell_e2";
  //char *filename="cell_e3";
  //char *filename="cell_e4"; 
  char *filename="cell_e5"; 
  // char *filename="cell_test";
  FILE *fp = fopen(filename,"r"); // open the file to read

  // -------------COUNT AMOUNT OF CELLS IN FILE-----------------
  
  // want to know how large the file; i.e how many cells are we dealing with?
  unsigned long linecount = 0;
  while(!feof(fp)){ //while we are not at the end of the file
    if(getc(fp)=='\n'){
      linecount++; // every newline means new line in the file
    }
  }
  printf("lines in file: %lu\n",linecount);
  fclose(fp);

  // Now, we want to parse as many cells as possible but not use more than 1GiB memory at any one time, so since we now know how many lines there are to parse we want to split them up
  // into manageable chunks and pairwise compare them to calculate distances. If we do this for a certain chunksize we can guarantee that we are not breaking the memory requirement
  
  long chunksize;
  if(linecount>max_mem_lines){// see if we will be above our max memory
    chunksize=max_mem_lines/2; // chosen so that we meet memory requirements
  }else{
    chunksize=linecount/2; // choose as large as we can 
  }
  long rest;
  long chunks;
  if(chunksize>linecount){
    chunksize=linecount;
    chunks=1;
  }else{
    chunks=(long)ceil((double)(linecount/chunksize)); // should be at least 96*2=192 to ensure correct memory usage
  }
  rest=linecount%chunksize;    //take the rest in a separate chunk
  
  // chunksizes to use when we add the rest.
  long chunksize_outer;
  long chunksize_inner;

  //--------------ALLOCATING MEMORY-------------
  // can we be clever and get rid of the rest in here somehow?
  cellmem2=(float *)malloc(num_points*(chunksize+rest)*sizeof(float));
  float ** cells=(float **)malloc((chunksize+rest)*sizeof(float*));
  
  cellmem=(float *)malloc(num_points*(chunksize+rest)*sizeof(float));
  float ** cells1=(float **)malloc((chunksize+rest)*sizeof(float*));
  for(size_t i,j=0; i<(chunksize+rest);i++,j+=num_points){
    cells1[i]=cellmem+j;
    cells[i]=cellmem2+j;
  }
  //------------ LOOP OVER CHUNKS-------------
  for(size_t ix=0;ix<chunks;ix++){

    //------------- SET CURRENT CHUNKSIZE--------------
      
    chunksize_outer=chunksize;
    if(ix==chunks-1){
      chunksize_outer+=rest;
    }
    // -------------READ IN CHUNK 1--------------
    fp = fopen(filename,"r"); // open the file to read

    //jump to correct location in file
    fseek(fp,24*chunksize*ix,SEEK_SET); //chunksize since we don't want to jump too far with chunksize_outer
    //   float cells1[chunksize_outer][num_points]; // stack allocated cells
    // parallellize?
    for(size_t i=0;i<chunksize_outer;i++){
      // ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
      fscanf(fp, "%f %f %f",&cells1[i][0],&cells1[i][1],&cells1[i][2]); 
      for(size_t j=0;j<num_points;j++){
	//	printf("CHUNK1 cells: %f\n",cells1[i][j]); // check correctness
      }
    }
    rewind(fp);
    fclose(fp);

    //-------------- COUNT DISTANCES WITHIN CHUNK 1---------------
    if(ix==0){
    {
      unsigned long i,j;
      unsigned short dist;
      //       printf("chunksize %d\n",chunksize_outer);
      //#pragma omp for schedule(static,chunksize/2) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
      for(i=0;i<chunksize_outer;i++){
	for(j=i+1;j<chunksize_outer;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
	  //#pragma omp critical
	  {
	    // roundf to ensure correct last digit
	    dist=(unsigned short)(sqrtf((cells1[i][0]-cells1[j][0])*(cells1[i][0]-cells1[j][0])+
					(cells1[i][1]-cells1[j][1])*(cells1[i][1]-cells1[j][1])+
					(cells1[i][2]-cells1[j][2])*(cells1[i][2]-cells1[j][2]))*100);
	    num_dist[dist]++;
	    
	  }
	  //printf("INTERNAL CHUNK 1 dist %hu\n", dist);
	}
      }
    }
    }

    
      
    //------------ LOOP OVER CHUNKS-------------  
    for(size_t jx=ix+1;jx<chunks;jx++){ // ix+1 to make the loop go over the chunks correctly, i.e not double up on computations


      //------------- SET CURRENT CHUNKSIZE--------------
      chunksize_inner=chunksize;
      if(jx==chunks-1){
	chunksize_inner+=rest;
      }
      //printf("chunksize %d\n",chunksize_inner);
      
      //-------------- READ IN CHUNK 2 ----------------
      
      fp = fopen(filename,"r"); // open the file to read

      //jump to correct location in file
      fseek(fp,24*chunksize*jx,SEEK_SET);
      
      //     float cells[chunksize_inner][num_points]; // stack allocated cells
      // parallellize?
      for(size_t i=0;i<chunksize_inner;i++){
	// ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
	fscanf(fp, "%f %f %f",&cells[i][0],&cells[i][1],&cells[i][2]); 
	for(size_t j=0;j<num_points;j++){
	  //printf("CHUNK2 cells: %f\n",cells[i][j]); // check correctness
	}
      }
      rewind(fp);
      fclose(fp);

      //-------------- COUNT DISTANCES WITHIN CHUNK 2---------------

      if(ix==0){ // count all other chunks internal distances first  
        {
	  unsigned long i,j;
	  unsigned short dist;
	  //#pragma omp for schedule(static,chunksize/2) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
	  for(i=0;i<chunksize_inner;i++){
	    for(j=i+1;j<chunksize_inner;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
	      //#pragma omp critical
	      {
		// roundf to ensure correct last digit
		dist=(unsigned short)(sqrtf((cells[i][0]-cells[j][0])*(cells[i][0]-cells[j][0])+
						  (cells[i][1]-cells[j][1])*(cells[i][1]-cells[j][1])+
					    (cells[i][2]-cells[j][2])*(cells[i][2]-cells[j][2]))*100);
		num_dist[dist]++;
	      }
	      //      printf("INTERNAL CHUNK 2 dist %hu\n", dist);
	    }
	  }
	}
      }
      
      
      
      //------- COMPUTATION OF DISTANCE AND FREQUENCY BETWEEN CHUNKS --------------
     
      //#pragma omp parallel shared(linecount, cells) // parse the file in parallel; variables in the shared() is shared between threads
      {
	unsigned long i,j;
	unsigned short dist;
	//#pragma omp for schedule(static,chunksize/2) // how do we choose the chunk size that we schedule?? some fraction of the whole. But what do we do for huge linecounts???
	for(i=0;i<chunksize_outer;i++){
	  for(j=0;j<chunksize_inner;j++){ 
	    //#pragma omp critical
	    {
	      // roundf to ensure correct last digit
	      dist=(unsigned short)(sqrtf((cells1[i][0]-cells[j][0])*(cells1[i][0]-cells[j][0])+
						(cells1[i][1]-cells[j][1])*(cells1[i][1]-cells[j][1])+
					  (cells1[i][2]-cells[j][2])*(cells1[i][2]-cells[j][2]))*100);
	      num_dist[dist]++;
	    }
	    // printf("BETWEEN CHUNKS dist %hu\n", dist);
	  }
	}
      }

    } // end of inner chunk loop here
  }   // end of outer chunk loop here

//-----------FREE MEMORY----------
  free(cells);
  free(cells1);
  free(cellmem);
  free(cellmem2);
  
  // WRITE DISTANCES WITH NONZERO FREQUENCIES
  for(unsigned short i=0;i<max_num_freq;i++){
    if(num_dist[i]!=0){
      if(i/100.0<10.0){
	printf("0%.2f %d\n",i/100.0,num_dist[i]); // write in desired format
      }else{
      printf("%.2f %d\n",i/100.0,num_dist[i]); // write in desired format
    }
  }
  }
  // OPTIONAL THINGS TO IMPLEMENT: having a validation for the test data we have, do the parsing as short int
  
  /*
     // read in validation file
        //-------------- READ IN VALIDATION ----------------
  char * filename_val="cell_validate";
  FILE * fp2 = fopen(filename_val,"r"); // open the file to read 
  
  size_t linecount_val=max_num_freq;
  // redo as mallocs if they work
  float dists_val[linecount_val];
  int freqs_val[linecount_val];
  for(size_t i=0;i<linecount_val;i++){
    if(!feof(fp2)){
      fscanf(fp2, "%f %d",&dists_val[i],&freqs_val[i]);
      }
  }
  fclose(fp2);

  int numcorrect=0;
  for(size_t i=1;i<linecount_val;i++){ // check both distance and frequency
    if(num_dist[i]!=0){
      if((num_dist[i]-(unsigned int)freqs_val[i])==0){
	//printf("Result: %hu %ld==%d %d '%s'\n",num_dist[i],i,freqs_val[i],(int)(100*dists_val[i]),"CORRECT!");
	numcorrect++;
      }else{
	//       printf("Result: %hu %ld==%d %d '%s'\n",num_dist[i],i,freqs_val[i],(int)(100*dists_val[i]),"INCORRECT!");
      }
    }
  }


   printf("Correctness %d/%ld\n",numcorrect,linecount_val);
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

  return 0;
}
