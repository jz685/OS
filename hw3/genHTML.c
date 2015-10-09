#include "mini_filesystem.h"
#include <dirent.h>
#include <stdio.h>

// remove extension of input filename
char *rmextension(const char * fn){
	size_t l = strlen(fn);
	char *newfn = (char *)malloc(l-3);
	memcpy(newfn, fn, l-4);
	newfn[l-4] = 0;
	return newfn;
}


int createHTML(char *outputdir)
{
	DIR *dir;
	int k = 0, length = 0;
	char key[] = "_thumb.jpg";
	char key2[] = ".jpg";
	struct dirent *ptr;
	
	char *images[100] = {};					// for original file name
	
	if ((dir = opendir(outputdir)) == NULL){
		perror("Error happend when open directory!");
		return 1;
	}
	printf("\nwaht the fuck!!\n");
	while((ptr = readdir(dir)) != NULL){
		if ( (strstr(ptr->d_name ,key)) == NULL && (strstr(ptr->d_name ,key2)) != NULL)
		{
			images[k] = ptr->d_name;
			k++;
			length++;
			fprintf(stderr, "filename: %s\n", ptr->d_name);
		}
		
	}

	FILE* fp;
	int kk = 0;
	char thumb[200] = {};
	for (kk = 0; kk < length; kk++){
		// write html file
		fp = fopen("filesystem_content.html", "w");
		fprintf(fp, "<html>\n<head>\n<title>\nImage 0");
		fprintf(fp, (char)kk);
		fprintf(fp, "</title>\n</head>\n<body>\n");
		
		
		fprintf(fp, "<img src=\"");				// img src is thumbnailfile
		fprintf(fp, getcwd(NULL,0));
		fprintf(fp, "/");
		fprintf(fp, outputdir);
		fprintf(fp, "/thumbs/");
		fprintf(fp, rmextension(images[kk]));
		fprintf(fp, "_thumb.jpg\"/></a>");
		
		
		fprintf(fp, "<a href=\"");				// href
		fprintf(fp, getcwd(NULL,0));
		fprintf(fp, "/");
		fprintf(fp, outputdir);
		fprintf(fp, "/");
		fprintf(fp, images[kk]);
		fprintf(fp, "\">");
		
		
		fprintf(fp, "</body></html>\n");
		fclose(fp);
	}
}

int main()
{
	char *outputdir = "testhtml";
	createHTML(outputdir);
	return 0;
}
