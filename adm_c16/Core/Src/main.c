/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "asm_func.h"
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

ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

 ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;
uint8_t ch[6]="2";
uint8_t espacio[]="\n\r";

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */
void zeros (uint32_t * vector, uint32_t longitud);
void productoEscalar32 (uint32_t * vectorIn, uint32_t * vectorOut, uint32_t longitud, uint32_t escalar);
void productoEscalar16 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar);
void productoEscalar12 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar);
void filtroVentana10(uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitudVectorIn);
void pack32to16 (int32_t * vectorIn, int16_t * vectorOut, uint32_t longitud);
int32_t max (int32_t * vectorIn, uint32_t longitud);
void downsampleM (int32_t * vectorIn, int32_t * vectorOut, uint32_t longitud, uint32_t N);
void invertir (uint16_t * vector, uint32_t longitud);
void audioEco (int16_t * vectorIn,int16_t * vectorOut,uint32_t longitud);
void inicializarVector (int16_t * vector);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void PrivilegiosSVC (void)
{
    // Obtiene valor del registro de 32 bits del procesador llamado "control".
    // El registro guarda los siguientes estados:
    // bit 2: Uso de FPU en el contexto actual. Usado=1, no usado=0.
    // bit 1: Mapeo del stack pointer(sp). MSP=0, PSP=1.
    // bit 0: Modo de ejecucion en Thread. Privilegiado=0, No privilegiado=1.
    //        Recordar que este valor solo se usa en modo Thread. Las
    //        interrupciones siempre se ejecutan en modo Handler con total
    //        privilegio.
    uint32_t x = __get_CONTROL ();

    // Actividad de debug: Ver registro "control" y valor de variable "x".
    //__BKPT (0);

    x |= 1;
    // bit 0 a modo No privilegiado.
    __set_CONTROL (x);

    // En este punto se estaria ejecutando en modo No privilegiado.
    // Lectura del registro "control" para confirmar.
    x = __get_CONTROL ();

    // Actividad de debug: Ver registro "control" y valor de variable "x".
    //__BKPT (0);

    x &= ~1u;
    // Se intenta volver a modo Privilegiado (bit 0, valor 0).
    __set_CONTROL (x);

    // Confirma que esta operacion es ignorada por estar ejecutandose en modo
    // Thread no privilegiado.
    x = __get_CONTROL ();

    // Actividad de debug: Ver registro "control" y valor de variable "x".
    //__BKPT (0);

    // En este punto, ejecutando en modo Thread no privilegiado, la unica forma
    // de volver a modo privilegiado o de realizar cualquier cambio que requiera
    // modo privilegiado, es pidiendo ese servicio a un hipotetico sistema
    // opertivo de tiempo real.
    // Para esto se invoca por software a la interrupcion SVC (Supervisor Call)
    // utilizando la instruccion "svc".
    // No hay intrinsics para realizar esta tarea. Para utilizar la instruccion
    // es necesario implementar una funcion en assembler. Ver el archivo
    // asm_func.S.
    asm_svc ();

    // El sistema operativo (el handler de SVC) deberia haber devuelto el modo
    // de ejecucion de Thread a privilegiado (bit 0 en valor 0).
    x = __get_CONTROL ();

    // Fin del ejemplo de SVC
}
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
  DWT->CTRL |= 1 << DWT_CTRL_CYCCNTENA_Pos;

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  //---------------------EJERCICIO 1---------------------
  //uint32_t vector_in[4] = { 4, 5, 0, 7 };
  //asm_zeros (vector_in, 4);
  //-----------------------------------------------------

  //---------------------EJERCICIO 2---------------------
  //uint32_t vector32_in[4] = { 4, 5, 0, 7 };
  //uint32_t vector32_out[4] = {1,1,1,1};
  //asm_productoEscalar32(vector32_in,vector32_out,4,3);
  //-----------------------------------------------------

  //---------------------EJERCICIO 3---------------------
  //uint16_t vector16_in[4] = {2, 0, 4, 5};
  //uint16_t vector16_out[4] = {1,1,1,1};

  //asm_productoEscalar16(vector16_in,vector16_out,4,7);
  //-----------------------------------------------------

  //---------------------EJERCICIO 4---------------------
  //uint16_t vector16_in[4] = {2046,2047,2048,2049};
  //uint16_t vector16_out[4] = {1,1,1,1};

  //DWT->CYCCNT = 0;
  //asm_productoEscalar12(vector16_in,vector16_out,4,2);
  //const volatile uint32_t Ciclos = DWT->CYCCNT;
  //sprintf((char*) ch, "%d",(int) Ciclos);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 5, HAL_MAX_DELAY);

  //DWT->CYCCNT = 0;
  //productoEscalar12(vector16_in,vector16_out,4,2);
  //uint32_t Ciclos2 = DWT->CYCCNT;
  //sprintf((char*) ch, "%d",(int) Ciclos2);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 5, HAL_MAX_DELAY);
  //-----------------------------------------------------

  //---------------------EJERCICIO 5---------------------
  /*uint16_t vector16_in[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  uint16_t vector16_out[20] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  DWT->CYCCNT = 0;
  asm_filtroVentana10(vector16_in, vector16_out, 20);
  const volatile uint32_t Ciclos = DWT->CYCCNT;
  sprintf((char*) ch, "%d",(int) Ciclos);
  HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 6, HAL_MAX_DELAY);

  DWT->CYCCNT = 0;
  filtroVentana10(vector16_in, vector16_out, 20);
  const volatile uint32_t Ciclos2 = DWT->CYCCNT;
  sprintf((char*) ch, "%d",(int) Ciclos2);
  HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 6, HAL_MAX_DELAY);*/
  //-----------------------------------------------------

  //---------------------EJERCICIO 6---------------------
  //int32_t vector32_in[8] = {1000,-7,65535,-65535,65536,-65536,4000000,-4000000};
  //int16_t vector16_out[8] = {1,1,1,1,1,1,1,1};

  //asm_pack32to16 (vector32_in, vector16_out, 8);
  //-----------------------------------------------------

  //---------------------EJERCICIO 7---------------------
  //int32_t vector32_in[4] = {4,-7,10,-22};
  //int32_t posicion;

  //posicion = asm_max (vector32_in, 4);
  //sprintf((char*) ch, "%d",(int) posicion);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 5, HAL_MAX_DELAY);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&espacio, 3, HAL_MAX_DELAY);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 5, HAL_MAX_DELAY);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&espacio, 3, HAL_MAX_DELAY);
  //-----------------------------------------------------

  //---------------------EJERCICIO 8---------------------
  //int32_t vector32_in[8] = {1,2,3,4,5,6,7,8};
  //int32_t vector32_out[8] = {1,1,1,1,1,1,1,1};

  //asm_downsampleM (vector32_in, vector32_out,8,3);
  //-----------------------------------------------------

  //---------------------EJERCICIO 9---------------------
 // uint16_t vector32_in[5] = {1,2,3,4,5};

  //asm_invertir (vector32_in, 5);
  //-----------------------------------------------------

  //---------------------EJERCICIO 10---------------------
  int16_t vector_in[4096] = {};
  int16_t vector_out[4096] = {};

  inicializarVector(vector_in);

  DWT->CYCCNT = 0;
  asm_audioEco (vector_in,vector_out,4096);
  const volatile uint32_t Ciclos3 = DWT->CYCCNT;
  //sprintf((char*) ch, "%d",(int) Ciclos3);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 6, HAL_MAX_DELAY);

  DWT->CYCCNT = 0;
  audioEco (vector_in,vector_out,4096);
  const volatile uint32_t Ciclos4 = DWT->CYCCNT;
  //sprintf((char*) ch, "%d",(int) Ciclos4);
  //HAL_UART_Transmit (&huart3, (uint8_t *)&ch, 6, HAL_MAX_DELAY);
  //-----------------------------------------------------

  PrivilegiosSVC ();

  const uint32_t Resultado = asm_sum (5, 3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void zeros (uint32_t * vector, uint32_t longitud){
    for (uint32_t i = 0; i<longitud;i++){
        vector[i]=0;
    }
}

void productoEscalar32 (uint32_t * vectorIn, uint32_t * vectorOut, uint32_t longitud, uint32_t escalar){
        for (uint32_t i = 0; i<longitud;i++){
        vectorOut[i]=escalar*vectorIn[i];
    }
}

void productoEscalar16 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar){
        for (uint32_t i = 0; i<longitud;i++){
        vectorOut[i]=escalar*vectorIn[i];
    }
}

void productoEscalar12 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar){
        uint16_t producto;
        for (uint32_t i = 0; i<longitud;i++){
        producto=escalar*vectorIn[i];
            if (producto > 4095){
                vectorOut[i]=4095;
            }
            else {
                vectorOut[i]=producto;
            }
    }
}

void filtroVentana10(uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitudVectorIn){
	int32_t ind_max=0;
	int32_t ind_min=0;
	uint32_t suma;
	uint32_t cantidad=0;

	for (uint32_t i=0;i<longitudVectorIn;i++){
		suma=0;
		cantidad=0;
		ind_max=i+2;
		if (ind_max>=longitudVectorIn){
			ind_max=longitudVectorIn-1;
		}
		ind_min=i-2;
		if (ind_min<0){
			ind_min=0;
		}
		for (uint32_t j=ind_min;j<=ind_max;j++){
			cantidad++;
			suma=suma+vectorIn[j];
		}
		vectorOut[i]=suma/cantidad;
	}
}

void pack32to16 (int32_t * vectorIn, int16_t * vectorOut, uint32_t longitud){
	for (uint32_t i = 0; i<longitud;i++){
	vectorOut[i]=(vectorIn[i]>>16);
	}
}

int32_t max (int32_t * vectorIn, uint32_t longitud){
	int32_t maximo = vectorIn [0];
	int32_t indice = 0;

	for (uint32_t i = 0; i<longitud;i++){
		if (vectorIn[i]>maximo){
			maximo= vectorIn[i];
			indice=i;
		}
	}
	return indice;
}

void downsampleM (int32_t * vectorIn, int32_t * vectorOut, uint32_t longitud, uint32_t N){
	int32_t indice=1;

	for (uint32_t i=0;i<longitud;i++){

		if(indice < N){
			vectorOut[i]=vectorIn[i];
			indice++;
		}
		else{
			vectorOut[i]=0;
			indice=1;
		}
	}
}

void invertir (uint16_t * vector, uint32_t longitud){
	uint16_t aux_in=0;
	uint16_t aux_out=0;
	uint32_t j=longitud/2;
	uint32_t indice;

	for (uint32_t i=0;i<j;i++){
		indice=longitud-1-i;
		aux_in=vector[indice];
		aux_out=vector[i];
		vector[i]=aux_in;
		vector[indice]=aux_out;
	}
}

void audioEco (int16_t * vectorIn,int16_t * vectorOut,uint32_t longitud){
	int16_t j=0;

	for (int16_t i=0; i<longitud;i++){
		if(i<882){
			vectorOut[i]=vectorIn[i];
		}
		else{
			j=i-882;
			vectorOut[i]=vectorIn[i]+vectorIn[j]/2;
		}
	}

}

void inicializarVector (int16_t * vector){

	for (int16_t i=0; i<4096;i++){
			vector[i]=(i+1)*2;
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
