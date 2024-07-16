/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "dbger.h"
#include "udp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void udpClient_connect(void);
static void udpClient_send(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t is_udp_connect = 0;
static uint32_t counter = 0;
static struct udp_pcb *upcb;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	LOG_DBG("\n\n\n\n\n/******** STM32V6 Ethernet(DM9161) Demo ********/\n");
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */
	//udpClient_connect();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		MX_LWIP_Process();
		
		if(is_udp_connect) {
			static uint32_t last_tick = 0;
			while(HAL_GetTick() - last_tick > 2000) {
				last_tick = HAL_GetTick();
				udpClient_send();
			}
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void udpClient_send(void)
{
  struct pbuf *txBuf;
	err_t err = ERR_OK;
  char data[100];

  int len = sprintf(data, "sending UDP client message %d", counter);
	LOG_DBG("udp client send \"%s\" to server\n", data);

  /* allocate pbuf from pool*/
  txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

  if (txBuf != NULL)
  {
    /* copy data to pbuf */
    err = pbuf_take(txBuf, data, len);
		if(err != ERR_OK) {
			LOG_ERR("pbuf_take() err[%d]\n", err);
		} else {
			/* send udp data */
			err = udp_send(upcb, txBuf);
			if(err != ERR_OK) {
				LOG_ERR("udp_send() err[%d]\n", err);
			}
		}
    /* free pbuf */
    pbuf_free(txBuf);
  }
}

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	/* Copy the data from the pbuf */
	//strncpy (buffer, (char *)p->payload, p->len);
	((char *)p->payload)[p->len] = 0;
	LOG_DBG("recv from [%s:%d](Len:%d): %s\n", ipaddr_ntoa(addr), port, p->len, (char*)p->payload);
	/*increment message count */
	counter++;
	/* Free receive pbuf */
	pbuf_free(p);
}

void udpClient_connect(void)
{
	err_t err;
	extern struct netif gnetif;

	/* 1. Create a new UDP control block  */
	upcb = udp_new();
	if(upcb == NULL) {
		LOG_ERR("udp_new err\n");
		return;
	}
	/* Bind the block to module's IP and port */
	ip_addr_t myIPaddr;
	IP_ADDR4(&myIPaddr, 192, 168, 0, 100);
	err = udp_bind(upcb, &myIPaddr, 8);
	//err = udp_bind(upcb, &gnetif.ip_addr, 8);
	if(err != ERR_OK) {
		LOG_ERR("udp client udp_bind() err[%d]\n", err);
	} else {
		LOG_DBG("udp client bind %s:%d\n", ip4addr_ntoa(&myIPaddr), 8);
	}

	/* configure destination IP address and port */
	ip_addr_t DestIPaddr;
	IP_ADDR4(&DestIPaddr, 192, 168, 0, 102);
	err = udp_connect(upcb, &DestIPaddr, 9);
	if (err == ERR_OK) {
		LOG_DBG("udp client conn with server[192.168.0.102:9]\n");
		/* 2. Send message to server */
		//udpClient_send ();
		/* 3. Set a receive callback for the upcb */
		udp_recv(upcb, udp_receive_callback, NULL);
		is_udp_connect = 1;
	} else {
		LOG_ERR("udp client conn with server[192.168.0.102:9] err[%d]\n", err);
	}
}

void udpClient_disconnect(void)
{
	if(upcb) {
		udp_disconnect(upcb);
		udp_remove(upcb);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
