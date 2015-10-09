/* Information:
 * CSci 4061 Spring 2015 Assignment 4
 * Name1 = Zixiao Wang
 * Name2 = Jia Zhang
 * StudentID1 = 5107014
 * StudentID2 = 4388046
 */
#include <errno.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/dir.h>
#include <signal.h>

typedef enum {
	JPG, 
	PNG,
	BMP,
	GIF,
	ELSE
} imagetype;

//	Define
struct imagefile
{
	/*
	 * FileId: inode number of the file in the directory, 
	 * FileName: Name of the image file in the directory, 
	 * FileType: Type of the file in the directory, 
	 * Size: Size in bytes of the file, 
	 * TimeOfModification: Time when this file was last modified, 
	 * ThreadId: Unique identifier of the thread responsible for writing the 
	 * aforementioned information of the current image file to the HTML file.
	 */
	ino_t FileId; 
	char FileName[128];
	char FileType[4];
	off_t Size;
	char TimeofModification[80];
	pthread_t ThreadId;

};


static pthread_mutex_t  htmllock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  outputlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numjpglock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numpnglock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numbmplock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numgiflock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numdirlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  loglock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  numthreadslock = PTHREAD_MUTEX_INITIALIZER;

char log_path[200] = {};
char outputlog_path[200] = {};


struct thread_args {
	char dir[200] ;
	char html_path[200];
	char mode[5];
	int id;					// indicate which kind of file that thread should check
};

int num_jpg = 0;
int num_png = 0;
int num_bmp = 0;
int num_gif = 0;
int num_threads = 0;
int num_dir = 0;
int executionTime = 0;



//--------------------------helper function------------------------------------
void printImage(struct imagefile image_info)
{
	FILE *temp;
	temp = fopen(outputlog_path, "a");
	fprintf(temp, "FileId: %u\n", (unsigned int)image_info.FileId);
	fprintf(temp, "FileName: %s\n", image_info.FileName);
	fprintf(temp, "FileType: %s\n", image_info.FileType);
	fprintf(temp,"Size: %d\n", (int)image_info.Size);
	fprintf(temp, "TimeofModification: %s\n", image_info.TimeofModification);
	fprintf(temp, "ThreadId: %lu\n", (unsigned long int)image_info.ThreadId);
	fclose(temp);
}

//-------------------------------------concatenate two char array-----------------------------------------
char *rmextension(const char * fn){
	size_t l = strlen(fn);
	char *newfn = (char *)malloc(l-3);
	memcpy(newfn, fn, l-4);
	newfn[l-4] = 0;
	return newfn;
}


//---------------------------append to html------------------------------
void appendHTML(char *html_path, char *file_path, struct imagefile image_info)		
	// every time call this function would add one file info to html
{
	FILE* fp;
	//printf("\n--------------append to html---------------\n");			//#############################
	fp = fopen(html_path, "a");
	if (fp == NULL)
	{
		pthread_mutex_lock(&outputlock);
		FILE *temp;
		temp = fopen(outputlog_path, "a");
		fprintf(temp, "The html file is not opened, Error.\n");
		fclose(temp);
		pthread_mutex_unlock(&outputlock);
	}
	char temp[200];


	// write html file
	//printf("start write to html file\n");
	fprintf(fp, "<a href=\"");				// href
	fprintf(fp, "%s", file_path);
	fprintf(fp, "\">\n");
	//printf("write href\n");			//#############################

	fprintf(fp, "<img src=\"");				// img src
	fprintf(fp, "%s", file_path);
	fprintf(fp, "\" width=100 height=100></img></a>\n");
	//printf("write img src\n");			//#############################


	fprintf(fp, "<p align= \"left\">");
	sprintf(temp, "FileId: %u, ", (unsigned int)image_info.FileId);
	fputs(temp, fp);
	//printf("FileId: %u\n", (unsigned int)image_info.FileId);

	sprintf(temp, "FileName: %s, ", image_info.FileName);
	fputs(temp, fp);
	//printf("FileName: %s\n", image_info.FileName);

	sprintf(temp,"FileType: %s, ", image_info.FileType);
	fputs(temp, fp);
	//printf("FileType: %s\n", image_info.FileType);

	sprintf(temp, "Size: %d, ", (int)image_info.Size);
	fputs(temp, fp);
	//printf("Size: %d\n", (int)image_info.Size);

	sprintf(temp, "TimeofModification: %s, ", image_info.TimeofModification);
	fputs(temp, fp);
	//printf("TimeofModification: %s\n", image_info.TimeofModification);

	sprintf(temp, "ThreadId: %lu", (unsigned long int)image_info.ThreadId);
	fputs(temp, fp);
	//printf("ThreadId: %lu\n", (unsigned long int)image_info.ThreadId);

	fprintf(fp, "</p>");
	//printf("write other info\n");			//#############################


	fclose(fp);
}

//------------------------------is JPG, PNG, BMP, GIF------------------------------------
imagetype typecheck(char *filename)
{
	int len = strlen(filename);
	int i = 0;
	char suffixOfFile[4];
	for (i = 0; i < 4; i++)
	{
		suffixOfFile[i] = filename[len - 4 + i];				// get last 4 character(suffix of file)
	}
	//fprintf(stderr,"The suffix of file is: %s\n", suffixOfFile);

	if(!(strncmp(suffixOfFile, ".jpg", 4)))
	{
		return JPG;
	}else if(!(strncmp(suffixOfFile, ".png", 4)))
	{
		return PNG;
	}else if (!(strncmp(suffixOfFile, ".bmp", 4)))
	{
		return BMP;
	}else if (!(strncmp(suffixOfFile, ".gif", 4)))
	{
		return GIF;
	}else{
		return ELSE;
	}
}

//-----------------------Read Directory------------------------------
int ReadDir(void *argp)
{
	struct thread_args *args = argp;
	char *dir = args->dir;
	char *html_path = args->html_path;
	char *mode = args->mode;
	int id = args->id;

	int file_count = 0;
	struct dirent* ptr;
	DIR* srcdir = opendir(dir);

	if (srcdir == NULL)
	{
		//fprintf(stderr, "The Directory is not opened, Error.\n");
		return -1;
	}
	//	Check if dir opened
	while((ptr = readdir(srcdir)) != NULL)
	{
		//printf("file name: %s\n", ptr->d_name);
		char* d_name;
		d_name = ptr -> d_name;

		//	check whether it is a directory
		if ((ptr -> d_type & DT_DIR) && id == 0)						// it is a dir
		{

			if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
			{
				//----------number of thread each dir level-------------
				int numfds = 0;			// decided by mode
				if (!strcmp(args->mode, "v1"))
				{
					numfds = 1;
				}else if (!strcmp(args->mode, "v2"))
				{
					numfds = 5;
				}
				else{
					//perror("No mode matched!");
				}
				//---------------Initialization--------------
				pthread_t *mythreads;
				int error, status;

				//---------------Reserve space for thread id-----------------------
				if ((mythreads = (pthread_t *)calloc(numfds, sizeof(pthread_t))) == NULL)
				{
					pthread_mutex_lock(&outputlock);
					FILE *temp;
					temp = fopen(outputlog_path, "a");
					fprintf(temp, "Failed to allocate space for thread IDs.\n");
					fclose(temp);
					pthread_mutex_unlock(&outputlock);
				}

				//----------------new input argu for new dir thread-------------------
				char path[200];
				sprintf(path, "%s/%s", dir, d_name);
				struct thread_args *arguments = malloc(sizeof *args);

				if (arguments != NULL)
				{
					int j = 0;
					for (j = 0; j < 200; j++)
					{
						arguments->dir[j] = path[j];
						arguments->html_path[j] = html_path[j];
					}
					for (j = 0; j < 5; j++)
					{
						arguments->mode[j] = mode[j];
					}

					//------------------------create threads---------------------
					int i = 0;
					for (i = 0; i < numfds; i++)
					{
						arguments->id = i;
						if ((error = pthread_create(&mythreads[i], NULL, (void *)&ReadDir, arguments)))
						{
							pthread_mutex_lock(&outputlock);
							FILE *temp;
							temp = fopen(outputlog_path, "a");
							fprintf(temp, "Failed to create thread %d: %s\n", i, strerror(error));
							fclose(temp);
							pthread_mutex_unlock(&outputlock);
							continue;
						}
						if ((error = pthread_join(mythreads[i], (void **)&status)))
						{
							pthread_mutex_lock(&outputlock);
							FILE *temp;
							temp = fopen(outputlog_path, "a");
							fprintf(temp, "Failed to join thread %d: %s\n", i, strerror(error));
							fclose(temp);
							pthread_mutex_unlock(&outputlock);
						}else{
							pthread_mutex_lock(&outputlock);
							FILE *temp;
							temp = fopen(outputlog_path, "a");
							fprintf(temp, "Joined thread %d: %s, parent thread: %lu\n", i, strerror(error), mythreads[i]);
							fclose(temp);
							pthread_mutex_unlock(&outputlock);
						}
						//-------------------num of threads ++ -----------------------
						pthread_mutex_lock(&numthreadslock);
						num_threads += 1;
						pthread_mutex_unlock(&numthreadslock);
					}
				}else{
					// print error information that could not malloc
				}

				//------------------------num dir ++ ------------------------
				pthread_mutex_lock(&numdirlock);
				num_dir += 1;
				pthread_mutex_unlock(&numdirlock);
			}
		}
		else 					// it is a file
		{	
			pthread_mutex_lock(&outputlock);
			FILE *temp;
			temp = fopen(outputlog_path, "a");
			fprintf(temp, "\n/////////////////////////START//////////////////////////\n%s/%s\n", dir, d_name);
			fclose(temp);
			pthread_mutex_unlock(&outputlock);
			
			if (!strcmp(args->mode, "v1"))
			{
				if (typecheck(d_name) != ELSE)
				{
					//-------------LOCK html file and write to it-------------------

					struct imagefile *image = malloc(sizeof(struct imagefile));
					char file_path[200];
					sprintf(file_path, "%s/%s", dir, d_name);
					struct stat attrib;
					stat(file_path, &attrib);

					//--------------FileID
					image->FileId = attrib.st_ino; 						
					//--------------FileName
					int i = 0;
					for (i = 0; i < strlen(d_name); i++)
					{
						image->FileName[i] = d_name[i];					
					}
					image->FileName[i] = '\0';
					//--------------FileType
					switch(typecheck(d_name))
					{
						case 0:
							strncpy(image->FileType, "jpg", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numjpglock);
							num_jpg += 1;
							pthread_mutex_unlock(&numjpglock);
							break;
						case 1:
							strncpy(image->FileType, "png", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numpnglock);
							num_png += 1;
							pthread_mutex_unlock(&numpnglock);
							break;
						case 2:
							strncpy(image->FileType, "bmp", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numbmplock);
							num_bmp += 1;
							pthread_mutex_unlock(&numbmplock);
							break;
						case 3:
							strncpy(image->FileType, "gif", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numgiflock);
							num_gif += 1;
							pthread_mutex_unlock(&numgiflock);
							break;

					}
					//--------------FileSize
					image->Size = attrib.st_size;
					//--------------File Last modification Time
					strftime(image->TimeofModification, sizeof(image->TimeofModification), "%c", gmtime(&(attrib.st_mtime)));
					//--------------ThreadId
					image->ThreadId = pthread_self();

					pthread_mutex_lock(&outputlock);
					printImage(*image);
					pthread_mutex_unlock(&outputlock);
					
					//------------------------mutex---------------------------
					pthread_mutex_lock(&htmllock);
					appendHTML(html_path, file_path, *image);
					pthread_mutex_unlock(&htmllock);


				}else{
				}
			}else{
				if ((typecheck(d_name) == JPG && id == 1) || (typecheck(d_name) == PNG && id == 2) || (typecheck(d_name) == BMP && id == 3) || (typecheck(d_name) == GIF && id == 4))
				{
					//-------------LOCK html file and write to it-------------------
					struct imagefile *image = malloc(sizeof(struct imagefile));

					char file_path[200];
					sprintf(file_path, "%s/%s", dir, d_name);
					struct stat attrib;
					stat(file_path, &attrib);

					//--------------FileID
					image->FileId = attrib.st_ino; 						
					//--------------FileName
					int i = 0;
					for (i = 0; i < strlen(d_name); i++)
					{
						image->FileName[i] = d_name[i];					
					}
					image->FileName[i] = '\0';
					//--------------FileType
					switch(typecheck(d_name))
					{
						case 0:
							strncpy(image->FileType, "jpg", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numjpglock);
							num_jpg += 1;
							pthread_mutex_unlock(&numjpglock);
							break;
						case 1:
							strncpy(image->FileType, "png", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numpnglock);
							num_png += 1;
							pthread_mutex_unlock(&numpnglock);
							break;
						case 2:
							strncpy(image->FileType, "bmp", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numbmplock);
							num_bmp += 1;
							pthread_mutex_unlock(&numbmplock);
							break;
						case 3:
							strncpy(image->FileType, "gif", 3);
							image->FileType[3] = '\0';
							pthread_mutex_lock(&numgiflock);
							num_gif += 1;
							pthread_mutex_unlock(&numgiflock);
							break;

					}
					//--------------FileSize
					image->Size = attrib.st_size;
					//--------------File Last modification Time
					strftime(image->TimeofModification, sizeof(image->TimeofModification), "%c", gmtime(&(attrib.st_mtime)));
					//--------------ThreadId
					image->ThreadId = pthread_self();
					
					pthread_mutex_lock(&outputlock);
					printImage(*image);
					pthread_mutex_unlock(&outputlock);

					//------------------------mutex---------------------------
					pthread_mutex_lock(&htmllock);
					appendHTML(html_path, file_path, *image);
					pthread_mutex_unlock(&htmllock);

				}else{
					pthread_mutex_lock(&outputlock);
					FILE *temp;
					temp = fopen(outputlog_path, "a");
					fprintf(temp, "The file is not JPG, PNG, BMP, GIF file, skipped it.\n");
					fclose(temp);
					pthread_mutex_unlock(&outputlock);
				}
			}

		}

	}
	closedir(srcdir);
	return file_count;
}

//--------------------------------ARGSUSED-------------------------------
static void myhandler(int s) {
	FILE* fp;
	char temp[200] = {};
	pthread_mutex_lock(&loglock);
	fp = fopen(log_path, "a");
	sprintf(temp, "Time    %d ms  #dir     %d      #files  %d\n", 100*(++executionTime), num_dir, num_png + num_jpg + num_bmp + num_gif);
	fputs(temp, fp);
	fclose(fp);
	pthread_mutex_unlock(&loglock);
}

static int setupinterrupt(void) {   			/* set up myhandler for SIGPROF */
	struct sigaction act;
	act.sa_handler = myhandler;
	act.sa_flags = 0;
	return (sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));
}

static int setupitimer(void) {  				/* set ITIMER_PROF for 2-second intervals */
	struct itimerval value;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 5;
	value.it_value = value.it_interval;
	return (setitimer(ITIMER_PROF, &value, NULL));
}
