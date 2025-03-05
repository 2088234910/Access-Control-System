#include "Key.h"
#include "main.h"
#include "tim.h"

KSTATE Key_State;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(GPIO_Pin);
    HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
    
  /* USER CODE BEGIN Callback 1 */
  if(htim==(&htim2))
  {
      HAL_TIM_Base_Stop_IT(&htim2);
      if(HAL_GPIO_ReadPin(Key_UP_GPIO_Port,Key_UP_Pin)==0)
        Key_State = Key_UP;
      else if(HAL_GPIO_ReadPin(Key_DOWN_GPIO_Port,Key_DOWN_Pin)==0)
        Key_State = Key_DOWN;
      else if(HAL_GPIO_ReadPin(Key_MID_GPIO_Port,Key_MID_Pin)==0)
        Key_State = Key_MID;
  }
  /* USER CODE END Callback 1 */
}
