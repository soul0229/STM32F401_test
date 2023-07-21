/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include <stdio.h>
#include <string.h>
//void SDFileTestWrite(void);
//void SDFileTestRead(void);
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
//    retUSER = f_mount(&USERFatFS,USERPath,1);//????A
//    if(retUSER == FR_NO_FILESYSTEM)//??????????????????
//    {
//				BYTE work[_MAX_SS];
//        retUSER = f_mkfs(USERPath,FM_FAT|FM_SFD,0,work,sizeof(work));
//        if(retUSER == FR_OK)
//        {
//            retUSER = f_mount(NULL,USERPath,1);//????,?????
//            retUSER = f_mount(&USERFatFS,USERPath,1);//??
//            printf("formmed successed, retUSER=%d\r\n",retUSER);
//        }
//        else{printf("formmed error retUSER=%d\r\n",retUSER);}//?????
//    }
//    else if(retUSER != FR_OK){printf("f_mont error, retUSER=%d\r\n",retUSER);}
//    else{printf("f_mont successed\r\n");}//????
//		SDFileTestWrite();
//		SDFileTestRead();
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
//FIL  fpSD;
//void SDFileTestWrite(void)
//{
//    FRESULT res_sd;
//    UINT fnum;/* ???????? */
//    char string[100];
//    signed int ByteNum = 0;

//    memset(string,0,sizeof(string));
//    sprintf(string,"%s%s.xls",USERPath,"Test");
//    res_sd = f_open(&fpSD, string,FA_CREATE_ALWAYS | FA_WRITE );
//    if(res_sd != FR_OK){printf("Failed to create file! %d\r\n",res_sd);}
//    sprintf(string,"Vreal\tA1\tA2\n");
//    ByteNum = strlen(string);
//    res_sd=f_write(&fpSD,string,ByteNum,&fnum);
//    res_sd = f_close(&fpSD);
//    if(res_sd != FR_OK){printf("Error:File closure Exception!  %d\r\n", res_sd);}
//    else{printf("SDFileTestWrite ok!\r\n");}
//}

//void SDFileTestRead(void)
//{
//    FRESULT res_sd;
//    char string[100];
//    uint32_t line = 0;

//    memset(string,0,sizeof(string));
//    sprintf(string,"%s%s.gif",USERPath,"gif_test3");
//		printf("%s\r\n",string);
//    res_sd = f_open(&fpSD, "gif_test3.gif", FA_OPEN_EXISTING | FA_READ);
//    if(res_sd != FR_OK){goto LoadFail;}
//    line = 0;

//    while(!(f_eof(&fpSD)))
//    {
//        memset(string,0,sizeof(string));
//        f_gets(string,sizeof(string),&fpSD);
//        if(strlen(string) == 0){break;}
//        ++line;
//        printf("line:%d %s\r\n",line,string);
//        //sscanf(string,"%f\t%f\t%f\n",&Vreal[*pNum],&Va1[*pNum],&Va2[*pNum]);//??????????
//    }
//    res_sd = f_close(&fpSD);
//    if(res_sd != FR_OK){printf("Error:Load File closure Exception!\r\n");}
//    printf("SDFileTestRead ok\r\n");
//    return;
//    LoadFail:
//    {
//      printf("Load Fail:%s\r\n",string);
//    }
//}

/* USER CODE END Application */
