#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAXINLINE 800

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int MAXPERCAR;
int numCars;
int peopleInLine = 0;
int turnedAway = 0;
int passengers = 0;
int totalTurnedAway = 0;
int totalPeopleInLine = 0;

void rideManager(){
	struct timespec timeToWait;
	struct timeval current;
	int timeInMs = 10;
	int timeUp;
	const time_t start = time(NULL); // 600 minute timer (9am to 7pm)
	time_t current;

	while(difftime(current, start) < 60)
	{
		time(&current);
		timeToWait.tv_nsec = (current.tv_usec+10UL * timeInMs) * 10UL;

	    pthread_mutex_unlock(&mutex);
	    timeUp = pthread_cond_timedwait(&cond, &mutex, &timeToWait);

	    // Line Code
	    int newPeople = Random.poisson(meanArrival);

	    if((newPeople + peopleInLine) < MAXINLINE)
	    {
		    peopleInLine += newPeople;
		}
		else if(peopleInLine == MAXINLINE)
		{
			turnedAway = newPeople;
		}
		else 
		{
			int over = peopleInLine + newPeople;
			int rem = over - MAXINLINE;
			peopleInLine += rem;
			turnedAway = newPeople - rem;
		}
		totalTurnedAway += turnedAway;

	    // Car Code
		if(peopleInLine < MAXPERCAR)
		{
			passengers = peopleInLine;
		}
		else
		{
			passengers = MAXPERCAR;
			peopleInLine = peopleInLine - MAXPERCAR;
		}

		pthread_mutex_lock(&mutex);
	}
}

int main(int argc, char **argv) {
	pthread_t carThreads[numCars];
	pthread_t lineThread;
	pthread_mutex_init(&mutex, NULL);
	long tid;
	long lineID;
	int car;
	int line;

	lineID = pthread_create(&lineThread, NULL, rideManager, NULL);

	pthread_mutex_inti(*mutex, NULL);
	pthread_cond_init(*cond, NULL);

	if(lineID)
	{
		perror("Error in thread creating\n"); 
	    exit(-1);
	}
	
	for (int tid = 0; tid < numCars; tid++) 
	{
		car = pthread_create(&tid[tid], NULL, rideManager, NULL);
		if (car) 
		{
			perror("Error in thread creating\n"); 
			exit(-1); 
		}
	}

	for(int w = 0; w < numCars; w++)
	{
		pthread_join(carThreads[w], NULL);
	}

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
	return 0;
}