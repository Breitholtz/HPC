# Assignment 3: OpenMP
In this assignment we have implemented a program that parses files with point coordinates and computes the distance between them and counts the frequency of each distance with two decimals precision. The program's  output will be the calculated distances and how many of a distance there is.

## Main
In the main function we parse the command line argument that gives us the number of threads, with some error checking, and set it to be the number of threads that OpenMP may use. We then run the function **parsefile** which both parses and computes distances.

## parsefile
This is the function which does the computation and parsing of the coordinates.

The function works by reading the datafile's lines in chunks. The size of the chunks is decided by how much memory is allowed to be used in the calculations and the number of chunks is decided by the size of the datafile. The computation of the distance is then done chunkwise. To further explain how the computation is done the function will be explained in more detail below.

The function starts by counting the amount of lines in the given datafile. The number of lines is the compared to a variable called max_mem_lines. This variable is set to be the maximum amount of lines such that the allocated memory will not be larger than 1GiBi. If we do have more lines we get chunks of half the max size to make data locality better and keep within the memory restriction given. This is because we will at most have allocated memory for two chunks at a time, which is not allowed to exceed 1GiBi. If the number of lines is less then there will only be one chunk which will be the whole datafile.    

We then calculate the number of chunks necessary and then allocate the memory that we will need for the two chunks which will be in memory simultaneously. After this we loop over the chunks with two for-loops. Within the first loop we read the first chunk into memory from the file and if the index $ix=0$ we calculate the distances between the points in the first chunk.

In the second loop we start at the index $jx=ix+1$, where $ix$ is the index of the outer loop since we want to ensure we only count the distances once. Then we read in the second chunk into memory and if $ix=0$ we calculate the distances between the points in the second chunk. This way we only calculate the internal distances of each chunk once. Thereafter we compute the distances between the first and second chunk.

When we have iterated through all the chunks we first free the memory that was used for the chunks and then write the result to stdout with printf. 

### Computing the distances
The computation of distance is done in two different ways depending on if the distance between the points in a chunk is calculated or if the distance between points between two different chunks. For the fist way two for-loops are used. An outer that ranges from $i=0$ to $i<$ chunk size $- 1$ and an inner ranging from $j=i+1$ to $j<$ chunk size. This will make sure that the one pair of points will only be calculated once and that the distance between a point and itself will never be calculated. Inside the inner for-loop the distance (the euclidean norm) is calculated using sqrtf(). The distance is then multiplied by 100 and the truncated by casting the float to an unsigned short. This way only two decimals will be accounted for and this will not be as time consuming as using roundf(). 
The calculation of distances between the chunks is done in a similar way the only difference is the loops. Here the outer loop ranges from $i=0$ to $i<$ chunk size of the ''first'' chunk and the inner loop ranges from $j=0$ to $j <$ size of the ''second'' chunk. 

### Parallelism
We only implemented parallelism on the distance computations. This was done with a reduction so every thread has a local copy of the result vector which they individually increment as they calculate and then at the end they add the local ones together. We also made it so that the indices and distance variable was private amongst the threads as to cause no conflicts.

One could probably use parallelism to speed up the parsing of the files, however, this seems a bit more tricky in our implementation since we would need to share the file pointer somehow so that every thread could read the file simultaneosly otherwise we would need to synchronise them explicitly.

Another ''solution'' could be to simply copy the file so that each thread would have its own file to read. This seems like a silly method since we already have a memory restriction on our program and it would be silly if we could just copy our data files ad nauseum to let every thread read its own.

### Recording distances
To simplify our recording of distance and frequency we just use one vector since we may encode all the information that we need in it. Since we know that the coordinates will be in the interval [-10,10] for all three dimensions we know that the maximum distance is 34.65.

$$
\sqrt{(10-(-10))^2+(10-(-10))^2+(10-(-10))^2}=20\sqrt{3}\approx34.65.
$$

And since we know that we should record the distances to two decimal precision we can use the places in a vector of length 3466. Where the places in the vector correspond to the length and the values of every place correspond to the frequency of the distance. This also includes zero if we want to count that distance. The extra place will not matter substantially in any case. We use a vector of unsigned longs with length 3466 in our program.  

## Memory management
We are given the restriction of not using more than $1024^3$ bytes at any one given time. Since we have chosen to parse and store the numbers in the file as floats we know that each line in a file requires $8\times3=24$ bytes each. With some simple arithmetic this yields that we may at most store about $44739242$ floats until we reach this limit. And since we want to store the numbers in at least two chunks to compare the points we may at most store $22369621$ floats in each. To simplify we have set the maximum number of lines to be $22300000$.

If a file contains more that we set the chunksize to be half of the maximum number to ensure that we keep comfortably within the bounds. This is also due to the fact that we allocate enough memory on the heap to handle a rest of any size, i.e if the modulus of the amount of lines and the chosen chunksize is bad then we might have almost enough to have twice the memory allocated and to avoid violating our restriction we divide by half.
Regarding the rest of the program we don't heap allocate anything else since it would be slower than simply stack allocating everything as we need it.


## Performance
There is many things to improve in our program to increase performance. We will mention some that were identified to be of value.
**Parsing:**
In the file parsing we use the library function fscanf to parse the coordinates as floats which is not an optimal choice. A better choice would be to further use the knowledge that we have of the input data. We know that it is always on the form ''$\pm xy.zwq$'' which we may use to write our own parser that parses the numbers as something smaller than floats, like short integers, to save space. Thus we will only need to cast the numbers to floats when we do the square root in the distance calculation and then cast back.
**Computing the distance:**
Here we use the function sqrtf to compute the square root which is faster than using sqrt or any other slow function. We could however still improve this by implementing a faster approximative scheme like the babylonian method/Heron's method. This would likely increase our performance even more since we may essentially choose our precision which should save time.
