#include <stdio.h>
#include <time.h>
#include "time_form.h"  

void time_fm(time_t access_time, char* formated_time) {
    struct tm *local_time = localtime(&access_time);
    if (local_time != NULL) {
        strftime(formated_time, 100, "%a, %d %b %Y %I:%M %p", local_time);
    } else {
        snprintf(formated_time, 100, "Unknown time");
    }
}
