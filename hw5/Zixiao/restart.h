#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <dirent.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "md5sum.h"

struct entry
{
	char *filename;
	int namelength;
	int size;
	char *checksum;
};
//initialization
int numPNG = 0;
int numGIF = 0;
int numTIFF = 0;
int numDownload = 0;


//------------------------read catalog.csv file--------------------------
void ReadCatalog(const char *catalog_path, int* numFile)
{
	// start read catalog.csv
	int i = 1;
	// open file
	FILE* fp;
	fp = fopen(catalog_path, "r");
	char *line = NULL;
	char *filename;
	size_t len = 0;
	// read every line from the file, untill the end
	getline(&line, &len, fp);
	while (getline(&line, &len, fp) != -1)
	{
		filename = strtok(line, ",");
		printf("[%d] %s\n", i, filename);
		
		// count files
		(*numFile)++;
		i++;
	}
	

}

// Load Catalog.csv file content into local catalog.csv structure variable
void LoadCatalog(const char *catalog_path, int numFile, struct entry *FileInfo)
{
	// load all information in catalog.csv into structor
	int i = 0;
	FILE* fp;
	fp = fopen(catalog_path, "r");
	
	char *line = NULL;
	size_t len = 0;
	// read every line from the file, untill the end
	getline(&line, &len, fp);
	
	while (getline(&line, &len, fp) != -1)
	{
		char *name = strtok(line, ",");
		FileInfo[i].filename = (char *) malloc (strlen(name) * sizeof(char));
		strcpy(FileInfo[i].filename, name);
		//printf("filename: %s\n", FileInfo[i].filename);
		
		FileInfo[i].namelength = strlen(FileInfo[i].filename);
		//printf("namelength: %d\n", FileInfo[i].namelength);
		
		FileInfo[i].size = atoi(strtok(NULL, ","));
		//printf("size: %d\n", FileInfo[i].size);

		char* checksum = strtok(NULL, ",");
		FileInfo[i].checksum = (char *) malloc (strlen(checksum) * sizeof(char));
		strcpy(FileInfo[i].checksum, checksum);
		//printf("checksum: %s\n\n", FileInfo[i].checksum);

		i++;
	}

	fclose(fp);

}


//---------------------------write to html------------------------------
void Write_to_html(char *html_path,  bool *result_check, int numFile, char *outputdir, struct entry FileInfo[], int * File_Flag)		
{
	FILE* fp;
	fp = fopen(html_path, "w");		// create html file
	if (fp == NULL)
	{
		fprintf(stderr, "The html file is not opened, Error.\n");
	}
	char temp[300];
	
	//-----------------write start of file--------------
	fprintf(fp, "<html>\n<head>\n<title>Downloaded images</title>\n</head>\n<body>\n<h1> Downloaded images</h1>\n<pre>\n");
	// write every file
	int i = 0;
	for (i = 0; i < numFile; i++)
	{
		if (File_Flag[i] == 1)
		{
			if(result_check[i])
			{
				fprintf(fp, "(Checksum match!)    ");
			
				sprintf(temp, "%s/%s", outputdir, FileInfo[i].filename);
				fprintf(fp, "<a href=\"");				// href
				fprintf(fp, "%s", temp);
				fprintf(fp, "\">");
				//printf("write href\n");				//#############################
			}else{
				fprintf(fp, "(Checksum mismatch!)    ");
			}
		

			fprintf(fp, "%s", FileInfo[i].filename);				// img name
			fprintf(fp, "</a><br />\n");
			//printf("write img name\n");			//#############################
		}
	}

	fprintf(fp, "</pre>\n</body>\n</html>");
	

	fclose(fp);
}


//----------------------check file type-----------------------------
//check if png
bool isPNG(char *filename)
{
    int len = strlen(filename);
    int i = 0;
    char suffixOfFile[4];
    for (i = 0; i < 4; i++)
    {
        suffixOfFile[i] = filename[len - 4 + i];                // get last 4 character(suffix of file)
    }
    return !(strncmp(suffixOfFile, ".png", 4));
}
//check if gif
bool isGIF(char *filename)
{
    int len = strlen(filename);
    int i = 0;
    char suffixOfFile[4];
    for (i = 0; i < 4; i++)
    {
        suffixOfFile[i] = filename[len - 4 + i];                // get last 4 character(suffix of file)
    }
    return !(strncmp(suffixOfFile, ".gif", 4));
}
//check if tiff
bool isTIFF(char *filename)
{
    int len = strlen(filename);
    int i = 0;
    char suffixOfFile[5];
    for (i = 0; i < 5; i++)
    {
        suffixOfFile[i] = filename[len - 5 + i];                // get last 5 character(suffix of file)
    }
    return !(strncmp(suffixOfFile, ".tiff", 5));
}


//-----------------------------Read directory------------------------
int ReadDir(const char *dir, const char *absocatalog)
{
    int i;
    int file_count = 0;
    int temp = 0;
    struct dirent* ptr;
    DIR* srcdir = opendir(dir);
    unsigned char sum[MD5_DIGEST_LENGTH];

    if (srcdir == NULL)
    {
        perror("opendir");
        return -1;
    }

    while((ptr = readdir(srcdir)) != NULL)                          // traverse done
    {
        char* d_name;
        d_name = ptr -> d_name;
        printf("\n///////////////////////// SCANNING %s //////////////////////////\n", dir);

        // Open catlog.csv file
        FILE* fp;
        FILE* fpcatalog;

        //----------------check whether it is a directory
        if (ptr -> d_type & DT_DIR)
        {
            if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
            {
                char path[300];
                sprintf(path, "%s/%s", dir, d_name);
                temp = ReadDir(path, absocatalog);
                file_count += temp;
            }
        }else{
            // If it is a file required
            if (isPNG(d_name) || isGIF(d_name) || isTIFF(d_name)){	// if it is png or tiff or gif file
				// add counter
				if(isPNG(d_name))
				{
					numPNG++;
				}
				else if(isGIF(d_name))
				{
					numGIF++;
				}
				else if(isTIFF(d_name))
				{
					numTIFF++;
				}
				
				char path[300];
				sprintf(path, "%s/%s", dir, d_name);
				fprintf(stderr, "%s\n", path);
				
				fp = fopen(path, "r");
				if (fp == NULL) 
				{
					fprintf(stderr, "Cannot open %s/%s .....\n", dir, d_name);
					break;             
				}else{
					// count the size of file
					fseek(fp, 0L, SEEK_END);
					int sz = ftell(fp);
					fseek(fp, 0L, SEEK_SET);

					// Print out on the screen
					md5sum(path, sum);
					fprintf (stderr, "File Name: %s,\tSize: %d,\tMD5: ", d_name, sz);
					for (i = 0; i < MD5_DIGEST_LENGTH; i++)
						printf("%02x", sum[i]);
					printf("\n");

					// Write into the catlog
					fpcatalog = fopen(absocatalog, "a+");

					
					fprintf(fpcatalog, "%s,%d,", d_name, sz);
					fprintf(stderr, "### Name: %s\n", d_name);
					for (i = 0; i < MD5_DIGEST_LENGTH; i++)
					{
						fprintf(fpcatalog, "%02x", sum[i]);
					}
					fprintf(fpcatalog, "\n");
					fclose(fpcatalog);
					file_count++;
				}
				//close the file
				fclose(fp);
			}
            // Check if the type matchs
            // if (isPNG(d_name) || isGIF(d_name) || isTIFF(d_name)){
                
            //     fseek(fp, 0L, SEEK_END);
            //     int sz = ftell(fp);
            //     fseek(fp, 0L, SEEK_SET);
            //     fprintf (stderr, "File Name: %s,\tSize: %d,\t ")
            //     fprintf(fp, 

            // }else{
            //     printf("The file is not png/gif/tiff file, skipped it.\n");
            // }
            
        }
        
    }
    // finished scanning
    closedir(srcdir);
    return file_count;
}

//------------------------------tansfer image---------------------
void Find_imagepath(char* imagedir, struct entry *FileInfo, int index, char *imagepath)
{
	// find the path for a specific file provided with index
    struct dirent* ptr;
    DIR* srcdir = opendir(imagedir);

    if (srcdir == NULL)
    {
        perror("opendir");
    }
	//fprintf(stderr, "In Find_imagepath\nindex: %d\n", index);
	//fprintf(stderr, "Search file: %s\n", FileInfo[index-1].filename);
	
    while((ptr = readdir(srcdir)) != NULL)                          // traverse done
    {
        char* d_name;
        d_name = ptr -> d_name;
		// search for the directory to get the path of the image
        //----------------check whether it is a directory
        if (ptr -> d_type & DT_DIR)
        {
            if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
            {
				// if it is a directory, search inside
                char path[300];
                sprintf(path, "%s/%s", imagedir, d_name);
                Find_imagepath(path, FileInfo, index, imagepath);
            }
        }else{
			// if it is a file, check if match
			if (!strcmp(FileInfo[index-1].filename, d_name))
			{
				sprintf(imagepath, "%s/%s", imagedir, d_name);
			}
		}
	}

}

//------------------------------check sum---------------------
void checksum(char *imagedir, unsigned char sum[][MD5_DIGEST_LENGTH], int numFile, struct entry *FileInfo, int * File_Flag)
{
	int i = 0;
	
	char *file_path[numFile];

	// find all file path in FileInfo
	for (i = 0; i < numFile; i++)
	{
		if (File_Flag[i] != 0)
		{
			file_path[i] = (char *)calloc(512, sizeof(char));
			Find_imagepath(imagedir, FileInfo, i+1, file_path[i]);
		}
	}

	// compute md5sum and store it into sum
	for (i = 0; i < numFile; i++)
	{
		if (File_Flag[i] != 0)
		{
			md5sum(file_path[i], sum[i]);
		}
	};
}
