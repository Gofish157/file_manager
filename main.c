#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
typedef struct dirent dirent;
typedef struct stat Stat;
typedef struct tm tm;
int main(){
    const char *path = ".";
    DIR *current_directory = opendir(path);
    if (current_directory == NULL){
        printf("Directory could not be opened");
        return -1;
    }

    Stat file_state;
    dirent *current_file;
    int a;
    
    printf("What do you want to output? \n1 - All files \n2 - Regular files \n3 - Hidden files \n");
    scanf("%d", &a);
    
    switch (a)
    {
    case 1:
        while ((current_file = readdir(current_directory)) != NULL){
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", path, current_file->d_name);
            if ((stat(file_path, &file_state)) == 0){
                time_t access_time = file_state.st_atime;
                tm *local_time = localtime(&access_time);
                char formated_time[32];
                if (local_time != NULL) {
                    strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                } 
                else {
                    snprintf(formated_time, sizeof(formated_time), "Unknown time");
                }
                strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                printf("%s\n", current_file->d_name);
                printf("File size: %ld bytes \n", file_state.st_size);
                printf("Last access time: %s \n\n", formated_time);
            }
            else printf("Could not get information about file %s \n", current_file->d_name);
            

        
    }
        break;

    case 2:
        while ((current_file = readdir(current_directory)) != NULL){
            if (current_file->d_name[0] != '.') {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", path, current_file->d_name);
            if ((stat(file_path, &file_state)) == 0){
                time_t access_time = file_state.st_atime;
                tm *local_time = localtime(&access_time);
                char formated_time[32];
                if (local_time != NULL) {
                    strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                } 
                else {
                    snprintf(formated_time, sizeof(formated_time), "Unknown time");
                }
                strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                printf("%s\n", current_file->d_name);
                printf("File size: %ld bytes \n", file_state.st_size);
                printf("Last access time: %s \n\n", formated_time);
            }
            else printf("Could not get information about file %s \n", current_file->d_name);
        }
    }
    break;
    case 3:
        while ((current_file = readdir(current_directory)) != NULL){
            if (current_file->d_name[0] == '.') {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", path, current_file->d_name);
            if ((stat(file_path, &file_state)) == 0){
                time_t access_time = file_state.st_atime;
                tm *local_time = localtime(&access_time);
                char formated_time[32];
                if (local_time != NULL) {
                    strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                } 
                else {
                    snprintf(formated_time, sizeof(formated_time), "Unknown time");
                }
                strftime(formated_time, sizeof(formated_time), "%a, %d %b %Y %I:%M %p", local_time);
                printf("%s\n", current_file->d_name);
                printf("File size: %ld bytes \n", file_state.st_size);
                printf("Last access time: %s \n\n", formated_time);
            }
            else printf("Could not get information about file %s \n", current_file->d_name);
        }
        
    }
    break;

    default:
    printf("Еhere is no such option\n");
        break;
    }
    
    closedir(current_directory);
    return 0;

}