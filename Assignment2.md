# Assignment 2 : Threads

In this assignment we have implemented parallellism with POSIX threads. This was done in a program which takes the square of sidelength 2, centered at the origin, and divides it up into L pieces. For these pieces the program then starts threads to compute which root Newton's method would converge to and how many iterations it took to get there. 

This information is then stored in a global array and simultaneously written to a .ppm file by a separate thread which handles the 




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



An image defined by URL: ![gustav](http://www.chalmers.se/siteCollectionImages/institutioner/MV/Profilbilder/perljung.jpg)

## code example


~~~ C
#include <stdio.h>
int main(){
printf("Cool stuff bro!");
return 0;
}

~~~

<!--stackedit_data:
eyJoaXN0b3J5IjpbLTI2MDgyOTQ2NCwxMjE3NjY2Mzg1LC05OT
AyNTkxMDgsLTE2MTQ4NTg3NDMsNDEzMjQ2NCwyMDk3NzgxMDI0
XX0=
-->