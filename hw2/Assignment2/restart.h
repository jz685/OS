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
	printf("start divide: \n\n"); // #################
	int i = 0;
	int i1 = 0, i2 = 0, i3 = 0;
	char key1[] = ".gif", key2[] = ".bmp", key3[] = ".png";
	for(i = 0; i < len; i++){
		printf("%s\n", files[i]);
	}
	
	printf("\n\n");
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
			printf("no match!\n");	// #################
		}
	}
	
	printf("-------------------finish division--------------------\n\n");
	a[0] = i1;
	printf("number of gif: %d\n",a[0]);
	a[1] = i2;
	printf("number of bmp: %d\n",a[1]);
	a[2] = i3;
	printf("number of png: %d\n",a[2]);

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
	printf("file: %s\n", combined);
}

void path_name_pid(const char* outputdir, const char *name, const pid_t id, char * combined){
	sprintf(combined, "%s/%s/%s_%ld.jpg", getcwd(NULL,0), outputdir, rmextension(name), id);
	printf("thumbnnail: %s\n", combined);
}

void path_name_html(const char* outputdir, const char *name, char * combined){
	sprintf(combined, "%s/%s/%s.html", getcwd(NULL,0), outputdir, rmextension(name));
	printf("thumbnnail: %s\n", combined);
}
// create html
void createHTML(char* path_html, char *path_pic, char *next_path_html){
	FILE *fp = fopen(path_html, "w");
	fprintf(fp, "<html>\n<head>\n<meta http‐equiv=\"refresh\" content=\"1; URL=file://");
	fprintf(fp, next_path_html);
	fprintf(fp, "\">\n</head>\n<body><img src=\"");
	fprintf(fp, path_pic);
	fprintf(fp, "\"></body>\n</html>\n");
	fclose(fp);
}



