/*
 * Information
 * CSci 4061 Spring 2015 Assignment 2
 * Name1= Zixiao Wang
 * Name2= Jia Zhang
 * StudentID1= 5107014
 * StudentID2= 4388046
 * Commentary=description of the program, problems encountered while coding, etc.
 *
 */


#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "restart.h"

int main(int argc, char *argv[]) {

//--------------------------initiating variables--------------------------------------------
	/* argc = 3, 
	 * argv[0] = parallel_convert: name of final executable file
	 * argv[1] = convert_count: number of processes(from 1 - 10)
	 * argv[2] = ouputdir
	 * argv[3] = inputdir 
	 */
	//int convert_count = atoi(argv[1]);
	char outputdir[64];
	strcpy(outputdir, argv[2]);
	printf("outputdir: %s\n", outputdir);			// #################
	char inputdir[64];
	strcpy(inputdir, argv[3]);
	printf("inputdir: %s\n", inputdir);			// #################

	
	pid_t childpid, currentpid;						// pid
	int i = 0, length = 0, c;					// counter, length: number of files
	char **files = (char **)malloc(100 * sizeof(char*));							// all files
	char **png = (char **)malloc(100 * sizeof(char*));								// png files
	char **bmp = (char **)malloc(100 * sizeof(char*));								// bmp files 
	char **gif = (char **)malloc(100 * sizeof(char*));								// gif files

	
	char thumbnails[100][100];															// thumbnails
	
	//jia-init
	//initiating local variables
	//pid_t childpid, currentpid;
	int convert_count;
	//char * files [100];
	int fileleft;
	//int filenum;
	//Iniciate the shared memory tobe *shared
	//int *shared = mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	
	//check input number
	if (argc < 2) 
	{
		fprintf (stderr, "Usage: parallel_convert convert_count output_dir input_dir\n");
		fprintf (stderr, "For example: $ ./parallel_convert 2 ./myweb ./input_dir\n");
		return 1;
	}

//-----------------------Read files in inputdir--------------------------------------------

	DIR *dir;
	struct dirent *ptr;
	if ((dir = opendir(inputdir)) == NULL){
		perror("Error happend when open directory!");
		return 1;
	}
	
	printf("Files:\n");			// #################
	
	ptr = readdir(dir);
	printf("ptr: %x\n", ptr);	// #################
	while(ptr != NULL){
		files[i] = ptr->d_name;
		printf("ptr->d_name: %s\n", ptr->d_name);			// #################
		printf("files[%d]: %s\n", i, files[i]); 			// #################
		printf("ptr: %x\n\n", ptr); // #################

		ptr = readdir(dir);
		i++;
		length++;
		
		
	}
	closedir(dir);
	printf("length: %d \n\n", length); // #################


//-------------------------------divide files----------------------------------------------
	int n_files[3] = {0, 0, 0};  
	divide(files, length, bmp, png, gif, n_files);
	printf("start flag creation\n");
	printf("number of gif: %d\n",n_files[0]);
	printf("number of bmp: %d\n",n_files[0]);
	printf("number of png: %d\n",n_files[0]);

//------------------------------------------------------------------------------------------
	int *gifflag = mmap(0, n_files[0]*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	int *bmpflag = mmap(0, n_files[1]*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	int *pngflag = mmap(0, n_files[2]*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	for (i = 0; i < n_files[0]; i++) { gifflag[i] = 1;}
	for (i = 0; i < n_files[1]; i++) { bmpflag[i] = 1;}
	for (i = 0; i < n_files[2]; i++) { pngflag[i] = 1;}
	
//------------------------------------------------------------------------------------------
	int j = 0;


	printf("number of gif: %d\n", n_files[0]);											// #################
	printf("gif:\n");
	for (j=0;j < n_files[0]; j++){ printf("%s\n", gif[j]); }								// #################
	printf("\nnumber of bmp: %d\n", n_files[1]);											// #################
	printf("bmp:\n");
	for (j=0;j < n_files[0]; j++){ printf("%s\n", bmp[j]); }								// #################
	printf("\nnumber of png: %d\n", n_files[2]);											// #################
	printf("png:\n");
	for (j=0;j < n_files[0]; j++){ printf("%s\n", png[j]); }								// #################

	//Jia's code----------------------------------------------------

	/*
	//set test case to *files
	filenum = 11;
	char *k0 = "aaa";
	files[0] = k0;
	char *k1 = "bbb";
	files[1] = k1;
	char *k2 = "ccc";
	files[2] = k2;
	char *k3 = "ddd";
	files[3] = k3;
	char *k4 = "eee";
	files[4] = k4;
	char *k5 = "fff";
	files[5] = k5;
	char *k6 = "ggg";
	files[6] = k6;
	char *k7 = "hhh";
	files[7] = k7;
	char *k8 = "iii";
	files[8] = k8;
	char *k9 = "jjj";
	files[9] = k9;
	char *k10 = "kkk";
	files[10] = k10;
	char *k11 = "lll";
	files[11] = k11;

    //set test case for shared memory
    for (c = 0; c < filenum; c++)
        shared[c] = 1;
    c ++;
    shared[c] = NULL;
    */

	//calculate the filenum left for converting
	fileleft = 0;
	for (c=0; c < n_files[0]; c++)
	{
	 fileleft = fileleft + gifflag[c];
	}
	for (c=0; c < n_files[1]; c++)
	{
	 fileleft = fileleft + bmpflag[c];
	}
	for (c=0; c < n_files[2]; c++)
	{
	 fileleft = fileleft + pngflag[c];
	}
	
	//printing the status of converting
    fprintf(stderr, "File converting progress status: \n");
    fprintf(stderr, "gif files: ");
    for (c = 0; c < n_files[0]; c++)
        fprintf(stderr, " %d ", gifflag[c]);
    fprintf(stderr, "\n");
	fprintf(stderr, "bmp files: ");
    for (c = 0; c < n_files[1]; c++)
        fprintf(stderr, " %d ", bmpflag[c]);
    fprintf(stderr, "\n");
    fprintf(stderr, "png files: ");
    for (c = 0; c < n_files[2]; c++)
        fprintf(stderr, " %d ", pngflag[c]);
    fprintf(stderr, "\n");
	
	//------------------fork loop----------------------
	while (fileleft >0) 
	{
		//set convert count number
		convert_count = atoi(argv[1]);
		fprintf(stderr, "convert count is: %d\n", convert_count);
		for (i = 1; i <= convert_count; i++) 
		{
			sleep(0.5);
			if ((childpid = fork()) <= 0)
				break;
		}
		// wait for all children 
		while(r_wait(NULL) > 0);
		//get current pid
		currentpid = (long)getpid();
		// fork failure handle code 
		if (childpid == -1) 
		{
			perror("Failed to fork");
			return 1;
		}
		
		//fork child code
		if (childpid == 0) 
		{
			//converting
			if (currentpid % 2 == 0)
			{
				//convert png only
				//check if any png photos neecs to be converted. if no, return. if yes, convert and return
				fprintf(stderr, "i-png-1:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
				//execl
				for (c = 0; c <= 100; c++) 
				{
					//fprintf(stderr, "i-2-4:%d process ID:%ld ------%d------%s------ \n", i, (long)getpid(), *s, files[j]);
					if (pngflag[c] == 1) 
					{
						pngflag[c] = 0;
						//fprintf(stderr, "i-2-2:%d process ID:%ld \t c:%d \t shard[c]:%d \t %s------ \n", i, (long)getpid(), c, shared[c], files[c]);
						break;
					}
				}
				execl("/bin/echo", "echo","ss--ss", NULL);
				perror("execl() failure!\n\n");
				fprintf(stderr, "execl failed\n");
				return(1);
			}
			else if (currentpid % 3 == 0 && currentpid % 6 != 0)
			{
				//convert bmp only
				//check if any bmp photos neecs to be converted. if no, return. if yes, convert and return
				fprintf(stderr, "i-bmp-1:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
				//execl
				for (c = 0; c <= 100; c++) {
					//fprintf(stderr, "i-2-4:%d process ID:%ld ------%d------%s------ \n", i, (long)getpid(), *s, files[j]);
					if (bmpflag[c] == 1) 
					{
						bmpflag[c] = 0;
						//fprintf(stderr, "i-3-2:%d process ID:%ld \t c:%d \t shard[c]:%d \t %s------ \n", i, (long)getpid(), c, shared[c], files[c]);
						break;
					}
				}
				execl("/bin/echo", "echo","ss--ss", NULL);
				perror("execl() failure!\n\n");
				fprintf(stderr, "execl failed\n");
				return(1);
			}
			else
			{
				//convert gif only
				//check if any gif photos neecs to be converted. if no, return. if yes, convert and return
				fprintf(stderr, "i-gif-1:%d process ID:%ld parent ID:%ld child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
				//execl
				for (c = 0; c <= 100; c++) 
				{
					//fprintf(stderr, "i-2-4:%d process ID:%ld ------%d------%s------ \n", i, (long)getpid(), *s, files[j]);
					if (gifflag[c] == 1) 
					{
						gifflag[c] = 0;
						//fprintf(stderr, "i-o-2:%d process ID:%ld \t c:%d \t shard[c]:%d \t %s------ \n", i, (long)getpid(), c, shared[c], files[c]);
						break;
					}
				}
				execl("/bin/echo", "echo","ss--ss", NULL);
				perror("execl() failure!\n\n");
				fprintf(stderr, "execl failed\n");
				return(1);
			}
		}
		else 
		{
			// parent code 
			//check if all photos are converted. if not, run fork again. if yes, finish and return
			fprintf(stderr, "Parient:%d process ID:%ld parent ID:%ld\n", i, (long)getpid(), (long)getppid() );
			//update the filenum left for converting
			fileleft = 0;
			for (c=0; c < n_files[0]; c++)
			{
			 fileleft = fileleft + gifflag[c];
			}
			for (c=0; c < n_files[1]; c++)
			{
			 fileleft = fileleft + bmpflag[c];
			}
			for (c=0; c < n_files[2]; c++)
			{
			 fileleft = fileleft + pngflag[c];
			}
			//printing the status of converting
			fprintf(stderr, "File converting progress status: \n");
			fprintf(stderr, "gif files: ");
			for (c = 0; c < n_files[0]; c++)
				fprintf(stderr, " %d ", gifflag[c]);
			fprintf(stderr, "\n");
			fprintf(stderr, "bmp files: ");
			for (c = 0; c < n_files[1]; c++)
				fprintf(stderr, " %d ", bmpflag[c]);
			fprintf(stderr, "\n");
			fprintf(stderr, "png files: ");
			for (c = 0; c < n_files[2]; c++)
				fprintf(stderr, " %d ", pngflag[c]);
			fprintf(stderr, "\n");
		}
	}
	//----------------end fork loop--------------------
	//release the shared memory
	//munmap(shared, sizeof *shared);
	//print finish status and finish
	fprintf(stderr, "All files converted, program finished\n");
	return 0;

}

