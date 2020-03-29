#include "get_data.h"

const char *TAG = "get_data.c";

#define HEART_BEAT_TIME 60*10

#define DEVICE_MAX_COUNT 2000

#define REFRESH_PERIOD 2

#define EXEC_LOCATION "/usr/share/oc_bacnet/log"
device_t g_device_list[DEVICE_MAX_COUNT] = {0};
int g_device_list_count = 0;

int fresh_seq;
sem_t server_sem;
sem_t cov_sem;
FILE *g_log = NULL;

static void open_log()
{
    if(opendir(EXEC_LOCATION) == NULL)
    {
        if(mkdir(EXEC_LOCATION, 0777) != 0)
        {
            BAC_LOGE("mkdir "EXEC_LOCATION" fail");
            return;
        }
    }

    g_log = fopen(EXEC_LOCATION"/log.txt","w+");
    if(g_log == NULL)
        BAC_LOGE("open "EXEC_LOCATION"/log.txt fail");

}

static void print_info(char *str)
{
    BAC_LOGD("%s",str);
    char temp[100] = {0};
  
    for(int n = 0; n < g_device_list_count; n++)
    {
        device_t *device = &g_device_list[n];
        temp[strlen(temp) - 1] = 0;
        BAC_LOGD("id: %3d | people: %4d | people_flag: %1d | name: %s | mac: %s \r\n",
		device->object_id, device->people_num, device->status_change.people, device->location, device->mac);
        if(g_log != NULL)
        {
		fprintf(g_log, "id: %3d | people: %4d | people_flag: %1d | name: %s |  mac: %s \r\n",
		device->object_id, device->people_num, device->status_change.people, device->location, device->mac);
	}

    }
    if(g_log != NULL)
    {
	fseek(g_log, 0, SEEK_SET);
        fprintf(g_log, "\r\n\r\n");
        fflush(g_log);
    }

}

static char *get_request(char *mac)
{
    char *packet = NULL;
    char body[12000] = {0};
    char *temp;
    packet = request_server(REQ_INFO, "");
    BAC_LOGV("get_request start");
    if(packet == NULL)
    {
        BAC_LOGE("request falut");
        return NULL;
    }

    int err_code = -1;
    json_parse_object(packet, "errorCode", &err_code);
    if(err_code != 0)
    {
        char *message;
        json_parse_object(packet, "message", &message);
        BAC_LOGE("request error: %s", message);   
        free(message);
        free(packet);
        return NULL;      
    }

    char *data;
    json_pack_object_raw(packet, "data", &data);
    free(packet);  
    BAC_LOGV("get_request success");
    return data;
}   

static device_t *get_device_by_mac(char *mac)
{
    for(int n = 0; n < g_device_list_count; n++)
    {
        if(strcmp(g_device_list[n].mac, mac) == 0)
        return &g_device_list[n];
    }
    BAC_LOGE("no mac match: %s", mac);
    return NULL;
}

static device_t *get_device_by_object_id(int object_id)
{
    for(int n = 0; n < g_device_list_count; n++)
    {
        if(g_device_list[n].object_id == object_id)
        return &g_device_list[n];
    }
    BAC_LOGE("no object id match");
    return NULL;
}





static int parse_device(char *str, device_t *device)
{   
    int len;
    char *temp;
    BAC_LOGV("parse_device start");


    int id;
    json_parse_object(str, "bacnet", &temp);
    len = strlen(temp);
    sscanf(temp, "%d", &id);
    device->object_id = id;
    free(temp);
    BAC_LOGV("object_id: %d", device->object_id);


    int pre;
    pre = device->people_num;
    json_parse_object(str, "status", &(device->people_num));
    if(pre != device->people_num)
        device->status_change.people = 1;
    BAC_LOGV("people: %d", device->people_num);

    json_parse_object(str, "mac", &temp);
    len = strlen(temp);
    memcpy(device->mac, temp, len + 1); 
    free(temp);
    BAC_LOGV("mac: %s", device->mac);

    json_parse_object(str, "location", &temp);
    len = strlen(temp);
    memcpy(device->location, temp, len + 1); 
    free(temp);
    BAC_LOGV("location: %s", device->location);

    BAC_LOGV("parse_device finish");
    return 0;
}

static void assign_device(device_t *old, device_t *new)
{
	BAC_LOGV("new bacnet id: %d", new->object_id);
    memcpy(&new->status_change, &old->status_change, sizeof(status_change_t));
    if(old->people_num != new->people_num)
        new->status_change.people = 1;
   
    memcpy(old, new, sizeof(device_t));
}

static int rebuild_device_list()
{
    char *packet;
    long temp;
    char **devcies_str;
    device_t device_real = {0};
    for(int i = 0; i< DEVICE_MAX_COUNT; i++)
    {
        memset(&g_device_list[i], 0, sizeof(device_t));
    }

    packet = get_request("");
    if(packet == NULL)
    {
        BAC_LOGE("rebuild_device_list get fail");
        return -1;
    }
    temp = json_parse_object(packet, "", &devcies_str);
    free(packet);

    if(temp <= 0)
    {
        BAC_LOGE("rebuild_device_list parse fail or device num is 0");
        return -1;
    }
    g_device_list_count = 0;
    for(int n = 0; n < temp; n++)
    {
        parse_device(devcies_str[n], &device_real);
        free(devcies_str[n]);
        if(device_real.object_id >= 1)
        {
            assign_device(&g_device_list[g_device_list_count], &device_real);
            g_device_list[g_device_list_count].status_change.people = 1;
            g_device_list_count++;
        }
    }
    free(devcies_str); 
    int object_count = 0;
    for(;;)
    {
        object_t  *object = &g_object_list[object_count];
        if(object->device_type == -1)
            break;
        object->object_count = g_device_list_count;
        object_count++;
    }

    BAC_LOGI("g_device_list_count : %d", g_device_list_count);
	
    return 0;
}




//if post fail or device struct has changed
static int refresh_devices(char *macs)
{
    char *data = get_request(macs);
    long temp;
    char **devcies_str;
    device_t *device;
    bool restart_flag = false;
    int actual_count = 0;
    device_t device_real = {0};
    if(data == NULL)
    {
        BAC_LOGE("refresh_devices get fail");
        return -1;
    }

    BAC_LOGV("get_devices start");
    temp = json_parse_object(data, "", &devcies_str);
    free(data);

    for(int n = 0; n < temp; n++)
    {
        parse_device(devcies_str[n], &device_real);
        
        free(devcies_str[n]);

        if(device_real.object_id > 0)
        {
			device = get_device_by_object_id(device_real.object_id);
            if(device == NULL)
            {
                BAC_LOGE("handle undefined object id: %d, device struct has changed", device_real.object_id);
                restart_flag = true;
            }
            if(restart_flag == false)
            {
                actual_count++;
                assign_device(device, &device_real);
            }
        }
    }   
    free(devcies_str);

    if(actual_count != g_device_list_count)
    {
        BAC_LOGE("device conunt has changed, %d---->%d", g_device_list_count, actual_count);
        restart_flag = true;
    }
        
    if(restart_flag == true)
    {
        BAC_LOGE("rebuild device list!");
        server_lock();
        cov_lock();
        while(rebuild_device_list() != 0)
        {
            BAC_LOGE("fail retry");
            sleep(5);
        }
        cov_unlock();
        server_unlock();
        object_init();
    }

    print_info("refresh after");
    return 0;

}

void server_lock()
{
    int res = sem_wait(&server_sem);
    if(res != 0)
        BAC_LOGE("sem_wait fault");
}

void server_unlock()
{
    int res = sem_post(&server_sem);
    if(res != 0)
        BAC_LOGE("sem_post fault");
}

void cov_lock()
{
    int res = sem_wait(&cov_sem);
    if(res != 0)
        BAC_LOGE("sem_wait fault");
}

void cov_unlock()
{
    int res = sem_post(&cov_sem);
    if(res != 0)
        BAC_LOGE("sem_post fault");
}


// static void check_timeout()
// {
//     BAC_LOGD("check timeout");
//     struct timeval tv;
//     int temp;
//     gettimeofday(&tv, NULL);
//     long now_s = tv.tv_sec;
//     for(int n = 0; n < g_device_list_count; n++)
//     {
//         if((now_s - g_device_list[n].time_s) >= HEART_BEAT_TIME)
//             temp = 1;
//         else
//             temp = 0;
//         if(temp != g_device_list[n].time_out)
//         {
//             g_device_list[n].status_change.pir = 1;
//             g_device_list[n].status_change.light= 1;
//         }
//         g_device_list[n].time_out = temp;
//     }
// }

void *refresh_task(void *argc)
{
    while (1)
    {
        sleep(REFRESH_PERIOD);
         
        refresh_devices("");
        
        //check_timeout();
    }
    return NULL;
}
 
void get_data_init(char *path)
{
    char buf[100];
    if(path == NULL)
        strcpy(buf, "http://101.132.42.189:3000");
    else
        strcpy(buf, path);
    sem_init(&server_sem, 0, 1);
    sem_init(&cov_sem, 0, 1);
    BAC_LOGI("server path: %s", buf);
    request_init(buf);

    int peer = 3349;
    fresh_seq = msgget(peer, IPC_CREAT|0777);
    while((fresh_seq = msgget(888, IPC_CREAT|0777) <= 0))
    {
        BAC_LOGE("get fresh seq fail, retary %d", peer);
        peer++;
    }
    BAC_LOGI("create fresh seq success");

    while(rebuild_device_list() != 0)
    {
        BAC_LOGI("fail retry");
        sleep(10);
    }
    open_log();
    pthread_t thread_refresh;
    pthread_create(&thread_refresh, NULL, refresh_task, NULL);
}





