#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

char in_buf[1024];
char out_buf[1024];

char inputFile[50];
char outputFile[50];

char intemp[1024];                                                                                         
char outtemp[1024];


void CopyFile(char* input, char* output){

    int n, in, out;
    char buf[1024];

    // copy regular file to regular file.
    if ((in = open(input, O_RDONLY)) < 0) {
        perror(input);
        exit(0); 
    }

    if ((out = open(output, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0) {
        perror(output);
        exit(0); 
    }
    printf(">> Start copying regular file...\n");
    while ((n = read(in, buf, sizeof(buf))) > 0){  
        write(out, buf, n); 
    }
    printf(">> Finish copying regular file :D\n");     
    close(out);
    close(in);
}

int CopyDir(DIR *read_dir, DIR *write_dir, char *dirname){
    while(1) {
        struct dirent *r_dirt = readdir(read_dir);
        struct dirent *w_dirt = readdir(write_dir);
	struct stat statBuf;
	stat(dirname, &statBuf);

        if(r_dirt == NULL){ // EOF
            printf(">> Finish copying directory file [%s] :D\n", dirname);
            return 1;
        }
	
        //check file or directory
        lstat(r_dirt->d_name, &statBuf);
        if(S_ISDIR(statBuf.st_mode) == 1){ // directory
            if(strcmp(r_dirt->d_name, ".") == 0 || strcmp(r_dirt->d_name, "..") == 0) {
                printf(">> except file: %s\n", r_dirt->d_name); // Pass
            }
            else{
                printf(">> copying directory file: %s\n", r_dirt->d_name);
                sprintf(in_buf,"%s/%s", inputFile, r_dirt->d_name);
                sprintf(out_buf,"%s/%s", outputFile, r_dirt->d_name);
                printf(">> input directory: %s/ output directory: %s\n", in_buf, out_buf);

                struct stat tempbuf;
                stat(in_buf, &tempbuf);
                int mkdirFlag = mkdir(out_buf, tempbuf.st_mode); 
                if(mkdirFlag == 0){
                    printf(">> copy sub-directory %s.\n", in_buf);
                    strcpy(intemp, inputFile);
                    strcpy(outtemp, outputFile);
                    strcpy(inputFile, in_buf);
                    strcpy(outputFile, out_buf);
                }
                else{
                    perror(">> Error: Can't make sub-directory..T.T\n");
                    printf("%d: %s\n", errno, strerror(errno));
                    exit(0);
                }
                DIR* indir = opendir(in_buf);
                DIR* outdir = opendir(out_buf);
                CopyDir(indir, outdir, in_buf);
                // RECURSIVE END
                strcpy(inputFile, intemp);
                strcpy(outputFile, outtemp);
            }
        }
        else if(S_ISREG(statBuf.st_mode) == 1){ // regular file
            printf(">> copying regular file: %s\n", r_dirt->d_name);
            char ref[1024];
            sprintf(ref, "%s/%s", inputFile, r_dirt->d_name);              
            char outref[1024];
            sprintf(outref,"%s/%s",outputFile,r_dirt->d_name);

            CopyFile(ref, outref);
        }
	    else{
	        printf(">> error: Not regular file & directory file. (Filename: %s)\n", r_dirt->d_name);
	    }
    }
    return 1;                             
}

 
int main(int argc, char **argv)
{
    int n, in, out;
    char buf[1024];
    struct stat in_buf;
    struct stat out_buf;
    DIR* input;
    DIR* output;
    
    if (argc < 3) {
        write(2, "Usage : ./copy src dst\n", 25);
        return -1; 
    }

    stat(argv[1], &in_buf);
    stat(argv[2], &out_buf);

    // 1. argv[1] && argv[2] == regular file
    if(S_ISREG(in_buf.st_mode) == 1 && S_ISREG(out_buf.st_mode) == 1){
	printf(">> start copying: regular file -> regular file\n");
        CopyFile(argv[1],argv[2]);
    }
    // 2. argv[1] && argv[2] == directory file
    else if(S_ISDIR(in_buf.st_mode) == 1 && S_ISDIR(out_buf.st_mode) == 1){
	printf(">> start copying: directory file -> directory file\n");

        input = opendir(argv[1]);
        output = opendir(argv[2]);                                                 
        
        inputFile[0]='\0';
        strcat(inputFile,"./");
        strcat(inputFile,argv[1]);

        outputFile[0]='\0';
        strcat(outputFile,"./");
        strcat(outputFile,argv[2]);
        int res = CopyDir(input, output, argv[1]);
        if(res == 0){
            printf(">> Fail copying directory file :(\n");;
        }
    }
    // 3. argv[1] == directory file && argv[2] == regular file
    else if(S_ISDIR(in_buf.st_mode) == 1 && S_ISREG(out_buf.st_mode) == 1){
        perror(">> Copy Failed. [src] is directory file. But [dst] is regular file...T.T\n");
        exit(0);
    }
    // 4. argv[1] == regular file && argv[2] == directory file
    else if(S_ISREG(in_buf.st_mode) == 1 && S_ISDIR(out_buf.st_mode) == 1){
	printf(">> start copying: regular file -> directory file\n");
        // read directory path                  
        char input[50];
        input[0]='\0';
        strcat(input,"./");                                   
        strcat(input, argv[1]);
        printf("%s\n",input);
        // write directory path
        char output[50];
        output[0]='\0';
        strcat(output,"./");
        strcat(output, argv[2]);
        strcat(output,"/");
        strcat(output, argv[1]);

        CopyFile(input, output);
    }
    else{
        perror(">> Copy Failed.\n");
        exit(0);
    }
    closedir(input);
    closedir(output);
    return 0;
}