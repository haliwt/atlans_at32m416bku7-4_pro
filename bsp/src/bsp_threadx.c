#include "bsp.h"

#define DEMO_STACK_SIZE         1024
#define DEMO_QUEUE_SIZE         10
#define PARSE_STACK_SIZE        2048



/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;
TX_THREAD               thread_4;
TX_THREAD               thread_5;
TX_QUEUE                queue_0;
TX_SEMAPHORE            semaphore_0;
TX_SEMAPHORE            can_rx_semaphore;

TX_EVENT_FLAGS_GROUP    event_flags_0;

/* Define the counters used in the demo application...  */

ULONG                   thread_0_counter;
ULONG                   thread_1_counter;
ULONG                   thread_1_messages_sent;
ULONG                   thread_2_counter;
ULONG                   thread_2_messages_received;
ULONG                   thread_3_counter;
ULONG                   thread_4_counter;
ULONG                   thread_5_counter;


/* Define the thread stacks.  */

UCHAR                   thread_0_stack[DEMO_STACK_SIZE];
UCHAR                   thread_1_stack[DEMO_STACK_SIZE];
UCHAR                   thread_2_stack[DEMO_STACK_SIZE];
UCHAR                   thread_3_stack[DEMO_STACK_SIZE];
UCHAR                   thread_4_stack[DEMO_STACK_SIZE];
UCHAR                   thread_5_stack[DEMO_STACK_SIZE];


/* Define the queue area.  */

UCHAR                   queue_0_area[DEMO_QUEUE_SIZE*sizeof(ULONG)];

/* 声明解析线程的控制块与堆栈 */
TX_THREAD can_parse_thread;

uint8_t parse_thread_stack[PARSE_STACK_SIZE];



/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    can_parse_thread_entry(ULONG thread_input);

/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数,在 ThreadX 中，数字越小优先级越高。
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无
*********************************************************************************************************
*/
void tx_application_define(void *first_unused_memory)
{

     /* 1. 优先创建用于中断与任务同步的“信号量” */
    tx_semaphore_create(&can_rx_semaphore, "CAN_RX_Semaphore", 0);

    /* Create the main thread.  */
	tx_thread_create(&thread_0,           /* 任务控制块地址 */   
					 "thread 0",          /* 任务名 */
					 thread_0_entry,      /* 启动任务函数地址 */
					 0,					  /* 传递给任务的参数 */
                     thread_0_stack,      /* 堆栈基地址 */
                     DEMO_STACK_SIZE,     /* 堆栈空间大小 */  
                     1,                   /* 任务优先级*/
                     1,                   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,    /* 不开启时间片 */
                     TX_AUTO_START);      /* 创建后立即启动 */

    /* Create threads 1 and 2. These threads pass information through a ThreadX
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, 			/* 任务控制块地址 */   
                     "thread 1",            /* 任务名 */
                     thread_1_entry,        /* 启动任务函数地址 */
                     1,                     /* 传递给任务的参数 */
                     thread_1_stack,        /* 堆栈基地址 */
                     DEMO_STACK_SIZE,       /* 堆栈空间大小 */  
                     16,                    /* 任务优先级*/
                     16,                    /* 任务抢占阀值 */
                     4,                     /* 不开启时间片 */
                     TX_AUTO_START);        /* 创建后立即启动 */

    tx_thread_create(&thread_2,             /* 任务控制块地址 */                         
		             "thread 2",            /* 任务名 */
		             thread_2_entry,        /* 启动任务函数地址 */
		             2,                     /* 传递给任务的参数 */
                     thread_2_stack,        /* 堆栈基地址 */
                     DEMO_STACK_SIZE,       /* 堆栈空间大小 */  
                     16,                    /* 任务优先级*/
                     16,                    /* 任务抢占阀值 */
                     4,                     /* 不开启时间片 */
                     TX_AUTO_START);         /* 创建后立即启动 */

	   tx_thread_create(&can_parse_thread,          /* 任务控制块地址 */                         
		             "can_fd_parse_task",           /* 任务名 */
		             can_parse_thread_entry,        /* 启动任务函数地址 */
		             0,                             /* 传递给任务的参数 */
                     parse_thread_stack,            /* 堆栈基地址 */
                     PARSE_STACK_SIZE,       		/* 堆栈空间大小 */  
                     1,                    			/* 任务优先级*/
                     1,                           	/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,              /* 不开启时间片 */
                     TX_AUTO_START);                /* 创建后立即启动 */

    /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.
       An interesting thing here is that both threads share the same instruction area.  */
    tx_thread_create(&thread_3,              /* 任务控制块地址 */ 
    				 "thread 3",             /* 任务名 */
    				 thread_3_and_4_entry,    /* 启动任务函数地址 */
    				 3,                       /* 传递给任务的参数 */
                     thread_3_stack,          /* 堆栈基地址 */
                     DEMO_STACK_SIZE,         /* 堆栈空间大小 */  
                     8,                       /* 任务优先级*/
                     8,                        /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,         /* 不开启时间片 */
                     TX_AUTO_START);           /* 创建后立即启动 */

    tx_thread_create(&thread_4,               /* 任务控制块地址 */ 
		             "thread 4",               /* 任务名 */
		             thread_3_and_4_entry,     /* 启动任务函数地址 */
		             4,                         /* 传递给任务的参数 */
                     thread_4_stack,            /* 堆栈基地址 */
                     DEMO_STACK_SIZE,           /* 堆栈空间大小 */  
                     8,                         /* 任务优先级*/
                     8,                         /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,           /* 不开启时间片 */
                     TX_AUTO_START);             /* 创建后立即启动 */

    /* Create thread 5.  This thread simply pends on an event flag which will be set
       by thread_0.  */
    tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,
            thread_5_stack, DEMO_STACK_SIZE,
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Create the message queue shared by threads 1 and 2.  */
    tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, queue_0_area, DEMO_QUEUE_SIZE*sizeof(ULONG));

    /* Create the semaphore used by threads 3 and 4.  */
    tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Create the event flags group used by threads 1 and 5.  */
    tx_event_flags_create(&event_flags_0, "event flags 0");


}



/* Define the test threads.  */
/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */


/**
  * @brief  ThreadX 独立的 CAN FD 协议接收与业务解析任务 (线程)
  * @note   无数据时休眠，有数据时被激活，保证极致的实时响应与低功耗
  */
void can_parse_thread_entry(ULONG thread_input)
{
    while (1)
    {
        /* 1. 永久等待信号量（TX_WAIT_FOREVER），无报文时线程处于挂起状态，不占用 CPU */
        if (tx_semaphore_get(&can_rx_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            /* 2. 成功被中断唤醒，提取全局缓冲区 g_can_rx_buffer 中的硬件数据进行解析 */
            /* 转换枚举类型为具体的字节长度 */
            uint8_t frame_len = (uint8_t)g_can_rx_buffer.data_length; 
            
            /* 调用业务层协议解析函数 */
            protocol_parse_ctrl_frame(g_can_rx_buffer.data, frame_len);
        }
    }
}


void thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {
        wdt_counter_reload();
        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }
}


void    thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;

        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
    }
}


void    thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /* Get the semaphore with suspension.  */
        status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(2);

        /* Release the semaphore.  */
        status =  tx_semaphore_put(&semaphore_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_5_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_flags;


    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR,
                                                &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }
}


