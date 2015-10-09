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
	 * argv[1] = mode
	 * argv[2] = Input_directory
	 * argv[3] = Output_directory
	 */
	char mode[5];
	char inputdir[200];
	char outputdir[200];

	strcpy(mode, argv[1]);
	strcpy(inputdir, argv[2]);
	strcpy(outputdir, argv[3]);

	FILE *tempfp;
	
	//---------------------------------------Output directory----------------------------------------
	DIR * output = NULL;
	
	if ((output = opendir(outputdir)) == NULL){
		char out[200];
		sprintf(out, "%s/%s", getcwd(NULL, 0), outputdir);
		mkdir(out, 0700);
		
		pthread_mutex_lock(&outputlock);
		tempfp = fopen(outputlog_path, "a");
		fprintf(tempfp, "Outputdir: %s\n\n", out);									// #################
		fclose(tempfp);
		pthread_mutex_unlock(&outputlock);
	}
	closedir(output);

	//----------------------------------------create output log file----------------------------------------
	sprintf(outputlog_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "output.log");
	tempfp = fopen(outputlog_path, "w");
	fprintf(tempfp, "-------------------Start process-------------------\n");
	fprintf(tempfp, "output.log file created.");
	fclose(tempfp);
	
	//----------------------------------------create html file----------------------------------------
	pthread_mutex_lock(&outputlock);
	tempfp = fopen(outputlog_path, "a");
	fprintf(tempfp, "\n---------------create catalog html file-----------------\n");
	fclose(tempfp);
	pthread_mutex_unlock(&outputlock);
	
	FILE* fp;
	char html_path[200] = {};
	sprintf(html_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "catalog.html");
	fp = fopen(html_path, "w");
	fprintf(fp, "<html>\n<head>\n<title>My Image Manager BMP</title>\n</head>\n<body>\n");
	fclose(fp);

	//----------------------------------------create log file----------------------------------------
	pthread_mutex_lock(&outputlock);
	tempfp = fopen(outputlog_path, "a");
	fprintf(tempfp, "\n---------------create catalog file-----------------\n");
	fclose(tempfp);
	pthread_mutex_unlock(&outputlock);
	
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
		pthread_mutex_lock(&outputlock);
		tempfp = fopen(outputlog_path, "a");
		fprintf(tempfp, "No mode matched!");
		fclose(tempfp);
		pthread_mutex_unlock(&outputlock);
	}
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
		pthread_mutex_lock(&outputlock);
		tempfp = fopen(outputlog_path, "a");
		fprintf(tempfp, "No mode matched!");
		fclose(tempfp);
		pthread_mutex_unlock(&outputlock);
	}

	//printf("numfds: %d\n", numfds);

	//---------------Initialization--------------
	pthread_t *mythreads;
	int error, status;

	//---------------Reserve space for thread id-----------------------
	if ((mythreads = (pthread_t *)calloc(numfds, sizeof(pthread_t))) == NULL)
	{
		pthread_mutex_lock(&outputlock);
		tempfp = fopen(outputlog_path, "a");
		fprintf(tempfp, "Failed to allocate space for thread IDs");
		fclose(tempfp);
		pthread_mutex_unlock(&outputlock);
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
		pthread_mutex_lock(&outputlock);
		tempfp = fopen(outputlog_path, "a");
		fprintf(tempfp, "\n---------------start read dir-----------------\n");
		fclose(tempfp);
		pthread_mutex_unlock(&outputlock);
		
		int i = 0;
		for (i = 0; i < numfds; i++)
		{
			arguments->id = i;
			//printf("\n  thread %d created.  \n", i);
			if ((error = pthread_create(&mythreads[i], NULL, (void *)&ReadDir, arguments)))
			{
				pthread_mutex_lock(&outputlock);
				tempfp = fopen(outputlog_path, "a");
				fprintf(tempfp, "Failed to create thread %d: %s\n", i, strerror(error));
				fclose(tempfp);
				pthread_mutex_unlock(&outputlock);
				continue;
			}
			if ((error = pthread_join(mythreads[i], (void **)&status)))
			{
				pthread_mutex_lock(&outputlock);
				tempfp = fopen(outputlog_path, "a");
				fprintf(tempfp, "Failed to join thread %d: %s\n", i, strerror(error));
				fclose(tempfp);
				pthread_mutex_unlock(&outputlock);
			}else{
				pthread_mutex_lock(&outputlock);
				tempfp = fopen(outputlog_path, "a");
				fprintf(tempfp, "Joined thread %d: %s, parent thread: %lu\n", i, strerror(error), mythreads[i]);
				fclose(tempfp);
				pthread_mutex_unlock(&outputlock);
			}
		}
	}else{
		// print error information that could not malloc
	}

	//--------------------------add end to html file-------------------------------
	pthread_mutex_lock(&outputlock);
	tempfp = fopen(outputlog_path, "a");
	fprintf(tempfp, "------------------add end to html file----------------\n");
	fclose(tempfp);
	pthread_mutex_unlock(&outputlock);
	
	fp = fopen(html_path, "a+");
	fprintf(fp, "</body>\n</html>\n");
	fclose(fp);

	//--------------------------add end to log file-------------------------------
	pthread_mutex_lock(&outputlock);
	tempfp = fopen(outputlog_path, "a");
	fprintf(tempfp, "------------------add end to catalog.log file----------------\n");
	fclose(tempfp);
	pthread_mutex_unlock(&outputlock);
	
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
	
	//--------------------------add end to log file-------------------------------
	pthread_mutex_lock(&outputlock);
	tempfp = fopen(outputlog_path, "a");
	fprintf(tempfp, "------------------add end to output.log file----------------\n Process finish!!");
	
	fclose(tempfp);
	pthread_mutex_unlock(&outputlock);
	
	

	return 0;
}

