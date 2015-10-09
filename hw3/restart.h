#include <errno.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <dirent.h>

//-------------------------------------------r_wait------------------------------------------
pid_t r_wait(int *stat_loc) {
	int retval;
	while (((retval = wait(stat_loc)) == -1) && (errno == EINTR)) ;
	return retval;
}

//---------------divide files into 3 different parts according to types of file-----------------
void divide(char **files, int len, char **bmp, char **png, char **gif, int* a){
	//printf("start divide: \n\n"); // #################
	int i = 0;
	int i1 = 0, i2 = 0, i3 = 0;
	char key1[] = ".gif", key2[] = ".bmp", key3[] = ".png";
	for(i = 0; i < len; i++){
		//printf("%s\n", files[i]);
	}
	
	//printf("\n\n");
	for(i = 0; i < len; i++){
		if (strstr(files[i], key1) != NULL){
			gif[i1] = files[i];
			i1++;
		}else if (strstr(files[i], key2) != NULL){
			bmp[i2] = files[i];
			i2++;
		}else if (strstr(files[i], key3) != NULL){
			png[i3] = files[i];
			i3++;
		}else{
			//printf("no match!\n");	// #################
		}
	}
	
	//printf("-------------------finish division--------------------\n\n");
	a[0] = i1;
	//printf("number of gif: %d\n",a[0]);
	a[1] = i2;
	//printf("number of bmp: %d\n",a[1]);
	a[2] = i3;
	//printf("number of png: %d\n",a[2]);

}

//-------------------------------------cconcatenate two char array-----------------------------------------
char *rmextension(const char * fn){
	size_t l = strlen(fn);
	char *newfn = (char *)malloc(l-3);
	memcpy(newfn, fn, l-4);
	newfn[l-4] = 0;
	return newfn;
}

void path_name(const char* inputdir, const char *name, char* combined){
	sprintf(combined, "%s/%s/%s", getcwd(NULL, 0) ,inputdir, name);
	//printf("file: %s\n", combined);
}

void path_name_pid(const char* outputdir, const char *name, const pid_t id, char * combined){
	sprintf(combined, "%s/%s/%s_%ld.jpg", getcwd(NULL,0), outputdir, rmextension(name), id);
	//printf("thumbnnail: %s\n", combined);
}

// create html
void createHTML(char *outputdir)
{
	DIR *dir;
	int k = 0, length = 0;
	char key[] = "_thumb.jpg";
	char key2[] = ".jpg";
	struct dirent *ptr;
	
	char *images[100] = {};					// for original file name
	
	if ((dir = opendir(outputdir)) == NULL){
		perror("Error happend when open directory!");
	}
	while((ptr = readdir(dir)) != NULL){
		if ( (strstr(ptr->d_name ,key)) == NULL && (strstr(ptr->d_name ,key2)) != NULL)
		{
			images[k] = ptr->d_name;
			k++;
			length++;
		}
		
	}
	
	/*
	for (k = 0; k < length; k++)
	{
		fprintf(stderr, "filename: %s\n", images[k]);
	}
	*/

	FILE* fp;
	char html_path[100] = {};
	sprintf(html_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "filesystem_content.html");
	fp = fopen(html_path, "w");
	fprintf(fp, "<html>\n<head>\n<title>Image 01</title>\n</head>\n<body>\n");
	int kk = 0;
	for (kk = 0; kk < length; kk++){
		// write html file
		fprintf(fp, "<a href=\"");				// href
		fprintf(fp, getcwd(NULL,0));
		fprintf(fp, "/");
		fprintf(fp, outputdir);
		fprintf(fp, "/");
		fprintf(fp, images[kk]);
		fprintf(fp, "\">\n");
		
		fprintf(fp, "<img src=\"");				// img src is thumbnailfile
		fprintf(fp, getcwd(NULL,0));
		fprintf(fp, "/");
		fprintf(fp, outputdir);
		fprintf(fp, "/thumbs/");
		fprintf(fp, rmextension(images[kk]));
		fprintf(fp, "_thumb.jpg\"/></a>\n");
		

	}
	fprintf(fp, "</body></html>\n");
	fclose(fp);
}

int deleteENCD(char *outputdir)
{
	DIR *dir;
	char key[] = ".jpgencd";
	struct dirent *ptr;
	
	if ((dir = opendir(outputdir)) == NULL){
		perror("Error happend when open directory!");
		return 1;
	}
	
	while((ptr = readdir(dir)) != NULL){
		if ( strstr(ptr->d_name ,key) != NULL)
		{
			char temp[100] = {};
			sprintf(temp, "%s/%s", outputdir, ptr->d_name);
			fprintf(stderr, "%s\n", temp);												// #################
			if( remove(temp) != 0 )
			{
				perror( "Error deleting file" );
				return -1;
			}
		}
	}
	return 0;
}

void genThumbs(char *outputdir)
{
	printf("\n---------------create thumbs dir in output dir-----------------\n");			// #################
	DIR * dir = NULL;
	char thumb_path[200] = {};
	sprintf(thumb_path, "%s/%s/%s", getcwd(NULL, 0), outputdir, "thumbs");
	
	if ((dir = opendir(thumb_path)) == NULL){
		mkdir(thumb_path, 0700);
		fprintf(stderr, "---------------Thumbs created---------------------\n");				// #################
	}else{
		closedir(dir);
	}
	
	//-----------------------------------read files from input----------------------------------
	int k = 0, length = 0;
	char key[] = "_thumb.jpg";
	char key2[] = ".jpg";
	struct dirent *ptr;
	
	char *images[100] = {};					// for original file name
	
	if ((dir = opendir(outputdir)) == NULL){
		perror("Error happend when open directory!");
	}
	while((ptr = readdir(dir)) != NULL){
		if ( (strstr(ptr->d_name ,key)) == NULL && (strstr(ptr->d_name ,key2)) != NULL)
		{
			images[k] = ptr->d_name;
			k++;
			length++;
		}
	}
	
	
	//-----------------------------------start convert thumb-------------------------------------
	pid_t childpid;
	int i = 0;
	for (i = 0; i < length; i++) 
	{
		sleep(0.5);
		if ((childpid = fork()) <= 0)
			break;
	}
	
	while(r_wait(NULL) > 0);
	
	if (childpid == -1)
	{
		perror("Failed to fork.");
	}
	
	if (childpid == 0)
	{
		char thumb_name[200] = {};
		char file_name[200] = {};
		sprintf(thumb_name, "%s/%s/thumbs/%s%s", getcwd(NULL, 0), outputdir, rmextension(images[i]),"_thumb.jpg" );
		sprintf(file_name, "%s/%s/%s", getcwd(NULL, 0), outputdir, images[i]);
		execlp("convert", "convert", "-resize", "200x200", file_name, thumb_name, NULL);
	}
	
}
