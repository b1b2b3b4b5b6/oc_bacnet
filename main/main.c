
#include "get_data.h"

void *restart_task(void *arg)
{
	sleep(60 * 60 * 24 * 5);
    for(;;)
    {
		struct tm *ptr;
        time_t lt;
        lt =time(NULL);
        ptr=localtime(&lt);
        if(ptr->tm_hour == 0 && ptr->tm_wday == 0 )
			exit(0);
        else
            sleep(60 * 20);
            
    }
}

int server(long int id);
int main(
    int argc,
    char *argv[])
{
    pthread_t restart;
    pthread_create(&restart, NULL, restart_task, NULL);
    char *path = NULL;
    if (argc == 2)
        path = argv[1];

    get_data_init(path);
    server(BAC_DEVICE_ID);
    return 0;
}

