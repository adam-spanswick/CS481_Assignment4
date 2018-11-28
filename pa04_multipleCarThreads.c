#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "random437.h"
#include "lineList.c"

#define MAXINLINE 800

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct node *line;
struct node *head;
int MAXPERCAR;
int numCars;
int newPeople = 0;
int peopleInLine = 0;
int passengers = 0;
int turnedAwayPerMin = 0;
int totalTurnedAway = 0;
int totalPeopleInLine = 0;
int totalPassengers = 0;
int totalPeopleArrived = 0;
int meanArrival = 0; 
int min = 1;
int maxLine = 0;
int prevLine = 0;
int maxLineMin = 0;
int prevMin = 0;
int avgWaitTime = 0;
int totalWaitTime = 0;
int newLine;
int lineArr[600];

void *ride();
void *arrivals();
void *waitingLine();

void *manager()
{
	FILE *fp;
	fp = fopen("outputStats.txt", "w");
	FILE *gp;
	gp = fopen("graphdata.csv", "w");
	FILE *final;
	final = fopen("finalStats.txt", "w");

	if(final == NULL)
    {
    	printf("Error with fp\n");
    	exit(1);
    }

    if(gp == NULL)
    {
    	printf("Error with gp\n");
    	exit(1);
    }

	pthread_t car;
	pthread_t arrival;
	pthread_t waiting;

	// Overall timer
	for(min; min <= 600; min++)
	{
		// Thread for the people arriving
		if(pthread_create(&arrival, NULL, arrivals, NULL))
		{
			perror("Error in creating arrival thread\n"); 
	        exit(-1);
		}

		// Thread for the waiting line
		if(pthread_create(&waiting, NULL, waitingLine, NULL))
		{
			perror("Error in creating line thread\n"); 
	        exit(-1);
		}

		// Threads for the cars
		for(int c = 1; c <= numCars; c++)
		{
			if(pthread_create(&car, NULL, ride, NULL))
			{
				perror("Error in thread creating\n"); 
	        	exit(-1);
			}
			pthread_join(car, NULL);
			totalPassengers += passengers;
		}

		totalPeopleArrived += newPeople;
		totalTurnedAway += turnedAwayPerMin;

		fprintf(fp, "Step %d: People arrived: %d, People rejected: %d, People in line: %d\n", min, newPeople, turnedAwayPerMin, peopleInLine);
		fprintf(fp, "\n");

		fprintf(gp, "%d, %d, %d\n", newPeople, turnedAwayPerMin, peopleInLine);

		// usleep(100000);
	}

	maxLine = lineArr[0];
	for(int i = 0; i < 600; i++)
	{
		if(lineArr[i] > maxLine)
		{
			maxLine = lineArr[i];
			maxLineMin = i;
		}
	}

	avgWaitTime = totalWaitTime/totalPassengers;

	fprintf(final, "Total people arrived: %d\n", totalPeopleArrived);
	fprintf(final, "Total passengers: %d\n", totalPassengers);
	fprintf(final, "Total turned away: %d\n", totalTurnedAway);
	fprintf(final, "Average wait time in mins: %d\n", avgWaitTime);
	fprintf(final, "Longest line length: %d, Time it occured: %d\n", maxLine, maxLineMin);
}

void *arrivals()
{
	pthread_mutex_lock(&mutex);

	    // Determine the mean arrival
		if(min < 120)
		{
		    meanArrival = 25;
	    }
	    else if(min < 300)
	    {
		    meanArrival = 45;
	    }
	    else if(min < 420)
	    {
		    meanArrival = 35;
	    }
	   	else if(min < 600)
	    {
		    meanArrival = 25;
		}

	    //Get how many people arrived in the line	
	    newPeople = poissonRandom(meanArrival);

    pthread_mutex_unlock(&mutex);
}

void *waitingLine()
{
	pthread_mutex_lock(&mutex);

	    // Linked list used to calculate waiting time
		line = (node*)malloc(sizeof(node));
		head = line;

		newLine = peopleInLine + newPeople;

		// Calculate if we are turning people away or not 
		if(newLine < MAXINLINE)
		{
			peopleInLine += newPeople;

			for(int i = 1; i <= newPeople; i++)
			{
				line = append(line, i);
			}
		}
		else if(newLine > MAXINLINE)
		{
			int rem = newLine - MAXINLINE;
			peopleInLine = MAXINLINE;
			turnedAwayPerMin = rem;

			for(int i = 1; i <= rem; i++)
			{
				line = append(line, i);
			}
		}
		else if(newLine == MAXINLINE)
		{
			peopleInLine = MAXINLINE;
		}

		// Add the current number of people to the line array to calculate max line
        lineArr[min] = peopleInLine;

	pthread_mutex_unlock(&mutex);
}

void *ride()
{
	// Manage the people going on the ride
	pthread_mutex_lock(&mutex);

		// Calculate the number of people to send on the ride & the number of people 
		// remaining in the line
		if(peopleInLine < MAXPERCAR)
		{
			passengers = peopleInLine;
			peopleInLine = 0;

            while(head != NULL)
            {
            	totalWaitTime += head->arrivalTime;
                head = head->next;
            }
            head = line;
		}
		else if(peopleInLine >= MAXPERCAR)
		{
			passengers = MAXPERCAR;
			peopleInLine = (peopleInLine - MAXPERCAR);
			
			while(head != NULL)
            {
            	totalWaitTime += head->arrivalTime;
                head = head->next;
            }
            head = line;
		}

	pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv) 
{
	if(argc < 5){
        printf("Not enough command line arguments.\n");
        return 0;
    }

    for(int i = 1; i < argc; i++){
        if(strcmp("-N", argv[i]) != 0){
            numCars = atoi(argv[2]);
        }else if(strcmp("-M", argv[i]) != 0){
            MAXPERCAR = atoi(argv[4]);
        }
    }

	pthread_t lineThread;
	
	if(pthread_create(&lineThread, NULL, manager, NULL))
	{
		perror("Error in thread creating\n"); 
	    exit(-1);
	}

	pthread_join(lineThread, NULL);

    pthread_mutex_destroy(&mutex);
     
	return 0;
}