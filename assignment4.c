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
int totalPassengers = 0;

void rideManager(){
	const time_t start = time(NULL); // Run the program for 60 secs total, where 1 minute = 10 ms
	struct timespec timeToWait;
	struct timeval current;
	int timeInMs = 100; // Corresponds to .1 seconds 
	int timeUp;
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
		totalPeopleInLine += peopleInLine;
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
		totalPassengers += passengers;

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