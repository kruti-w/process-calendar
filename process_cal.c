
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500

//from timeplay.c provided with the assignment files
void dt_format(char *formatted_time, const char *dt_time, const int len){
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}

//similar to timeplay.c provided with the assignment files
void tm_format(char *formatted_tm, const char *tm_str, const int len){
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(tm_str, "%2d%2d",
        &temp_time.tm_hour, &temp_time.tm_min);

    full_time = mktime(&temp_time);
    strftime(formatted_tm, len, "%l:%M %p", 
        localtime(&full_time));
}

//from timeplay.c provided with the assignment files
void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time, *p_temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16); 
    after[MAX_LINE_LEN - 1] = '\0';
}


// defining the two structs here for RRULE and all the events in general 
 struct RRULE{
        char FREQ[80];
        char WKST[80];
        char UNTIL[80];
    };
      
    struct Cal_events {
        char DTSTART[80];
        char DTEND[80];
        struct RRULE rrule;
        char LOCATION[80];
        char SUMMARY[80];

    };

// the function to print the final formatted string
// takes in the struct of all events, the start date and end date range provided in the input args
void print_events(struct Cal_events* events, char *strtdate, char *enddate){
    char formatted_time[30];
    char *strt_time;
    char *end_time;
    int flag = 0;
    char new_date[80];
    char old_date[80] = "";

    for (int k = 0; k < 80; k++){  
        //if events[k] isn't populated, break
        if (strcmp(events[k].DTSTART, "") == 0){
            break;
        }

        //Get the new date from the current event
        strcpy(new_date, events[k].DTSTART);
        if (strncmp(new_date, strtdate, 8)<0){
            continue;
        }
        
        if (strncmp(new_date, enddate, 8)>0){
            break;
        }
        if (strncmp(old_date, new_date, 8)!= 0){
            dt_format(formatted_time, events[k].DTSTART, 30);
            if (flag == 0){
                printf("%s\n", formatted_time);
                flag = 1;
            }else {
                printf("\n%s\n", formatted_time);
            }
            for (int dash = 0; dash < strlen(formatted_time); dash++){
                printf("-");
                
            }
            printf("\n");
            strcpy(old_date, new_date);
        }
        
        strt_time = &events[k].DTSTART[9];   
        end_time = &events[k].DTEND[9];
        tm_format(strt_time, strt_time, 30);
        tm_format(end_time, end_time, 30);
        printf("%s to %s: ", strt_time, end_time);
        printf("%s {{%s}}\n", events[k].SUMMARY, events[k].LOCATION);
        if (events[k+1].DTSTART == "") break;
    }
}

//compares two dates
//called in qsort later in main
int compare(const void *a, const void *b){
    const struct Cal_events *dt_1 = (struct Cal_events*)a;
    const struct Cal_events *dt_2 = (struct Cal_events*)b;
    if (strcmp(dt_1->DTSTART, "") == 0){
        return -1;
    }else if (strcmp(dt_2->DTSTART, "") == 0){
    return -1;
    }else if (strcmp(dt_1->DTSTART, dt_2->DTSTART) < 0){
        return -1;   
    }else if(strcmp(dt_1->DTSTART, dt_2->DTSTART) > 0){
        return 1;
    }else{ 
        return 0;
    }
}

// https://codeforwin.org/2016/04/c-program-to-trim-trailing-white-space-characters-in-string.html
//referenced the website above for a function to remove trailing white spaces
void trimTrailing(char * str){
    int index, i;

    // Set default index */
    index = -1;

    // Find last index of non-white space character */
    i = 0;
    while(str[i] != '\0'){
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
            index= i;
        }
        i++;
    }
    // Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}

// Main function
//Reads input arguments, reads the file, copies the file contents into the structs defined earlier
int main(int argc, char *argv[]){
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i; 

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }   
    // code in main provided up to this point

    //define struct Cal_events and populate the DTSTART array with the empty string
    struct Cal_events cal_events[500];
    for (int y=0; y<500; y++){
        strcpy(cal_events[y].DTSTART, "");
    }
     
     // this block of code concatenates the start date, month, year into a single string "from_date", which then gets "T235959" concatenated to it
    char from_date[18] = "";
    char from_year[5];
    char from_month[3];
    char from_day[3];
    sprintf(from_year, "%2d", from_y);
    sprintf(from_month, "%02d", from_m);
    sprintf(from_day, "%02d", from_d);
    strcat(from_date, from_year);
    strcat(from_date, from_month);
    strcat(from_date, from_day);
    strcat(from_date, "T235959");

    // this block of code concatenates the end date, month, year into a single string "to_date", which then gets "T235959" concatenated to it
    char to_date[18] = "";
    char to_year[5];
    char to_month[3];
    char to_day[3];
    sprintf(to_year, "%2d", to_y);
    sprintf(to_month, "%02d", to_m);
    sprintf(to_day, "%02d", to_d);
    strcat(to_date, to_year);
    strcat(to_date, to_month);
    strcat(to_date, to_day);
    strcat(to_date, "T235959");

    // reading the file
    FILE* filep = fopen(filename, "r");
    if (filep == NULL) return -1;

    char buffer[80];
    int task_index=0;
    char *rrule_tokens[3];
    const char s[] = "\\:; "; //delimiters for strtok

    i = 0;
    while (fgets(buffer, sizeof(buffer), filep)!=NULL){
        if (strncmp(buffer, "DTSTART:", 8)==0){
            strcpy(cal_events[i].DTSTART, &buffer[0]+8); 
            cal_events[i].DTSTART[15] = 0;
        }

        if (strncmp(buffer, "DTEND:", 6)==0){
            strcpy(cal_events[i].DTEND, &buffer[0]+6);
            cal_events[i].DTEND[15] = 0;
        }
        //checks if RRULE exists; tokenizes the string into three tokens, the most relevant is the 'UNTIL' one as it gives the end date range for repeating events
        if (strncmp(buffer, "RRULE:", 6)==0){
            for (int j=0; j<3; j++){
                if (j==0){
                    rrule_tokens[j] = strtok(&buffer[0]+11, s);
                    strcpy(cal_events[i].rrule.FREQ, rrule_tokens[j]);
                }
                if (j==1){
                    rrule_tokens[j] = strtok(&buffer[0]+8, s);
                    strcpy(cal_events[i].rrule.WKST, rrule_tokens[j]);

                }
                if (j==2){
                    rrule_tokens[j] = strtok(&buffer[0]+32, s);
                    strcpy(cal_events[i].rrule.UNTIL, rrule_tokens[j]);
                }  
            } 
        }
        //get the location, remove extra white spaces
        if (strncmp(buffer, "LOCATION:", 9)==0){
            strcpy(cal_events[i].LOCATION, &buffer[0]+9);
            trimTrailing(cal_events[i].LOCATION);
        }
        //get the summary, remove the extra white spaces
        if (strncmp(buffer, "SUMMARY:", 8)==0){
            strcpy(cal_events[i].SUMMARY, &buffer[0]+8);
            trimTrailing(cal_events[i].SUMMARY);
        }
        if (strncmp(buffer, "END:", 4)==0){
            task_index+=1;
            if (strncmp(cal_events[i].rrule.FREQ, "WEEKLY", 6)==0){
                char temp_date[MAX_LINE_LEN];
                char local_cpy[50];  
                dt_increment(temp_date, cal_events[i].DTSTART, 7);
                strcpy(local_cpy, cal_events[i].rrule.UNTIL);
            
                //loop runs if the date of the event is less than the end range date and it's less than the until date specified for recurring events
                //the date is incremented by 7, a new event with that date is created in cal_events
                while(strcmp(temp_date, to_date) < 0 && strcmp(temp_date, local_cpy) < 0) {
                    i += 1;
                    strcpy(cal_events[i].LOCATION, cal_events[i-1].LOCATION);
                    strcpy(cal_events[i].SUMMARY, cal_events[i-1].SUMMARY);
                    strcpy(cal_events[i].DTSTART, temp_date);
                    strcpy(cal_events[i].DTEND, cal_events[i-1].DTEND);
                    dt_increment(temp_date, temp_date, 7);
                }
            }
            i += 1;
        }
    }

    //qsort used to make the dates chronological
    qsort(&cal_events, 80, sizeof(struct Cal_events), compare);
    print_events(cal_events, from_date, to_date);
    fclose(filep); //close the file
   exit(0);
}