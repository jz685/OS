/* Information:
 * CSci 4061 Spring 2015 Assignment 4
 * Name1 = Zixiao Wang
 * Name2 = Jia Zhang
 * StudentID1 = 5107014
 * StudentID2 = 4388046
 */
 
how you analyzed the run times for the variants and compare their
performance based on time and space requirements.


According to the log file for different mode showed below:

Log for variant 1

------------------------------------------------
Time    100 ms  #dir     2      #files  12

Programme initiation: Sun Apr 19 21:02:53 GMT 2015

Number of jpg files = 12
Number of bmp files = 5
Number of png files = 8
Number of gif files = 5

Total number of valid image files = 30

Total time of execution = 1
--------------------------------------
number of threads created = 11

Log for variant 2

------------------------------------------------
Time    100 ms  #dir     0      #files  0
Time    200 ms  #dir     2      #files  6
Time    300 ms  #dir     4      #files  10
Time    400 ms  #dir     9      #files  23

Programme initiation: Sun Apr 19 21:01:12 GMT 2015

Number of jpg files = 12
Number of bmp files = 5
Number of png files = 8
Number of gif files = 5

Total number of valid image files = 30

Total time of execution = 4
--------------------------------------
number of threads created = 55

we could find out that variant 1 has a greater performance than variant 2.
Becase all the thread writes into the same catalog.html and catalog.log,
so if there are more threads, those threads have to wait for more time than 
less threads which do the same work.

