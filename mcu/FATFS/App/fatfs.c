/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
UINT br;          // File R/W count
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
void SD_load(void)
{
    OLED_Clear();
    if(f_mount(&SDFatFS, SDPath, 1) == FR_OK)
    {
        if(f_open(&SDFile, "Font16cn.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK)    return;            
        else    OLED_ShowString(0, 16, "Font loading failed", OLED_8X16);
    }
    else    OLED_ShowString(0, 0, "SD mount failed", OLED_8X16);
    OLED_Update();
    HAL_Delay(1000);
}
void SD_unload(void)
{
    OLED_Clear();
    f_mount(0,SDPath,0);
    OLED_ShowString(10, 48, "SD unloading successfully", OLED_8X16);
    OLED_Update();
    HAL_Delay(1000);
}

/* �ֿ����� */
uint8_t Find_Chinese(uint8_t* p, uint8_t* buffer)
{
    FRESULT fre;
	uint8_t High8bit,Low8bit;
	High8bit=*p;
	Low8bit=*(p+1);
    
    fre = f_lseek(&SDFile,32*((High8bit-0x81)*190+Low8bit-0x41));
	if(fre == FR_OK)
    {
        f_read(&SDFile, buffer, 32, &br);
        return FR_OK;
    }
	return fre;	
}

/* USER CODE END Application */
