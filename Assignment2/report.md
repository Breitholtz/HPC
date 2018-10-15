# Assignment 2 : Threads

In this assignment we have implemented parallellism with POSIX threads. This was done in a program which takes the square of sidelength 2, centered at the origin, and divides it up into a given amount of points. For these pieces the program then starts threads to compute which root Newton's method would converge to in each point and how many iterations it took to get there. This is done for a polynomial $x^d-1$ where d is given. This information is then stored in a global array and simultaneously written to a .ppm file by a separate thread. This thread only writes the information to file and takes no part in the computations mentioned above. 

The program also takes command line arguments which govern the following properties:
1. The exponent, d, for the polynomial.
2. How many points, l, the square should be partitioned into.
3. How many threads, t, that should be used for the newton computation.

Additionally,  we use the following include files: 
* pthread - for POSIX threads mutices and such
* math - essentially only to use fabs()
* stdlib - malloc and free
* stdio - easy printing and writing to file
* string - for comparing strings
* time - for our own benchmarking

We have also defined constants such as:
* DIST - lower limit to abort iteration in newtons method
* MAX - maximum limit to abort interation in newtons method
* MAX_ITER - caps number of iterations before writing to file

We have used a number of global variables to ease the use of threads. We will go through and explain the different parts of the program.
   
### 	Main
	In the main function we start by calling the argument parsing function, parse_args, which assigns the command line arguments to global variables. We then use these to allocate some memory which we will use some (previously defined) global arrays to point to. These global arrays are:
* initial - used to point to the initial values that we want our Newton function to start at. 
* iterations - used to point to the memory where we store the number of iterations for each point, this is an integer array since it should suffice with a few million iterations as a natural maximum.
*  result - points to where we store an integer which corresponds to which root our newton method converged to
* rows_done - an array where we keep track of which rows the program finished calculating. Initialised to 0.
* roots_exact - the array where we store the complex roots for the chosen  polynomial 

This is followed by the computation of the roots for the given polynomial. For polynomials on the form $x^d-1$ we know that one root is always 1+0i and the rest are spaced by k/d around the rest of the unit circle in the complex plane. We may then for every value of d easily precompute the values of the roots.

We then create the threads that we need and initialise a mutex which we will use later. We here also allocate a small array called Row which we use to send in the starting row for each respective thread.

Finally we join all the threads and free all the memory which we have allocated in main.

###  parse_args
This function is in charge of parsing the command line arguments. It starts by using strncmp to see of the first two characters of the first argument matches any of our supported flags (-t or -l) and uses strtol to parse the number after the flag. It prints an error message to stdout and exits if if there is either something that cannot be parsed after the flag or if the flag/argument is invalid/unknown.

It repeats this for the second argument and then parses the third argument directly using strtol since we know it should only be a number, though the function errors if it cannot parse the argument or there is some junk after the argument, such as '3s' for example.

Finally it takes the arguments that it has parsed and assigns them to the global variables: POWER, SIZE and THREADS.

### threaded_newton
This function is used to calculate the root to which every point converge. To ensure that the threads does not compute the same row a global and a local variable is used. The global variable indicates which row to compute next and the local variable indicates which row the thread presently calculates. These variables are called **Row** and **Index** respectively. When each thread is started the function is passed an argument that specifies the starting row  (ranging from row 0 to row < number of threads) from the matrix which contains the starting values. To make this clearer take for example that we have four threads. Thread 1 is then passed **Row** = 0, thread 2 is passed **Row** = 1, and so on. Thus **Index** = 4 since it is the next row to be calculated. 

The computation then starts by going into a while loop which breaks when **Row**$\geq$ number of rows (-l input into newton.c). Since each thread has been assigned a starting row by the passed argument and as such starts at that row. Then we directly go into a for loop over the elements of the current row; for every element we now compute the root using newtons method. The method begins with a while-loop which breaks when the exact root is in a $10^{-3}$ radius of the approximated root or if the method have diverged too far in either the real or imaginary direction, i.e. $|z_{re}| \geq 10^{10}$ or $|z_{im}| \geq 10^{10}$. 
To compute the next step ($i+1$) we use the fact that we may write the iteration for the imaginary and real part of our polynomial, using Euler's formula, on the form
$$
 z_{{re}_{i+1}}=[z_{re_i}(d-1)+\frac{1}{|z|^{d-1}}\cos(arg(z)(1-d))]/d, \\
 z_{im_{i+1}}=[z_{im_i}(d-1)+\frac{1}{|z|^{d-1}}\sin(arg(z)(1-d))]/d.
$$
Where $|z|^{d-1}$ is computed using a for-loop and arg(z) using **atan2()**.
The distance of the approximated $z$ and each of the exact roots is checked every iteration to see if we are sufficiently close to any of the roots. This is done using a for-loop and an if statement.  If we are close enough a boolean variable is set as true and the root it converged to is noted afterwards the newton method breaks, otherwise the function iterates once more while incrementing the count of how many iterations there has been.

When a row is computed the results of which root the initial point converged to and the number of iterations are stored in two different matrices. Also the corresponding row in the vector rows_done is set to 1.
We then consult the global index to see which index the next row we should compute has. Using a mutex to first lock the global index, then the local row index is then set equal the global index ( **Row** $=$ **Index**) and lastly the global index is incremented by 1 before unlocking the mutex. Thus making certain that the next row is only computed by one thread. When all rows have been computed the function terminates.


### writeppm
This function is used to take the data that is computed by threaded_newton and write it to two .ppm files, one for the iterations and one for which root the function converged to for each point. 

It first creates two character arrays on the stack which we will store our filenames in. Then sprintf is employed to create the filenames. Then the two files are created and opened for writing.

We use fprintf to easily write the header to both files, then we proceed to the main writing stage. It consists of a while loop which runs until an index $ix$ is equal to SIZE, i.e until we have fully written all the rows to the files. Inside we have a an if-statement which checks if row $ix$ is done.  If the row isn't done then we continue and wait for it to be done.

If the row is done then we proceed to write it to the files using the colors globally defined and fwrite by writing elementwise. We also check if the iterations are larger than a chosen max value, MAX_ITER, and if they are we set them to the max. Inthis way we write a grey scale file using the grey scale globally defined and fwrite. After this we increment $ix$ and continue waiting until the next row is done. This repeats until the writing is finished. We then close the files and return.

One should note that the while loop in this function ensures that the rows of the arrays are written in consecutive order which means that the picure will be assembled correctly.

## Performance of the program

The parse_args seems to take about 12000 ns to complete its parsing which is fairly fast and as such we did not consider it for optimisation but if one wrote it in a more condensed way without errorchecking one could get the time down significantly. By timing the script in the linux terminal for different number of threads and polynomials the following results is obtained.

| Size of image | Thread count | Exponent of polynomial | Time| 
|:--------|:-------:|--------:|--------:|
| 1000  | 1	| 1 | 0.304s |
| 1000  | 1 | 2 | 0.645s |
| 1000 	| 1 | 5 | 0.725s |
| 1000  | 1 | 7 | 1.171s |
| 1000 	| 2 | 5 | 0.552s |
| 1000 	| 3 | 5 | 0.379s |
| 1000 	| 4 | 5 | 0.311s |
| 1000 	| 10 | 7 | 0.159s |
| 50000	| 10 | 7 |  |


// IDEAS: to check our programs performance characteristic we can time every function for different inputs and see how they behave and how long it appears to take them to process. Maybe easier with a profiler... Timed with a slow Gantenbein...

~~~

