# Assignment 2 : Threads

In this assignment we have implemented parallellism with POSIX threads. This was done in a program which takes the square of sidelength 2, centered at the origin, and divides it up into a given amount of points. For these pieces the program then starts threads to compute which root Newton's method would converge to in each point and how many iterations it took to get there. This is done for a polynomial $x^d-1$ where d is given.

This information is then stored in a global array and simultaneously written to a .ppm file by a separate thread. This thread only writes the information to file and takes no part in the computations mentioned above. 

The program also takes command line arguments which govern the following properties:
1. The exponent, d, for the polynomial.
2. How many points, L, the square should be partitioned into.
3. How many threads, T, that should be used for the newton computation.

Additionally,  we use the following include files: 
* pthread - for POSIX threads mutices and such
* math - essentially only to use fabs()
* stdlib - malloc and free
* stdio - easy printing and writing to file
* string - for comparing strings
* time - for our own benchmarking
 We will go through and explain the different parts of the program.
// TODO: -maybe talk more about the constants that we have hardcoded and other global stuff
### 	Main
In the main function we start by calling the argument parsing function, parse_args, which assigns the command line arguments to global variables. We then use these to allocate some memory which we will use some (previously defined) global arrays to point to. These global arrays are:
* initial - used to point to the initial values that we want our Newton function to start at. 
* iterations - used to point to the memory where we store the number of iterations for each point, this is an integer array since it should suffice with a few million iterations as a natural maximum.
*  result - points to where we store an integer which corresponds to which root our newton method converged to
* rows_done - an array where we keep track of which rows the program finished calculating. initialised to 0.
* roots_exact - the array where we store the complex roots for the chosen  polynomial 

This is followed by the computation of the roots for the given polynomial. For polynomials on the form $x^d-1$ we know that one root is always 1+0i and the rest are spaced by k/d around the rest of the unit circle in the complex plane. We may then for every value of d easily precompute the values of the roots.

We then create the threads that we need and initialise a mutex which we will use later. We here also allocate a small array called Row which we use to send in the starting row for each respective thread.

Finally we join all the threads and free all the memory which we have allocated in main.
###  parse_args
This function is in charge of parsing the command line arguments. It starts by using strncmp to see of the first two characters of the first argument matches any of our supported flags (-t or -l) and uses strtol to parse the number after the flag. It prints an error message to stdout and exits if if there is either something that cannot be parsed after the flag or if the flag/argument is invalid/unknown.

It repeats this for the second argument and then parses the third argument directly using strtol since we know it should only be a number, though the function errors if it cannot parse the argument or there is some junk after the argument, such as '3s' for example.

Finally it takes the arguments that it has parsed and assigns them to the global variables: POWER, SIZE and THREADS.

### threaded_newton
This is the function which is used to calculate the convergence of every point in our picture. The function first assigns the argument which was sent to it into a local variable. Then the type declarations of all the used local variables follow. 

The function itself consists of several nested loops

//TODO: explain that we use a global index to tell each thread which row to compute next.
### writeppm
This function is used to take the data that is computed by threaded_newton and write it to two .ppm files. One for the iterations and one for which root the function converged to for each point. 

It first creates two character arrays on the stack which we will store our filenames in. Then sprintf is employed to create the filenames. Then the two files are created and opened for writing.

We use fprintf to easily write the header to both files, then we proceed to the main writing stage. It consists of a while loop which runs until an index $ix$ is equal to SIZE, i.e until we have fully written all the rows to the files. Inside we have a an if-statement which checks if row $ix$ is done.  If the row isn't done then we continue and wait for it to be done.

If the row is done then we proceed to write it to the files with..... We also check if the iterations are larger than a chosen max  value, MAX_ITER, and if they are we set them to the max. After this we increment $ix$ and continue waiting until the next row is done. This repeats until the writing is finished. We then close the files and return.

One should note that the while loop in this function ensures that the rows of the arrays are written in consecutive order which means that the picure will be assembled correctly.

## Performance of the program

The parse_args seems to take about 12000 ns to complete its parsing which is fairly fast and as such we did not consider it for optimisation but if one wrote it in a more condensed way without errorchecking one could get the time down significantly. 

| Size of image | Thread count | Exponent of polynomial | Time| 
|:--------|:-------:|--------:|--------:|
| 1000  | 1  | 1   |          |
| 1000   | 1   | 3  |         |
|1000|1         | 5 |         |
| 1000  | 1   |    7 |        |
| cell4   | cell5   | cell6   ||
|=====
| Foot1   | Foot2   | Foot3   | |
{: rules="groups"}
//TODO: change implementation so that we use fwrite 


// IDEAS: to check our programs performance characteristic we can time every function for different inputs and see how they behave and how long it appears to take them to process. Maybe easier with a profiler...
# Additional stuff
In addition to the source code for your solution, you have to write a report in (kramdoc compatible) markdown that explains your solution, with focus on the program layout and performance characteristics. You shall also answers all questions posed in context of the assignment, if there is any.

# Useful syntax help

1. This is a list item
2. And another item
2. And the third one
   with additional text

* this is an item

## Table example

| Header1 | Header2 | Header3 |
|:--------|:-------:|--------:|
| cell1   | cell2   | cell3   |
| cell4   | cell5   | cell6   |
|----
| cell1   | cell2   | cell3   |
| cell4   | cell5   | cell6   |
|=====
| Foot1   | Foot2   | Foot3   |
{: rules="groups"}

### slightly emphasized 
*emphasized*
### More emphasized
**emphasized**


This is a paragraph
{::comment}
This is a comment which is
completely ignored.
{:/comment}
... paragraph continues here.



An image defined by URL: ![soetis](http://www.chalmers.se/siteCollectionImages/institutioner/MV/Profilbilder/perljung.jpg)

## code example


~~~ C
#include <stdio.h>
int main(){
printf("Cool stuff bro!\n");
return 0;
}

~~~

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTI1Mjg5NjE1XX0=
-->