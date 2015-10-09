/* Information:
 * CSci 4061 Spring 2015 Assignment 4
 * Name1 = Zixiao Wang
 * Name2 = Jia Zhang
 * StudentID1 = 5107014
 * StudentID2 = 4388046
 */
#include <stdio.h>
#include "restart.h"

//---------------------------Main function----------------------------------
int main(int argc, char **argv)
{
	if (setupinterrupt() == -1) {
		//perror("Failed to set up handler for SIGPROF");
		return 1;
	}
	if (setupitimer() == -1) {
		//perror("Failed to set up the ITIMER_PROF interval timer");
		return 1;
	}
	//--------------------------Usage Test--------------------------------------------
	if (argc != 4) 
	{
		//fprintf (stderr, "Usage: ./test output_dir input_dir logfile\n");
		//fprintf (stderr, "Input Directory needs to be absolute \n");
		return 1;
	}


	//-----------initialization------------
	time_t timer = time(NULL);
	char initTime[200] = {};
	strftime(initTime, sizeof(initTime),  "%a %b %d %X %Z %Y", gmtime(&timer));

	/* argc = 3, 
	 * argv[0] = test: name of final executable file
	 * argv[1] = Input_directory
	 * argv[2] = Output_directory
	 * argv[3] = Log_filename 
	 */
	char mode[5];
	char inputdir[200];
	char outputdir[200];

	strcpy(mode, argv[1]);
	strcpy(inputdir, argv[2]);
	strcpy(outputdir, argv[3]);

	//---------------------------------------Output directory----------------------------------------

	DIR * output = NULL;
	//printf("\n---------------create output dir-----------------\n");
	if ((output = opendir(outputdir)) == NULL){
		char out[200];
		sprintf(out, "%s/%s", getcwd(NULL, 0), outputdir);
		//printf("%s\n", out);
		mkdir(out, 0700);
		//printf("Outputdir: %s\n\n", out);									// #################
	}
	closedir(output);

	//----------------------------------------create html file----------------------------------------
	FILE* fp;
	char html_path[200] = {};
	sprintf(html_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "catalog.html");
	fp = fopen(html_path, "w");
	fprintf(fp, "<html>\n<head>\n<title>My Image Manager BMP</title>\n</head>\n<body>\n");
	fclose(fp);

	//----------------------------------------create log file----------------------------------------
	sprintf(log_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "catalog.log");
	fp = fopen(log_path, "w");
	fprintf(fp, "Log for variant ");
	if (!strcmp(mode, "v1"))
	{
		fprintf(fp, "1\n\n------------------------------------------------\n");
	}else if (!strcmp(mode, "v2"))
	{
		fprintf(fp, "2\n\n------------------------------------------------\n");
	}
	else{
		//perror("No mode matched!");
	}
	fclose(fp);
	
	//----------------------------------------create log file----------------------------------------
	sprintf(outputlog_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "output.log");
	fp = fopen(outputlog_path, "w");
	fclose(fp);

	//------------------------------------------start read file------------------------------------
	//----------number of thread each dir level-------------

	int numfds = 0;			// decided by mode
	if (!strcmp(mode, "v1"))
	{
		numfds = 1;
	}else if (!strcmp(mode, "v2"))
	{
		numfds = 5;
	}
	else{
		//perror("No mode matched!");
	}

	//printf("numfds: %d\n", numfds);

	//---------------Initialization--------------
	pthread_t *mythreads;
	int error, status;

	//---------------Reserve space for thread id-----------------------
	if ((mythreads = (pthread_t *)calloc(numfds, sizeof(pthread_t))) == NULL)
	{
		//perror("Failed to allocate space for thread IDs");
	}

	//----------------new input argu for new dir thread-------------------
	struct thread_args *arguments = malloc(sizeof(struct thread_args));

	if (arguments != NULL)
	{
		int j = 0;
		for (j = 0; j < 200; j++)
		{
			arguments->dir[j] = inputdir[j];
			arguments->html_path[j] = html_path[j];
		}
		for (j = 0; j < 5; j++)
		{
			arguments->mode[j] = mode[j];
		}

		//------------------------create threads---------------------
		//printf("\n---------------start read dir-----------------\n");
		int i = 0;
		for (i = 0; i < numfds; i++)
		{
			arguments->id = i;
			//printf("\n  thread %d created.  \n", i);
			if ((error = pthread_create(&mythreads[i], NULL, (void *)&ReadDir, arguments)))
			{
				//fprintf(stderr, "Failed to create thread %d: %s\n", i, strerror(error));
				continue;
			}
			if ((error = pthread_join(mythreads[i], (void **)&status)))
			{
				//fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(error));
			}else{
				//fprintf(stderr, "Joined thread %d: %s, parent thread: %lu\n", i, strerror(error), mythreads[i]);
			}
		}
	}else{
		// print error information that could not malloc
	}

	//--------------------------add end to html file-------------------------------
	fp = fopen(html_path, "a+");
	fprintf(fp, "</body>\n</html>\n");
	fclose(fp);

	//--------------------------add end to log file-------------------------------
	char temp[200] = {};
	fp = fopen(log_path, "a+");
	sprintf(temp, "\nProgramme initiation: %s", initTime);
	fputs(temp, fp);
	sprintf(temp,"\n\nNumber of jpg files = %d\n", num_jpg);
	fputs(temp, fp);
	sprintf(temp, "Number of bmp files = %d\n", num_png);
	fputs(temp, fp);
	sprintf(temp, "Number of png files = %d\n", num_bmp);
	fputs(temp, fp);
	sprintf(temp, "Number of gif files = %d\n", num_gif);
	fputs(temp, fp);
	sprintf(temp, "\nTotal number of valid image files = %d\n", num_png + num_jpg + num_bmp + num_gif);
	fputs(temp, fp);
	sprintf(temp, "\nTotal time of execution = %d", executionTime);
	fputs(temp, fp);
	fprintf(fp, "\n--------------------------------------\n");
	sprintf(temp, "number of threads created = %d", num_threads);
	fputs(temp, fp);
	fclose(fp);

	return 0;
}

