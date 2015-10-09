a. Personal Information for the group:
	CSci 4061 Spring 2015 Assignment 3
	Name1 = Zixiao Wang
	Name2 = Jia Zhang
	StudentID1 = 5107014
	StudentID2 = 4388046


b. CSELab machine tested on: kh4-250 -28

c.Syntax and usage pointers:
	Usage: test <input_dir> <output_dir> <log_filename>

Assumption: 1) offsets will be aligned at 512 bytes
	    2) file name will not be larger than 15 characters.
	    3) input files exist and are readable.
	    4) none of the input file names will have any spaces in the name.
	    5) total file count won’t exceed 128 or the total size would not exceed the size of the mini file system.
	    6) There are no soft / hard links in the directory tree. There are only files.


d. Special test cases handled or note handled:
	None.


e. Experiment results:
(Note: Sample question and experiment from CSci2021 text book problem 6.4 answer!)
Assumption for disk parameters: 
	Rotation Rate – 15,000 RPM
	Average Seek time – 4 ms
	Average number of sectors per track – 1000
	Sector size = Block size = 512 bytes.

Max Rotation Time = 1/RPM * 60secs/1min = 4 ms

Average Rotation Time = 1/2 * Max Rotation Time = 2 ms

Assuming every time accessing to the file system, we read a sector(block).
Average number of accessing to file system = (Total number of accessing to file system)/(number of files in input dir) 
					   = 5529/6 = 921.5 times

So the Average blocks of a file = Average # accessing to file system * 1/2 = 461 blocks

Best case:
	Rotation number to read a file(n) = (Average blocks of a file) * (Average # setors/track)) 
					  = 461 blocks * 1/1000 
					  = 0.461 (rotations)
	
	In the optimal case, the blocks are mapped to contiguous sectors, on the same cylinder, that can be read on after the other
	without moving the head. Once the head is positioned over the first sector it takes n(n = 0.4, it is based on the rough 
	estimate of example input_directory size) of the disk to read all blocks which store the file. So the total time to read file is 
	Following:

	Time = (Average Seek Time) + (Average Rotation time) + n * Max Rotation Time = 
	     = 4 ms + 2 ms + 0.461 * 4 ms = 7.844 ms

Average case:
	In this case, where blocks are mapped randomly to sectors, reading each of file(about 461 blocks) requires 
	Average Seek Time + Average Rotation Time ms, so the total time to read a file is following:

	Time = ((Average Seek Time) + (Average Rotation time)) * 921.5 blocks
	     = (4 ms + 2 ms) * 461 = 2766 ms(2.766 s).
