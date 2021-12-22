#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <cstdlib>
#include <iostream>
#include "pthread_sleep.c"
#include <cstdio>
#include <ctime>

#define NUM_THREADS 5

pthread_t laneThreads[NUM_THREADS-1];

pthread_mutex_t mutex;
pthread_cond_t condition;
pthread_attr_t attributes;

enum lane{NORTH,EAST,SOUTH,WEST};

struct Car{
    time_t arrival;
    lane lane;
};



void* defaultLane(void* lane);

using namespace std;
int main(int argc, char **argv){
    int sim_time = 10;
    double probability = 0;
    queue<Car> northQueue;
    queue<Car> eastQueue;
    queue<Car> southQueue;
    queue<Car> westQueue;
    void* status;

    //reading s and p from commandline
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"-s") == 0){
            sim_time = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-p")==0){
            char* temp = argv[i+1];
            probability = strtod(temp,NULL);
        }
    }

    Car car;
    eastQueue.push(car);
    westQueue.push(car);


    // time_t start;
    // time(&start);

    pthread_create(&laneThreads[1],NULL,defaultLane,(void *)EAST);
    pthread_create(&laneThreads[3],NULL,defaultLane,(void *)WEST);

    cout << "sim_time: " << sim_time << endl;
    cout << "probability: " << probability << endl;

    //pthread_sleep(15);

    pthread_join(laneThreads[1],&status);
    pthread_join(laneThreads[3],&status);

    // time_t end;
    // time(&end);

    // cout<<"duration: "<< difftime(end,start) <<endl;

    return 0;
}

using namespace std;
void *defaultLane(void *lane){
    long way;
    way = (long)lane;
    if(way == 1){
        cout << "lane: " << way << endl;
    }else if(way == 2){
        cout << "lane: " << way << endl;
        
    }
}