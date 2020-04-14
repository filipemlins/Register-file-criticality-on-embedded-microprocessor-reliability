/*
 * Author: Lucas Antunes Tambara {lucas.a.tambara@gmail.com}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define FALSE 0
#define TRUE 1
#define BUFFER_SIZE 1024
#define SLEEPING_TIME 3000 // 3 s
#define BACKUP_TIME 300 // 5 min

FILE* logs;

int firstExecution = TRUE;

HANDLE handleError(HANDLE hSerial, char*, char[], int);
void printTime(FILE*);
void printMessage(FILE*, char*);
HANDLE openConnection(char[], int);
void createBackup(char[]);
void hardResetBoard();
void softResetBoard();
void firstReadback();
void readback();
int appendArray(char[], char[]);
void clearArray(char[], int);

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		printf("usage: _campaignStart/_campaignResume <COM port> <timeout (s)> <log file>\n\r");
      printf("ex: _campaignStart/_campaignResume COM10 60 logs_s1_arm.txt\n\r");
		exit(0);
	}
	
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts={0};
	DWORD dwBytesRead = 0;
	double timeout;
	time_t start, now, backupTime;
	
   timeout = (double) atoi(argv[2]);

   int logsNameSize = sizeof(argv[3]);
   char logsName[logsNameSize];
   strcpy(logsName, argv[3]);
   
   logs = fopen(argv[3], "a");
	
	hSerial = openConnection(argv[1], SLEEPING_TIME);
	//FlushFileBuffers(hSerial);
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	
	// error getting state
	if(!GetCommState(hSerial, &dcbSerialParams)) hSerial = handleError(hSerial, "error getting state\n\r", argv[1], SLEEPING_TIME);
   
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	
	// error setting serial port state
	if(!SetCommState(hSerial, &dcbSerialParams)) hSerial = handleError(hSerial, "error setting state\n\r", argv[1], SLEEPING_TIME);
	
	timeouts.ReadIntervalTimeout = 500;
	timeouts.ReadTotalTimeoutConstant = 10000; //timeout for alive reception
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
   
	if(!SetCommTimeouts(hSerial, &timeouts)) hSerial = handleError(hSerial, "error setting timeout\n\r", argv[1], SLEEPING_TIME);
	
	int lastIndex = 0, mustResetBoard = FALSE;
   
   char* ptr;
   
   int unexpected_output = 0;
   int repeated_output = 0;
   char prev_ptr[128];
   int  count = 0;
   
   memset(prev_ptr, '\0', 128);
	
	time(&backupTime);
	time(&start);
    int i = 0;
	while(i != 100000)
	{
      if(firstExecution)
      {
         firstReadback();
      
         firstExecution = FALSE;
      }
      char buffer[BUFFER_SIZE];
      
		if(ReadFile(hSerial, buffer, BUFFER_SIZE-1, &dwBytesRead, NULL))
		{
			if(dwBytesRead > 0)
			{
				buffer[dwBytesRead] = 0;
            
            mustResetBoard = FALSE;
            
            // evaluate output
            ptr = strtok(buffer, "\n");
            while(ptr != NULL)
            {
               if(strstr(ptr, "ARM mm_s1"))
               {               
                  printTime(logs);
                  fprintf(logs, ptr); 
                  fprintf(logs, "\n\r");
                  printf("%s\n\r", ptr);

                  unexpected_output = 0;
                  if(strstr(prev_ptr, ptr)){ repeated_output++; } else { repeated_output = 0; }
               }
               else if(strstr(ptr, "FAILED"))
               {
                  printTime(logs);
                  fprintf(logs, ptr); 
                  fprintf(logs, "\n\r");
                  printf("%s\n\r", ptr);
                  mustResetBoard = TRUE;
                  unexpected_output = 0;
                  if(strstr(prev_ptr, ptr)){ repeated_output++; } else { repeated_output = 0; }
               } 
			   else if(strstr(ptr, "SUCCESS;"))
			   {
				 count++;
				 if(count == 30){
				   unexpected_output++;
				   count = 0;
				   mustResetBoard = TRUE;
				 }
			   }
            /*   else if(!(strstr(ptr, "r0;"))&&
                       !(strstr(ptr, "r1;"))&&
                       !(strstr(ptr, "r2;"))&&
                       !(strstr(ptr, "r3;"))&&
                       !(strstr(ptr, "r4;"))&&
                       !(strstr(ptr, "r5;"))&&
					   !(strstr(ptr, "r6;"))&&
                       !(strstr(ptr, "r7;"))&&
                       !(strstr(ptr, "r8;"))&&
                       !(strstr(ptr, "r9;"))&&
                       !(strstr(ptr, "r10;"))&&
					   !(strstr(ptr, "r11;"))&&
                       !(strstr(ptr, "r12;"))&&
                       !(strstr(ptr, "lr;"))&&
                       !(strstr(ptr, "r14;"))&&
                       !(strstr(ptr, "SUCCESS;"))&&
					   !(strstr(ptr, "0;")))
			
					   
					   
               {
                  printTime(logs);
                  fprintf(logs, ptr);
                  fprintf(logs, " - unexpected output\n\r");
                  printf("%s - unexpected output\n\r", ptr);
                  
                  unexpected_output++;
                  if(strstr(prev_ptr, ptr)){ repeated_output++; } else { repeated_output = 0; }
               }*/
               else
               {
                  printTime(logs);
                  fprintf(logs, ptr);
                  fprintf(logs, "\n\r");
                  printf("%s\n\r", ptr);
                  
                  unexpected_output = 0;
				  count = 0;
                  if(strstr(prev_ptr, ptr)){ repeated_output++; } else { repeated_output = 0; }
               }
               
               if(unexpected_output > 1)
               {
                  mustResetBoard = TRUE;
                  printTime(logs);
                  fprintf(logs, "repeated unexpected outputs. board will be reseted\n\r");
                  printf("repeated unexpected outputs. board will be reseted\n\r");
               }
               if(repeated_output > 1)
               {
                  mustResetBoard = TRUE;
                  printTime(logs);
                  fprintf(logs, "repeated outputs. board will be reseted\n");
                  printf("repeated outputs. board will be reseted\n");
               }
               
               if(mustResetBoard)
               {
			       mustResetBoard = FALSE;
                  readback();
                  hardResetBoard();
               }
               memset(prev_ptr, '\0', 128);
               strcpy(prev_ptr, ptr);
               ptr = strtok(NULL, "\n");
            }
            
				time(&start);
			}
         // no data read
			else
			{
				printMessage(logs, "no data read\n\r");
				time(&now);
				if(difftime(now, start) >= timeout)
				{
					// timeout
               printMessage(logs, "timeout. board will be reseted\n\r");
					readback();
               hardResetBoard();
					time(&start);
				}
			}
			i++;
		}
      // no data received
		else 
      {
         hSerial = handleError(hSerial, "error receiving alive. board will be reseted\n\r", argv[1], SLEEPING_TIME);
      }
		
		// backup
		time(&now);
		if(difftime(now, backupTime) >= BACKUP_TIME)
		{
         createBackup(logsName);
			time(&backupTime);
		}
	}
}

/*
 * handle error
 */
 HANDLE handleError(HANDLE hSerial, char* message, char arg[], int sleepingTime)
{
	if(hSerial != NULL)
   {
      CloseHandle(hSerial);
   }
	if(message != NULL)
   {
      printMessage(logs, message);
   }
	
	char lastError[1024];
	
   FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lastError,
		1024,
		NULL
	);
	
	printf("%s\n\r", lastError);
	
	return openConnection(arg, sleepingTime);
}

/*
 * keep trying to connect
 */
HANDLE openConnection(char arg[], int sleepingTime)
{
	hardResetBoard();
   
	HANDLE hSerial;
	
	do
	{
		hSerial = CreateFile(arg,
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);
		
		// error while connecting
		if(hSerial == INVALID_HANDLE_VALUE)
		{
			if(GetLastError() == ERROR_FILE_NOT_FOUND) 
         {
            printMessage(logs, "no serial port. port is busy\n\r");
         }
			else
         {
            printMessage(logs, "error opening port. port is busy\n\r");
         }

			Sleep(sleepingTime);
		}
		
	} while(hSerial == INVALID_HANDLE_VALUE);
	
	return hSerial;
}

/*
 * print log message
 */
void printMessage(FILE* logs, char *message)
{
	printTime(logs);
	fprintf(logs, message);
	printf(message);
}

/*
 * print time to file
 */
void printTime(FILE* logs)
{
   SYSTEMTIME lt;

   GetLocalTime(&lt);
   fprintf(logs, "%02d/%02d/%04d - %02d:%02d:%02d - ", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond);
}

/*
 * create a backup of the logs
 */
void createBackup(char *logsName)
{
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	long version = 100 * (100 * (100 * lt.wMonth + lt.wDay) + lt.wHour) + lt.wMinute;
	
	// creating command
	char strVersion[10];
	sprintf(strVersion, "%lu", version);
	int commandSize = 32;
	char command[commandSize];
	clearArray(command, commandSize);
	appendArray(command, "_createBackup.bat ");
	appendArray(command, strVersion);
	
	// closing log files
   fflush(logs);
	fclose(logs);
	system(command);
	
	// creating backup
	printMessage(logs, "backup created\n\r");
	
	// opening log files
	logs = fopen(logsName, "a");
}

/*
 * reset board
 */
void hardResetBoard()
{
	system("_resetBoardHard.bat");
	printMessage(logs, "board hard reseted\n\r");
}

void softResetBoard()
{
	system("_resetBoardSoft.bat");
	printMessage(logs, "board soft reseted\n\r");
}

/*
 * FPGA readback
 */
void firstReadback()
{
	system("_readbackFirst.bat");
	printMessage(logs, "first readback\n\r");
}

void readback()
{
	system("_readback.bat");
	printMessage(logs, "readback\n\r");
}

/*
 * array operations
 */
int appendArray(char array1[], char array2[])
{
	int size1, size2;
	for (size1 = 0; array1[size1] != 0; size1++);
	for (size2 = 0; array2[size2] != 0; array1[size1++] = array2[size2++]);
	array1[size1] = 0;
	
	return size1;
}

void clearArray(char array[], int size)
{
	int i;
	for(i = 0; i < size; array[i++] = 0);
}
