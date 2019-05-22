// Jonathan Rokicki, Assignment 1 UD1447

#include <iostream>
#include "ComLib.h"

void producerState(char argv, size_t msgSize, size_t buffSize, int nrOfMsg);
void consumerState(char argv, size_t msgSize, size_t buffSize, int nrOfMsg);
void gen_random(char *s, const int len);
int rndNr();

std::string createMsg();

int delay = 0;
int memSize = 0;
int numMsg = 0;
int msgSize = 0;

std::string sharedMem = "abcs";

DWORD ms = INFINITE;
HANDLE myMutex;


int main(int argc, char** argv)
{
	//myMutex = CreateMutex(nullptr, false, "mutex1");

	// argv[2][0] == delay in millseconds
	delay = atoi(&argv[2][0]);
	//std::cout << delay << std::endl;

	// argv[3][0] == shared memory size in mb
	memSize = atoi(&argv[3][0]);
	//std::cout << memSize << std::endl;

	// argv[4][0] == number of messages
	numMsg = atoi(&argv[4][0]);
	//std::cout << numMsg << std::endl;

	// argv[5][0] == random/message size(only for producer)
	if (argv[5][0] == 'r' || argv[5][0] == 'r')
	{
		msgSize = rndNr();
	}
	else
	{
		msgSize = atoi(&argv[5][0]);
	}
	//std::cout << msgSize << std::endl;
	
	//WaitForSingleObject(myMutex, ms);
	//{
		if (argv[1][0] == 'p' || argv[1][0] == 'P')
		{
			producerState(argv[1][0], msgSize, memSize, numMsg);
		}
		if (argv[1][0] == 'c' || argv[1][0] == 'C')
		{
			consumerState(argv[1][0], msgSize, memSize, numMsg);
		}
	//}
	//ReleaseMutex(myMutex);

	return 0;
}

void producerState(char argv, size_t msgSize, size_t buffSize, int nrOfMsg)
{
	// create comlib object as producer
	// for each message
	//	   create message
	//     send message

	ComLib myComLib(sharedMem, buffSize, ComLib::TYPE::PRODUCER);
	Sleep(delay);
	char *rnd = new char[msgSize];
	int notDone = 0;
	while(notDone < nrOfMsg)//for (int i = 0; i < nrOfMsg; i++)
	{
		gen_random(rnd, msgSize);

		if (myComLib.send(rnd, msgSize) == true)
		{
			notDone++;
			std::cout << notDone << ": " << rnd << std::endl;
		}
	}
	

	//std::cout << "Producer\n";
	delete[] rnd;
	//getchar();
}

void consumerState(char argv, size_t msgSize, size_t buffSize, int nrOfMsg)
{
	// allocate buffer for entire execution.

	// create comlib object as consumer
	// for each message
		//	   query size of next message, check if buffer is big enough
		//     if buffer is not big enough, reallocate (delete and new again)
		//     else call recv to get message copied into buffer,
	    //     print message,
	ComLib myComLib(sharedMem, buffSize, ComLib::TYPE::CONSUMER);
	Sleep(delay);
	size_t waterMarkSize = msgSize;

	char *msgToRecv = new char[waterMarkSize];
	int notDone = 0;
	while(notDone < nrOfMsg)//for (int i = 0; i < nrOfMsg; i++)
	{
		size_t nextSize = myComLib.nextSize();
		if (nextSize != -1)
		{
			if (nextSize > waterMarkSize)
			{
				delete[] msgToRecv;
				msgToRecv = new char[nextSize];
				waterMarkSize = nextSize;
			}
			else
			{
				if (myComLib.recv(msgToRecv, msgSize))
				{
					std::cout << notDone + 1 << ": " << msgToRecv << std::endl;
					notDone++;
				}
			}
		}
	}

	//std::cout << "Consumer\n";
	delete[] msgToRecv;
	//getchar();
}

void gen_random(char * s, const int len)
{
	static const char alphanum[] = "0123456789""ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < len; ++i)
	{
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	s[len - 1] = 0;
}

int rndNr()
{
	int randomNumber = 0;

	for (auto i = 0; i < 3; i++)
	{
		randomNumber = rand() % 1024 + 10;
	}

	return randomNumber;
}

std::string createMsg()
{
	std::string myString = "Hello World!";
	return myString;
}
