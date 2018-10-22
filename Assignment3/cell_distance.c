#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
size_t thread_count;
#define max_mem_lines 22300000 //1024^3/(24*2) amount of allocated memory will not be larger than 1GiBi for the two chunks
#define num_points 3 // point amount in a cell
#define max_num_freq 3466

//there is 4 numbers that decide the length(since we output with 2 decimal places)
// we have a maximum distance of sqrt(3)*20<34.65, i.e we may have at most between 00.00 and 34.65 roughly speaking
// and if we take one place per number we get 3466 different places in our vector to increment

unsigned long num_dist[max_num_freq];// global vector to keep track of frequency for distances
float * cellmem;
float * cellmem2;    

void parsefile(){
	unsigned long i,j;
	unsigned short dist;
	
	//char *filename="cell_50";
	//char *filename="cell_e2";
	//char *filename="cell_e3";
	//char *filename="cell_e4"; 
	//char *filename="cell_e5"; 
	//char *filename="cell_test";
	char *filename="cells";
	FILE *fp = fopen(filename,"r"); // open the file to read

	// -------------COUNT AMOUNT OF CELLS IN FILE-----------------
  
	// want to know how large the file; i.e how many cells are we dealing with?
	unsigned long linecount = 0;
	while(!feof(fp)){ //while we are not at the end of the file
		if(getc(fp)=='\n'){
			linecount++; // every newline means new line in the file
		}
	}
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
	  chunks=(long)ceil((linecount/(chunksize*1.0))); 
	}
	rest=linecount%chunksize;    //take the rest in the last chunk

	// chunksizes to use when we add the rest.
	long chunksize_outer;
	long chunksize_inner;

	//--------------ALLOCATING MEMORY-------------
	cellmem2=(float *)malloc(num_points*(chunksize+rest)*sizeof(float));
	float ** cells=(float **)malloc((chunksize+rest)*sizeof(float*));
  
	cellmem=(float *)malloc(num_points*(chunksize+rest)*sizeof(float));
	float ** cells1=(float **)malloc((chunksize+rest)*sizeof(float*));
	for(size_t i,j=0; i<(chunksize+rest);i++,j+=num_points){
		cells1[i]=cellmem+j;
		cells[i]=cellmem2+j;
	}
	//------------ LOOP OVER CHUNKS-------------
	for(size_t ix=0;ix<chunks-1;ix++){

		//------------- SET CURRENT CHUNKSIZE--------------
      
		chunksize_outer=chunksize;
		if(ix==chunks-1){
			chunksize_outer+=rest;
		}
		// -------------READ IN CHUNK 1--------------
		fp = fopen(filename,"r"); // open the file to read

		//jump to correct location in file
		fseek(fp,24*chunksize*ix,SEEK_SET); //chunksize since we don't want to jump too far with chunksize_outer

		for(size_t i=0;i<chunksize_outer;i++){
			// ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
			fscanf(fp, "%f %f %f",&cells1[i][0],&cells1[i][1],&cells1[i][2]); 
			}
		rewind(fp);
		fclose(fp);

		//-------------- COUNT DISTANCES WITHIN CHUNK 1---------------
		if(ix==0){
			#pragma omp parallel for shared(chunksize_outer, cells1) private(dist, i, j) reduction(+:num_dist[:])
			for(i=0;i<chunksize_outer-1;i++){
				for(j=i+1;j<chunksize_outer;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
					  dist=(unsigned short)(sqrtf((cells1[i][0]-cells1[j][0])*(cells1[i][0]-cells1[j][0])+
						(cells1[i][1]-cells1[j][1])*(cells1[i][1]-cells1[j][1])+
									     (cells1[i][2]-cells1[j][2])*(cells1[i][2]-cells1[j][2]))*100);
					num_dist[dist]++; 
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
		  
			//-------------- READ IN CHUNK 2 ----------------
		  
			fp = fopen(filename,"r");
			//jump to correct location in file
			fseek(fp,24*chunksize*jx,SEEK_SET);

			for(size_t i=0;i<chunksize_inner;i++){
				// ok, but we could use a similar parse to how we parse args since we know exactly how the numbers are arranged! Confirmed faster by Martin!
				fscanf(fp, "%f %f %f",&cells[i][0],&cells[i][1],&cells[i][2]);
				}
			rewind(fp);
			fclose(fp);

			//-------------- COUNT DISTANCES WITHIN CHUNK 2---------------

				if(ix==0){ // count all other chunks internal distances the first time we load them
					#pragma omp parallel for shared(chunksize_inner, cells) private(dist, i, j) reduction(+:num_dist[:])
					for(i=0;i<chunksize_inner-1;i++){
						for(j=i+1;j<chunksize_inner;j++){ // j=i+1 so we only calculate each distance once and we also avoid i=j since that is of no use
						  dist=(unsigned short)(sqrtf((cells[i][0]-cells[j][0])*(cells[i][0]-cells[j][0])+
											(cells[i][1]-cells[j][1])*(cells[i][1]-cells[j][1])+
										  (cells[i][2]-cells[j][2])*(cells[i][2]-cells[j][2]))*100);
							num_dist[dist]++;
						}
					}
				}		
		  
		
		  
		  //------- COMPUTATION OF DISTANCE AND FREQUENCY BETWEEN CHUNKS --------------
				#pragma omp parallel for shared(chunksize_outer, chunksize_inner, cells, cells1) private(dist, i, j) reduction(+:num_dist[:])
				for(i=0;i<chunksize_outer;i++){
					for(j=0;j<chunksize_inner;j++){ 
					  dist=(unsigned short)(sqrtf((cells1[i][0]-cells[j][0])*(cells1[i][0]-cells[j][0])+
										(cells1[i][1]-cells[j][1])*(cells1[i][1]-cells[j][1])+
										 (cells1[i][2]-cells[j][2])*(cells1[i][2]-cells[j][2]))*100);
						num_dist[dist]++; 
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
		
}

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
