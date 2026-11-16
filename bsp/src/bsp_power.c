#include "bsp.h"


static uint8_t get_max_fan_speed(void);


void power_on_off_handler(void)
{

   static uint8_t fan_max_value;
   
	if(g_care_status.power_disingfect_status == false  &&  g_care_status.power_dry_status == false \
		&& g_care_status.power_deodorize_status == false && g_care_status.power_mites_status == false){

        g_care_status.process_func = 0;
		g_care_status.dry_func_en = false;
		g_care_status.remain_deodorize_seconds = 0;
		g_care_status.remain_mites_seconds = 0;
		g_care_status.remain_disingfect_seconds = 0;
		g_care_status.remain_dry_seconds = 0;
		g_care_status.process_deodorize_fan_speed =0 ;
	    g_care_status.process_mites_fan_speed =0 ;
		g_care_status.process_disingfect_fan_speed =0 ;
		g_care_status.process_dry_fan_speed =0 ;

		   uvc_led_off(); 
		   PLASMA_OFF() ;
		   NEGATIVE_OFF();
		   DRY_OFF();
		   fan_stop();


		

	}  
    else{


	  switch(g_care_status.process_func){


	   case 0x01:

	//if(g_care_status.process_func  == FUNC_DEODORIZE){ // 0x01 FUNC_DEODORIZE) //除异味

       if(g_care_status.remain_deodorize_seconds >0){
            NEGATIVE_ON();

	        
            fan_set_gear(g_care_status.process_deodorize_fan_speed) ;       
	   }

	   if(g_care_status.remain_deodorize_seconds==0){
            g_care_status.deodorize_gear = 0;
			g_care_status.process_deodorize_fan_speed =0;
            fan_stop();
	        NEGATIVE_OFF();
	   }
	
	    g_care_status.process_func = 0xfe;

	  break;

	  case 0x02:

	   if(g_care_status.process_func  == FUNC_MITES){ // 0x02) // 除螨虫

       if(g_care_status.remain_mites_seconds >0){
            PLASMA_ON();

	        fan_set_gear(g_care_status.process_mites_fan_speed) ;       
	   }

	   if(g_care_status.remain_mites_seconds==0){
            g_care_status.mites_gear = 0;
			g_care_status.process_mites_fan_speed = 0;
            fan_stop();
	        PLASMA_OFF();
	   }

	  
	    g_care_status.process_func = 0xfe;

	  break;

	  case 0x03:

	 // if(g_care_status.process_func  == FUNC_DISINFECT){ // 0x03 消毒

       if(g_care_status.remain_disingfect_seconds >0){
            PLASMA_ON();
            uvc_led_on();
	        //fan_max_value = get_max_fan_speed();
            fan_set_gear(g_care_status.process_disingfect_fan_speed) ;       
	   }

	   if(g_care_status.remain_disingfect_seconds==0){
            g_care_status.disingfect_gear = 0;
			g_care_status.process_disingfect_fan_speed=0;
            fan_stop();
	        PLASMA_OFF();
			uvc_led_off();
	   }

	  //}
         g_care_status.process_func = 0xfe;
	   break;

	   case 0x04:

	  //if(g_care_status.process_func  == FUNC_DRY){ // 0x03 消毒

           if(g_care_status.remain_dry_seconds >0){
               DRY_ON();
               //fan_max_value = get_max_fan_speed();
               fan_set_gear(g_care_status.process_dry_fan_speed) ;       
	        }

	       if(g_care_status.remain_dry_seconds==0){
              g_care_status.dry_gear = 0;
			  g_care_status.process_dry_fan_speed =0;
			  DRY_OFF();
              fan_stop();
	       
	       }

	  //}
         g_care_status.process_func = 0xfe;
	  break;

    }
    	}
   }
}



/**
 * @brief  获取四个护理流程中风扇速度的最大值
 * @retval 最大风扇速度值
 */
static uint8_t get_max_fan_speed(void)
{
    uint8_t max_speed = g_care_status.process_deodorize_fan_speed;

    /* 依次与其余三个参数比较，找出最大值 */
    if (g_care_status.process_disingfect_fan_speed > max_speed) {
        max_speed = g_care_status.process_disingfect_fan_speed;
    }
    
    if (g_care_status.process_mites_fan_speed > max_speed) {
        max_speed = g_care_status.process_mites_fan_speed;
    }
    
    if (g_care_status.process_dry_fan_speed > max_speed) {
        max_speed = g_care_status.process_dry_fan_speed;
    }

    return max_speed;
}

