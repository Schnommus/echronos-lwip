#ifndef TIME_H
#define TIME_H

struct tm
{
    int   tm_sec;
    int   tm_min;
    int   tm_hour;
    int   tm_mday;
    int   tm_mon;
    int   tm_year;
    int   tm_wday;
    int   tm_yday;
    int   tm_isdst;
};

typedef long int time_t;

#endif
