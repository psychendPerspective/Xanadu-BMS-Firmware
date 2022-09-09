#include "driverHWRTC.h"

RTC_HandleTypeDef driverHWRTCHandle;

void driverHWRTCInit(void)
{
    /** Initialize RTC Only
     */
    driverHWRTCHandle.Instance = RTC;
    driverHWRTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    driverHWRTCHandle.Init.AsynchPrediv = 127;
    driverHWRTCHandle.Init.SynchPrediv = 255;
    driverHWRTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    driverHWRTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    driverHWRTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&driverHWRTCHandle) != HAL_OK)
    {
        Error_Handler();
    }


}


void driverHWRTCSetTime(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    /** Initialize RTC and set the Time and Date
     */
    sTime.Hours = 0x02;
    sTime.Minutes = 0x50;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if(HAL_RTC_SetTime(&driverHWRTCHandle, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
    sDate.Month = RTC_MONTH_SEPTEMBER;
    sDate.Date = 0x0A;
    sDate.Year = 0x22;

    if(HAL_RTC_SetDate(&driverHWRTCHandle, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */
    HAL_RTCEx_BKUPWrite(&driverHWRTCHandle,RTC_BKP_DR1, 0x32F2);

    /* USER CODE END RTC_Init 2 */
}


void driverHWRTCGetCurrentTime(void)
{
    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&driverHWRTCHandle, &gTime, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&driverHWRTCHandle, &gDate, RTC_FORMAT_BIN);
}