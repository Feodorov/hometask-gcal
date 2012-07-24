#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define DAYS_IN_WEEK		7
#define	BUFFER_SIZE			256
#define OFFSET				5
#define	ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}

	setlocale(LC_ALL, "");
	char * locale = setlocale(LC_ALL, NULL);

	int month, year;
	sscanf(argv[1], "%d.%d", &month, &year);

	//leap year
	if(!(year % 4))
		days_in_month[1] = 29;

	//get the first day of the week.
	//this info is in LC_TIME, in locale settings
	int first_weekday;
	char path[BUFFER_SIZE], tmp[BUFFER_SIZE];
	locale[5] = 0;
	sprintf(path, "/usr/share/i18n/locales/%s", locale);
	FILE * fp = fopen(path,"r");

	if(fp != NULL)
	{
		while(fgets(tmp, sizeof(tmp), fp) != NULL)
		{
			if (strstr(tmp, "first_weekday") != NULL)
			{
				first_weekday = atoi(tmp + sizeof("first_weekday"));
				break;
			}
		}

		fclose(fp);
	}

	time_t raw_time;
	struct tm * time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	int current_month = time_info->tm_mon;
	int current_date = time_info->tm_mday;

	char datestring[BUFFER_SIZE];
	int i;

	//print header with weekdays
	for(i = first_weekday - 1; i < first_weekday - 1 + DAYS_IN_WEEK; ++i)
	{
		time_info->tm_wday = i % DAYS_IN_WEEK;
		strftime(datestring, sizeof(datestring), "%a", time_info);
		printf(ANSI_COLOR_RED	"%4s "	ANSI_COLOR_RESET, datestring);
	}
	printf("\n");

	//get the weekday for the 1st
	time_info->tm_year = year;
	time_info->tm_mon = month - 1;
	time_info->tm_mday = 1;

	mktime(time_info);

	//if the 1st day of month is not the first day
	//of the week, make an offset for the first week
	int start_day = (time_info->tm_wday - first_weekday + DAYS_IN_WEEK) % DAYS_IN_WEEK;

	memset(datestring, ' ' , OFFSET * start_day);
	datestring[OFFSET * start_day] = '\0';
	printf(datestring);

	//print dates
	for(i = 1; i <= days_in_month[month - 1]; ++i)
	{
		//mark current date with []
		if(current_month == time_info->tm_mon && current_date == i)
		{
			char str[OFFSET];
			sprintf(str, "[%d]", i);
			//set color to red for weekends
			if((i + start_day - 1) % 7 == 0 || (i + start_day - 1) % 7 == 6)
				printf(ANSI_COLOR_RED	"%5s"	ANSI_COLOR_RESET, str);
			else
				printf("%5s", str);
		}
		else
		{
			//set color to red for weekends
			if((i + start_day - 1) % 7 == 0 || (i + start_day - 1) % 7 == 6)
				printf(ANSI_COLOR_RED	"%4d "	ANSI_COLOR_RESET, i);
			else
				printf("%4d ", i);
		}
		if(!((start_day + i) % DAYS_IN_WEEK))
			printf("\n");
	}
	printf("\n");

	return 0;
}


