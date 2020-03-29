#include "request.h"

#define SERVER "http://101.132.42.189:3000"
#define SESSIOID "96D82B3EFEB2D5647B26AA43F8295DDC"

char g_request_ip[100] = {0};
static const char *TAG="request.c";
int request_init(char *server)
{
    strcpy(g_request_ip, server);
    curl_global_init(CURL_GLOBAL_ALL);
    return 0;
}

int request_clean()
{
    curl_global_cleanup();
    return 0;
}


static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {    
    strncat(userp, buffer, size*nmemb);
    return size*nmemb;
}

char *request_server(request_type_t type, char *body)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char buf[1000000];
    char url[100] = {0};
    memset(buf, 0, 1000000);
    switch(type)
    {
        case REQ_INFO:
            sprintf(url, "%s/api/oc/sensors/get/info", g_request_ip);
            break;
        default:
            BAC_LOGE("undefined type: %d", type);
            break;

    }
    BAC_LOGV("request body: %s", body);
    if (curl)
    {
        struct curl_slist *headers=NULL; /* init to NULL is important */
        /* pass our list of custom made headers */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url);   // 指定url
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buf);           //回调函数写入数据指针
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);   //回调函数，可有可无
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
	if (res != CURLE_OK)
	{
		switch(res)
		{
			case CURLE_UNSUPPORTED_PROTOCOL:
				BAC_LOGE("不支持的协议,由URL的头部指定");
			case CURLE_COULDNT_CONNECT:
				BAC_LOGE("不能连接到remote主机或者代理");
			case CURLE_HTTP_RETURNED_ERROR:
				BAC_LOGE("http返回错误");
			case CURLE_READ_ERROR:
				BAC_LOGE("读本地文件错误");
			default:
				BAC_LOGE("返回值:%d",res);
		}
		return NULL;
    }

    BAC_LOGV("%s", buf);
    int len = strlen(buf);
    BAC_LOGV("res len[%d]", len);
    char *p = calloc(len + 1, 1);
    strcpy(p, buf);
    BAC_LOGV("exit request");
    return p;
}