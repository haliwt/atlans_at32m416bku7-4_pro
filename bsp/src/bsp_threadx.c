#include "bsp.h"

#define MAIN_STACK_SIZE         1024
#define DEMO_QUEUE_SIZE         10
#define PARSE_STACK_SIZE        2048//1024
#define BROADCAST_STACK_SIZE    512
#define KEY_STACK_SIZE           512

/* --- 1. 外部变量或宏定义（根据你实际的工程修改） --- */
//#define BROADCAST_STACK_SIZE    1024        // 广播线程栈大小：1024 字节
#define BROADCAST_PRIORITY      15          // 线程优先级（根据系统架构自行调整）


/* --- 3. 量产关键点：动态适配底层 CAN FD 结构体的 FIFO 内存池 --- */
#define CAN_RX_QUEUE_MAX_PACKETS    1    // 队列深度设为 16 帧，足以应对短时爆发的密集总线流
#define PACKET_WORDS_SIZE           ((sizeof(can_rxbuf_type) + sizeof(ULONG) - 1) / sizeof(ULONG))



/* --- 2. 定义 ThreadX 线程组件（全局静态分配） --- */

//static uint8_t   broadcast_stack[BROADCAST_STACK_SIZE]; 

/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;
//TX_THREAD               thread_4;
//TX_THREAD               thread_5;
//TX_QUEUE                queue_0;
//TX_SEMAPHORE            semaphore_0;
//TX_SEMAPHORE            can_rx_semaphore;

/* 声明 ThreadX 消息队列控制块 */
TX_QUEUE can_rx_queue;




TX_EVENT_FLAGS_GROUP    event_flags_0;

/* Define the counters used in the demo application...  */

//ULONG                   thread_0_counter;
//ULONG                   thread_1_counter;
//ULONG                   thread_1_messages_sent;
//ULONG                   thread_2_counter;
//ULONG                   thread_2_messages_received;
//ULONG                   thread_3_counter;
//ULONG                   thread_4_counter;
//ULONG                   thread_5_counter;


/* Define the thread stacks.  */


uint8_t                   thread_0_stack[MAIN_STACK_SIZE];
uint8_t                   thread_1_stack[PARSE_STACK_SIZE];
uint8_t                   thread_2_stack[BROADCAST_STACK_SIZE];
uint8_t 				  thread_3_stack[KEY_STACK_SIZE];
//UCHAR                   thread_4_stack[DEMO_STACK_SIZE];
//UCHAR                   thread_5_stack[DEMO_STACK_SIZE];


/* Define the queue area.  */

//UCHAR         queue_0_area[DEMO_QUEUE_SIZE*sizeof(ULONG)];

/* 声明解析线程的控制块与堆栈 */
TX_THREAD can_parse_thread;

uint8_t parse_thread_stack[PARSE_STACK_SIZE];




/* Define thread prototypes.  */

void  thread_main_entry(ULONG thread_input);
void  can_parse_thread_entry(ULONG thread_input);

void  can_broadcast_thread_entry(ULONG thread_input);
void  key_thread_entry(ULONG thread_input);
//void    thread_2_entry(ULONG thread_input);
//void    thread_3_and_4_entry(ULONG thread_input);

/* 静态分配 ULONG 数组作为队列底层的物理连续内存块 */
static ULONG        can_queue_memory_pool[CAN_RX_QUEUE_MAX_PACKETS * PACKET_WORDS_SIZE];


//void    can_parse_thread_entry(ULONG thread_input);

/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数,在 ThreadX 中，数字越小优先级越高。
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无⚠️ 核心细节：
*             ThreadX 队列传输的单位是 4 字节（1个 ULONG）。
*             如果你的 CAN 报文结构体总大小是 72 字节，这里的 PACKET_WORDS_SIZE 必须传入 72 / 4 = 18。
*********************************************************************************************************
*/
void tx_application_define(void *first_unused_memory)
{

     /* 1. 优先创建用于中断与任务同步的“信号量” */
   // tx_semaphore_create(&can_rx_semaphore, "CAN_RX_Semaphore", 0);
	//tx_queue_create(&can_rx_queue, "can_rx_queue", TX_1_ULONG, queue_0_area, DEMO_QUEUE_SIZE*sizeof(ULONG));
     /* 1. 【修复】安全创建 CAN FD 专用消息队列，传入正确的结构体字长与独立存储池 */
        /* 1. 安全创建唯一的 CAN FD 原生消息队列 */
    /* 严格按照结构体计算出来的 ULONG 尺寸搬运，彻底规避内存越界覆写死机隐患 */
    tx_queue_create(&can_rx_queue,                       //队列控制块指针
                    "CAN FD RX FIFO Queue",              //队列名称
                    PACKET_WORDS_SIZE,                   //单条消息的字长（32位字，即 ULONG 个数）。
                    can_queue_memory_pool,               //队列内存缓冲区首地址
                    sizeof(can_queue_memory_pool));      //内存缓冲区的总字节数



	 /* Create the main thread.  */
	tx_thread_create(&thread_0,           /* 任务控制块地址 */   
					 "thread 0",          /* 任务名 */
					 thread_main_entry,      /* 启动任务函数地址 */
					 0,					  /* 传递给任务的参数 */
                     thread_0_stack,      /* 堆栈基地址 */
                     MAIN_STACK_SIZE,     /* 堆栈空间大小 */  
                     2,                   /* 任务优先级*/
                     2,                   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,    /* 不开启时间片 */
                     TX_AUTO_START);      /* 创建后立即启动 */

    /* Create threads 1 and 2. These threads pass information through a ThreadX
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, 					/* 任务控制块地址 */   
                     "thread 1",            		/* 任务名 */
                     can_parse_thread_entry,        /* 启动任务函数地址 */
                     1,                     		/* 传递给任务的参数 */
                     thread_1_stack,        		/* 堆栈基地址 */
                     PARSE_STACK_SIZE,       		/* 堆栈空间大小 */  
                     1,                    			/* 任务优先级*/
                     1,                    			/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,                     		/* 不开启时间片 */
                     TX_AUTO_START);        		/* 创建后立即启动 */



    tx_thread_create(&thread_2,             			/* 任务控制块地址 */                         
		             "thread 2",            			/* 任务名 */
		             can_broadcast_thread_entry,        /* 启动任务函数地址 */
		             2,                     			/* 传递给任务的参数 */
                     thread_2_stack,        /* 堆栈基地址 */
                     BROADCAST_STACK_SIZE,       /* 堆栈空间大小 */  
                     3,                    /* 任务优先级*/
                     3,                    /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,                     /* 不开启时间片 */
                     TX_AUTO_START);         /* 创建后立即启动 */



	   tx_thread_create(&thread_3,          	/* 任务控制块地址 */                         
		             "thread_3",           		/* 任务名 */
		             key_thread_entry,        	/* 启动任务函数地址 */
		             0,                         /* 传递给任务的参数 */
                     thread_3_stack,            /* 堆栈基地址 */
                     KEY_STACK_SIZE,       		/* 堆栈空间大小 */  
                     0,                    			/* 任务优先级*/
                     0,                           	/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,              /* 不开启时间片 */
                     TX_AUTO_START);                /* 创建后立即启动 */

}
	 
#if 0


	 /* 创建状态广播发送线程 */
    tx_thread_create(&broadcast_thread,             // 线程控制块指针
                     "Status Broadcast Thread",     // 线程名称（Debug调试用）
                     broadcast_thread_entry,        // 线程入口函数地址
                     0,                             // 传递给入口函数的参数(thread_input)
                     broadcast_stack,               // 线程栈起始地址
                     BROADCAST_STACK_SIZE,          // 线程栈大小（字节）
                     2,            // 线程优先级
                     2,            // 禁用抢占阈值（与优先级一致表示允许抢占）
                     TX_NO_TIME_SLICE,              // 时间片（此处用不到，填无）
                     TX_AUTO_START);                // 创建后自动启动运行

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

#endif 


/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */
void thread_main_entry(ULONG thread_input)
{
     /* ARMCC V5 铁律：局部变量必须在最顶部 */
	static uint8_t dc_first_flag=0 ;

    (void)thread_input; // 消除未引用警告
	
 
     /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {
        wdt_counter_reload();

		if(gpro_t.power_on_flag == true){
        /* 维持 1 秒（1000ms）执行周期，假设 1 Tick = 1ms */
        /* 只有在开机状态、且有功能正在运行，并且倒计时大于 0 时才处理 */
        power_main_power_handler();
		LED_POWER_TOGGLE();
		led_1_blink_1s();

		}
		else{
		  gpro_t.dc_power = 0;
		  if(dc_first_flag ==0){
			 dc_first_flag++;

             buzzer_sound_on();
		  }
          LED_POWER_OFF() ;
		  LED_1_OFF();
		  uvc_led_off();


		}
	
		tx_thread_sleep(20);//10ms * 20 = 200ms .
    }
}
/**
 * @brief  :  entry 1
 * @note    
 * @param   None
 * @retval  None
 */
 /* 2. CAN 广播线程入口函数 */
can_rxbuf_type local_rx_packet; 
uint8_t frame_len;

void  can_parse_thread_entry(ULONG thread_input)
{
      /* 声明线程私有的报文接收实体，确保线程安全 */
    //can_rxbuf_type local_rx_packet; 
	//uint8_t frame_len;


    while(1){
         /* 1. 阻塞等待队列中的数据。
         * 如果队列为空，线程挂起不占 CPU；一旦中断投递了新报文，线程立即被唤醒。
         */
         if(tx_queue_receive(&can_rx_queue, &local_rx_packet, TX_WAIT_FOREVER)== TX_SUCCESS){

              /* 2. 从收到的结构体中提取真实的字节长度并解析 */
               frame_len = (uint8_t)local_rx_packet.data_length; 
            
               protocol_parse_ctrl_frame(local_rx_packet.data, frame_len);
        }

    }

}

/**
 * @brief  :  entry 1
 * @note    
 * @param   None
 * @retval  None
 */
 /* 2. CAN 广播线程入口函数 */
void can_broadcast_thread_entry(ULONG thread_input)
{
     (void)thread_input; // 消除未引用警告
	while (1)
    {
        /* 按照协议打包当前系统的最新状态（功能、挡位、时间、温湿度、故障码） */
        
         broadcast_thread_handler();
        
        /* 使用 ThreadX 的精准延时，控制 500ms 周期 
         * 假设你的系统 Tick 是 1ms (100 ticks = 1秒，那么 500ms = 50 ticks)
         */
        tx_thread_sleep(50); 
    }
}

/**
 * @brief  :  KEY TASK
 * @note    
 * @param   None
 * @retval  None
 */
  uint8_t cur_power_pressed = 0;
     uint8_t cur_mode_pressed  = 0;
	 
void  key_thread_entry(ULONG thread_input)
{

 
    

    (void)thread_input;

    while (1) {
        /* 简单消抖：确认连续两次采样都为按下（低电平 RESET） */
       

        if (KEY_POWER_VALUE() == RESET ) {
             if(cur_power_pressed < 10 )//tx_thread_sleep(10); /* 消抖延时（按实际 tick 频率调整） */
             {
                cur_power_pressed ++;
             }
        }
		else if (KEY_MODE_VALUE() == RESET && gpro_t.power_on_flag == true) {
             if(cur_mode_pressed < 10){
			    cur_mode_pressed ++;
			  }
        }

        if(cur_power_pressed == 10){
			 cur_power_pressed ++;
           buzzer_sound_on();
           //gpro_t.power_on_flag = (gpro_t.power_on_flag == 0) ? 1 : 0;
         
		   if(gpro_t.power_on_flag == false )
             gpro_t.power_on_flag = true;
		   else 
		   	 gpro_t.power_on_flag = false;
		   
        }
		else if(KEY_POWER_VALUE() == SET){
		    cur_power_pressed =0;

         }

        
		

        /* 模式键：仅在开机状态下，按下瞬间触发一次 */
        if (cur_mode_pressed == 10){
			  cur_mode_pressed++ ;
                buzzer_sound_on();
                gpro_t.mode_counter_flag++;
                if (gpro_t.mode_counter_flag > 3) {
                    gpro_t.mode_counter_flag = 0;
                }
				
         }
		 else if(KEY_MODE_VALUE() == SET){
		 	cur_mode_pressed=0;

		 }
        
         

        tx_thread_sleep(2);
    
}


}

#if 0

void thread_1_entry(ULONG thread_input)
{

UINT    status;

   #if 0

    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {
	       status =  tx_semaphore_get(&can_rx_semaphore, TX_WAIT_FOREVER);
	
		   /* Check status.  */
		   if (status != TX_SUCCESS){
			   break;

		   }
		  /* --- 接收成功（获取信号量成功）的逻辑 --- */
        // 例如：打印调试信息、处理接收到的数据或更新状态标志
         // printf("Semaphore received successfully!\n");
    }

	#endif 
}







/**
 * @brief  :  
 * @note    
 * @param   None
 * @retval  None
 */
void thread_1_entry(ULONG thread_input)
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


/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

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

#endif 
