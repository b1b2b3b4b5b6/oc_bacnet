## bacnet协议栈使用

示例：https://git.dev.tencent.com/cesc_mu/oc_bacnet.git

以下用oc senser的bacnet服务端来说明协议栈使用方式



## 组成：

### bacnet-stack

官方bacnet-stack源码，包括以下文件夹

- ports
- src
- include
- main/handler

源码不可改动



### 相关对象定义部分：

- 声明object_t对像

- 声明device_t对像

- 实现object_t对象方法

文件或文件夹：

- main/object



### device_t设备数据刷新部分

- 定义全局设备数据数组（device_t）
- 刷新设备数据

文件或文件夹：

- main/*.c



### 对象定义部分

#### main/object/object.h

##### 定义device_t对象

对象中除了`object_id`字段不可改变，其他属性可自行定义，不做限制，需要用户自己决定如何处理数据，以下为示例

```c
typedef struct {
    int object_id;//对象相对id，与对象偏移量相加得到对象实际id,字段名不可修改
    int people_num;//有人无人，有人 1， 无人0，自定义
    status_change_t status_change;//状态改变标志结构体，自定义
    char mac[30];//mac地址字符串buf，自定义
    char upload_time[100];//更新时间字符串buf，自定义
    char location[100];//地理位置字符串buf，自定义
}device_t;
```

​		

##### 定义object_t对象

object_t 对象会被bacnet-stack所使用，必须实现相关的方法或函数

```c

typedef struct {
    //对象自定类型（自定义枚举，仅作为不同object_t之间的区分，无实际意义），用户初始化设置
    int device_type;
    //对象类型（必须使用bacnet协议栈的定义），用户初始化设置
    int object_type;
    //对象id偏移，对象的id为对象为相对地址（从服务器获取）+对象id偏移（自定义），用户初始化设置
    long object_offset;
    
    /*绑定的设备数组，在device_t对象数据刷新部分定义
    数组内device的个数决定了bacnet-stack返回的对象个数   
    数组中的每个device会被当做参数传入下面定义的方法中，方法如何处理device中的数据和返回，由使用者决定*/
    device_t *device_list;//用户初始化设置
    
    //对象总数，内部参数，用户不关心，初始化时置为0
    long object_count;

    /*以下方法或函数必须定义*/

    //被cov程序调用，检查设备状态是否发生改变，改变时返回1，否则返回0
    int (*check_sensor_change)(device_t *);
    //被cov程序调用，清楚设备状态位，固定返回0
    int (*clear_sensor_change)(device_t *);
    //被协议栈object对象调用，获取设备名称，参数2传入待填充字符串buf指针，固定返回0
    int (*get_device_name)(device_t *, char *);
    //被协议栈object对象调用，获取设备描述，参数2传入待填充字符串buf指针，固定返回0
    int (*get_device_description)(device_t *, char *);
    //被协议栈object对象调用，获取设备对象值，返回整数值
    int (*get_device_value)(device_t *);
    //被协议栈object对象调用，获取设备是否超时，超时返回1，否则返回0
    int (*get_device_timeout)(device_t *);
    //被协议栈object对象调用，获取设备上次数据更新时间，参数2传入待填充字符串buf指针，固定返回0
    int (*get_device_upload_time)(device_t *, char *);
    //被协议栈object对象调用，设置对象值，固定返回0
    int (*set_device_value)(device_t *), 

}object_t;
```

##### 		

#### main/object/object.c

##### 实现object_t中相关方法并填充至g_object_list[]

```c
int check_sensor_change_people(device_t * device)
{
    if(device->status_change.people == 1)
        return 1;
    else
        return 0;
}

int clear_sensor_change_people(device_t * device)
{
    device->status_change.people = 0;
    return 0;
}


int get_device_name_people(device_t * device, char *name)
{
    sprintf(name, "%s", device->location);  
    return 0;
}

int get_device_description(device_t * device, char *str)
{
    sprintf(str, "%s", device->mac);
    return 0;    
}

int get_device_value_people(device_t *device)
{
	return device->people_num;
}

int get_device_timeout(device_t * device)
{
        return 0;
}

int get_device_upload_time(device_t * device, char *str)
{
    strcpy(str, device->upload_time);
    return 0; 
}


object_t g_object_list[] = {
    {
        INFO,//INFO为自定义枚举类型
        OBJECT_ANALOG_INPUT,//模拟输入设备
        1000,//对象id偏移
        g_device_list,//绑定的设备数组，在device_t对象数据刷新部分定义
        0,//对象总数，初始化时总是设为0，实际会变成g_device_list中设备的数量
        
        check_sensor_change_people,
        clear_sensor_change_people,
        get_device_name_people,
        get_device_description,
        get_device_value_people,
        get_device_timeout,
        get_device_upload_time,
        NULL,//对象是输入设备，所以没有set_device_value方法
    },
    
    {
        PEOPLE,//PEOPLE为自定义枚举类型
        OBJECT_ANALOG_INPUT,//模拟输入设备
        1000,//对象id偏移
        g_device_list,//绑定的设备数组，在device_t对象数据刷新部分定义
        0,//对象总数，初始化时总是设为0，实际会变成g_device_list中设备的数量
        
        check_sensor_change_people,
        clear_sensor_change_people,
        get_device_name_people,
        get_device_description,
        get_device_value_people,
        get_device_timeout,
        get_device_upload_time,
        NULL,//对象是输入设备，所以没有set_device_value方法
    },
	
    //仅作为数组结束判断
    {
        -1,
        0,
        0,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

```



### device_t对象数据刷新部分

- 定义devict_t类型的g_device_list数组，用于容纳实时刷新的数据和被object_t对象绑定
- 负责刷新g_device_list数组中的数据，包括元素的个数和指针所指对象的内容
- 如果数据发生改变，则需要通过某种方法（现在的做法是在device_t中定义状态标志位）使得int (*check_sensor_change)(device_t *)返回1，并且bacnet-stack 调用int (*clear_sensor_change)(device_t *)可以清除之前的置位，使得check_sensor_change返回1的次数有限。

##### 注：

- 设备数据的获取和解析并没有说明，因为不同场景，需求下的解决方法不会一样。
- 目前使用的是http请求数据（curl库），在get_data.c中解析数据并刷新至g_device_list



### g_device_list和object_t关系

- object_t对象会绑定一个g_device_list数组，数组内元素的个数（非NULL连续device个数）会被赋值到object_t中object_count值中
- bacnet-stack会将g_device_list中的device当做参数传入object_t的方法中，并且只关心返回值，所以device中的数据和object_t方法相对应即可，不做格式要求
- 多个不同的object_t对象可以绑定同一个g_device_list，只要object_t的device_type字段不同即可
- 不同object_t的对象id偏移要保证不重叠，如object_a定义为1000，另外一个object_b定义为2000。那么g_device_list中每个device的object_id字段值不能大于1000， 否则偏移量+object_id>2000，可能与object_b中的对象id发生重叠



### COV？

- cov是bacnet-stack中的一个机制，旨在数据发生改变时，主动将数据发送至订阅的服务端
- bacnet-stack会重复调用check_sensor_change来检查数据是否发生变化，返回1是发生变化，bacnet-stack会发送最新的数据，然后调用clear_sensor_change来清楚变化标志位，使得下次调用check_sensor_change返回0
- 目前cov机制运行在一个单独的线程内（cov_task），需要注意的是cov线程会访问device指针所指向的数据，所以，删除deivce数据可能会导致异常指针，推荐删除device时将cov_task停止（上锁），修改后启动cov_task（解锁），目前的上锁解锁函数在main/get_data.c中定义



### 编译流程（linux）

修改main/common.h中宏定义来确定日志输出等级

- 在主目录下make all
- 进入main目录，执行make all，生成bacserv可执行文件
- ./bacserv执行（参数的传入与默认值自行编写）



### 自启动

使用systemd系统，修改service中内容