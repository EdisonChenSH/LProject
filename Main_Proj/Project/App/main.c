/* Includes ------------------------------------------------------------------*/
#include "Rs485.h"
#include "Process.h"
#include "rec.h"
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
  ProcessInit();
  while (1)
  {
		Rs485_Process();
		CAN_Process();
		if(gSystemWorkLedCounter>500){Work_LED_Tog();gSystemWorkLedCounter=0;}
	}
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
  }
}

#endif

