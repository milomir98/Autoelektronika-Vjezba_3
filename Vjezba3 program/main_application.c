// Driver includes
#include "HWS_Driver/HWS_conf.h"
static const char character[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x06D, 0x7D, 0x07, 0x7F, 0x6F };

// Priorities at which the tasks are created
#define task_prioritet		( tskIDLE_PRIORITY + 2 )

// Kernel includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "II_vezba.h"

static mxDisp7seg_Handle myDisp;
static BaseType_t myTask;

// Local function declaration
static void DisplayToggle_0();
static void firstTask(void* pvParams);

// Local function implementation
static void DisplayToggle_0() {
	static uint16_t display_value = 0;
	mxDisp7seg_SelectDigit(myDisp, 0);
	if (display_value == 0) {		
		mxDisp7seg_SetDigit(myDisp, character[display_value]);
		display_value = 1;
	}
	else {		
		mxDisp7seg_SetDigit(myDisp, character[display_value]);
		display_value = 0;
	}
}

static void firstTask(void* pvParams) {
	while (1) {   
		vTaskDelay(pdMS_TO_TICKS(500));
		DisplayToggle_0();
	}
}


void II_vezba(void)
{
	// Inicijalizacija drajvera za displej
	mxDisp7seg_Init();
	myDisp = mxDisp7seg_Open(MX7_DISPLAY_0);


	// Kreiranje taskova 
	myTask = xTaskCreate(firstTask,			// funkcija koja iplementira task 
		NULL, 								// tekstualni naziv taska, nije nepohodan, koristi se samo za debug 
		configMINIMAL_STACK_SIZE, 			// velicina steka u bajtovima za ovaj task  
		NULL, 								// parametar koji se prosledjuje tasku, ovde se ne koristi
		task_prioritet,						// prioritet ovog taska, sto veci broj veci prioritet
		NULL);								// referenca (handle) ovog taska, ovde se ne koristi 
	if (myTask != pdPASS) { while (1); }	// proveravamo da li se task pravilno kreirao, ako nije zakucaj program
	
	

	// Kreiranje softverskih tajmera 

	
	// Kreiranje binarnih semafora 


	// Pokretanje rasporedjivaca
	vTaskStartScheduler();
	while (1);
}