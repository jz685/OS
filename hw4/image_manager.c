#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>

//	Define
typedef struct imagefile
{
	int FileId; 
	char *FileName;
	char *FileType;
	int Size;
	int TimeofModification;
	int ThreadId;
	/*
	FileId: inode number of the file in the directory, 
	FileName: Name of the image file in the directory, 
	FileType: Type of the file in the directory, 
	Size: Size in bytes of the file, 
	TimeOfModification: Time when this file was last modified, 
	ThreadId: Unique identifier of the thread responsible for writing the 
		aforementioned information of the current image file to the HTML file.
	*/
}Image_File;

//	Initiate
pthread_t *mythreads;
int error;

//	Read Directory
int ReadDir(const char *dir)
{
	int file_count = 0;
	int temp = 0;
	struct dirent* ptr;
	DIR* srcdir = opendir(dir);

	if (srcdir == NULL)
	{
		fprintf(stderr, "The Directory is not opened, Error.");
		return -1;
	}
	//	Check if dir opened
	while((ptr = readdir(srcdir)) != NULL)
	{
		char* d_name;
		d_name = ptr -> d_name;

		//	check whether it is a directory
		if (ptr -> d_type & DT_DIR)
		{
			if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
			{
				char path[100];
				sprintf(path, "%s/%s", dir, d_name);
				temp = ReadDir(path);
				file_count += temp;
			}
		}
		else
		{
			printf("\n/////////////////////////START//////////////////////////\n%s/%s\n", dir, d_name);
			// if (isJPG(d_name))
			// {
				
					
			// }else{
			// 	printf("The file is not JPG file, skipped it.\n");
			// }
			
		}
		
	}
	closedir(srcdir);
	return file_count;
}

//	Call function
void *printa (int c)
{
	int *cc;
	//	Allocate space
	if ((cc = (int *)malloc(sizeof(int))) == NULL)
    	return NULL;
    //	Print
	fprintf(stderr, "I am %d\t my numb is: %d\n", (int)pthread_self(), c);
	//	Increase variable c
	c = c + 10;
	// sleep(1);
	*cc = c;
	return cc;
}

// //	Call function 2
// void *printb (void *c)
// {
// 	int *temp;
// 	temp = (int *)c;
// 	//	Print
// 	fprintf(stderr, "I am %d\t my numb is: %d\n", (int)pthread_self(), *temp);
// 	// temp = temp + 10;
// 	return temp;
// }

//	Main function
int main(void)
{
	//-----------initiation------------
	// int c = 0;
	int i;
	int *returnvalue;
	int numfds = 5;

	//------------function-------------
	//	Reserve space for thread id
	if ((mythreads = (pthread_t *)calloc(numfds, sizeof(pthread_t))) == NULL)
	{
    	perror("Failed to allocate space for thread IDs");
	}

	//	Creating the threads
	for (i = 0; i < numfds; i++)
	{
		if ((error = pthread_create(mythreads + i, NULL, (void *)&printa, (void *)i)))
		{
			fprintf(stderr, "Failed to create thread %d: %s\n", i, strerror(error));
	    	mythreads[i] = pthread_self();
		}
	}

	//	Handling threads
	for (i = 0; i < numfds; i++) 
	{
		if (pthread_equal(pthread_self(), mythreads[i]))
	    	continue;
	    if ((error = pthread_join(*(mythreads + i), (void **)&returnvalue)))
			fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(error));
		else 
			fprintf(stderr, "I am returning value: %d\n", *returnvalue);
	}

	// //	Deteach threads
	// for (i = 0; i < numfds; i++) 
	// {
	// 	// if (pthread_equal(threadtemp, mythreads[i]))
	//  //    	continue;
	//     if (error = pthread_detach(*(mythreads + i)))
	// 		fprintf(stderr, "Failed to deteach thread %d: %s\n", i, strerror(error));
	// }
	free(returnvalue);
	free(mythreads);
	fprintf(stderr, "I am parent\n");
	return 0;
}

