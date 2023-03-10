/* Driver includes */
#include "HWS_Driver/HWS_conf.h"
static const char character[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x06D, 0x7D, 0x07, 0x7F, 0x6F };

/* Priorities at which the tasks are created. */
#define task_prioritet		( tskIDLE_PRIORITY + 2 )

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "II_Vezba.h"
static mxDisp7seg_Handle myDisp;
static SemaphoreHandle_t binSem1;
static TimerHandle_t myTimer1, myTimer2;
/* A software timer that is started from the tick hook. */

static BaseType_t myTask;

//local function declaration

static void Display_Toggle_0();
static void first_task(void* pvParams);
static void TimerCallback(TimerHandle_t tmH);


static void Display_Toggle_0()
{
	static uint16_t display_value = 0;
	mxDisp7seg_SelectDigit(myDisp, 0); //aktivna prva cifru displeja sa leve srane
	if (display_value == 0)
	{
		mxDisp7seg_SetDigit(myDisp, character[display_value]);
		display_value = 1;
	}
	else
	{
		mxDisp7seg_SetDigit(myDisp, character[display_value]);
		display_value = 0;
	}
}
static void TimerCallback(TimerHandle_t tmH)
{
	uint32_t timer_number;
	timer_number = (uint32_t)pvTimerGetTimerID(tmH);
	if (timer_number==1)  xTimerStop(myTimer1, 0);

	if (timer_number == 0) xSemaphoreGive(binSem1);
}

static void first_task(void* pvParams)
{
	xTimerStart(myTimer1, portMAX_DELAY); //start tajmera 1
	xTimerStart(myTimer2, portMAX_DELAY); //start tajmera 2
	while (1) {
		
		xSemaphoreTake(binSem1, portMAX_DELAY); // cekamo na semafor
		Display_Toggle_0();
	}
}


void II_vezba_2(void)
{
	/* Inicijalizacija drajvera za displej*/
	mxDisp7seg_Init();
	myDisp = mxDisp7seg_Open(MX7_DISPLAY_0);

	const TickType_t xTimerPeriod = pdMS_TO_TICKS(500UL); // perioda tajmera


	// kreiranje taska
	myTask = xTaskCreate(first_task,			       /* funkcija koja iplementira task */
		NULL, 							/* tekstualni naziv taska, nije nepohodan, koristi se samo za debug */
		configMINIMAL_STACK_SIZE, 		/* velicina steka u bajtovima za ovaj task  */
		NULL, 							/* parametar koji se prosledjuje tasku, ovde se ne koristi*/
		task_prioritet,                 /* prioritet ovog taska, sto veci broj veci prioritet*/
		NULL);							/* referenca (handle) ovog taska, ovde se ne koristi */
	if (myTask != pdPASS) { while (1); } // proveravamo da li se task pravilno kreirao, ako nije zakucaj program



	/* Kreiramo softverski tajmer,ali ga jos ne pokrecemo */
	myTimer1 = xTimerCreate(
		NULL,/* tekstualni naziv tajmera, nije nepohodan, koristi se samo za debug  */
		pdMS_TO_TICKS(500UL),		/* perioda softverskog tajmera od 500 ms */
		pdTRUE,				/* xAutoReload je podesen na pdTRUE, tako da je ovo ciklicni tajmer */
		(void*) 0, 				/* indetifikacija ovog tajmera, ovde broj 0*/
		TimerCallback);/* ova funkcija se izvrsava kada se zavrsi jedna perioda tajmera */
	if (myTimer1 == NULL) { while (1); } // proveravamo da li se tajmer pravilno kreirao, ako nije zakucaj program
	
	myTimer2 = xTimerCreate(
		NULL,/* tekstualni naziv tajmera, nije nepohodan, koristi se samo za debug  */
		pdMS_TO_TICKS(10000UL),		/* perioda  od 5000 ms */
		pdTRUE,				/* xAutoReload je podesen na pdTRUE, tako da je ovo ciklicni tajmer */
		(void*) 1, 				/* indetifikacija ovog tajmera, ovde broj 1 */
		TimerCallback); /* poziva istu funkciju kao i myTimer1*/
	if (myTimer2 == NULL) { while (1); } // proveravamo da li se tajmer pravilno kreirao, ako nije zakucaj program

	binSem1 = xSemaphoreCreateBinary();
	if (binSem1 == NULL) while (1); // proveravamo da li se semafor pravilno kreirao

	vTaskStartScheduler();
	while (1);
}