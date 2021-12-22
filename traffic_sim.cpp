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
pthread_t policeThread;
pthread_mutex_t street;
pthread_cond_t condition;
pthread_attr_t attributes;

enum lane{NORTH,EAST,SOUTH,WEST}; // 0,1,2,3

struct Car{
    time_t arrival;
    lane lane;
};

    std::queue<Car> northQueue;
    std::queue<Car> eastQueue;
    std::queue<Car> southQueue;
    std::queue<Car> westQueue;

int sim_time = 10;
    double probability = 0;
void* defaultLane(void* lane);
void* police(void*);
using namespace std;
int main(int argc, char **argv){
    srand((int)time(0));
    void* status;

    int sec = 0;
    double duration = 0;
    time_t startTime = time(0);
    clock_t start = clock();
    clock_t prev_sec = clock();

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

    cout << "sim_time: " << sim_time << endl;
    cout << "probability: " << probability << endl;

    pthread_create(&laneThreads[1],NULL,defaultLane,(void *)EAST);
    pthread_create(&laneThreads[3],NULL,defaultLane,(void *)WEST);
    pthread_create(&policeThread,NULL,police,NULL);

    while(duration < sim_time){
        pthread_mutex_lock(&street);
        if( clock() - prev_sec > CLOCKS_PER_SEC) {
            prev_sec = ++sec * CLOCKS_PER_SEC; 
            cout << "one second passed" <<endl;
        }

        duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
        pthread_mutex_unlock(&street);
    }

    
    exit(0);
    //pthread_sleep(15);

    // time_t end;
    // time(&end);

    // cout<<"duration: "<< difftime(end,start) <<endl;

    return 0;
}

void addCar(int lane){
    Car car;
    //double random = (double)rand() / (double)RAND_MAX;
    
    if(lane==0){
        if((double)rand() / (double)RAND_MAX<1-probability){
            northQueue.push(car);
        }
    }else if(lane == 1){
        if((double)rand() / (double)RAND_MAX<probability){
            eastQueue.push(car);
        }
    }else if(lane == 2){
        if((double)rand() / (double)RAND_MAX<probability){
            southQueue.push(car);
        }
        
    }else if(lane == 3){
        if((double)rand() / (double)RAND_MAX<probability){
            westQueue.push(car);
        }
    }
}

using namespace std;
void *defaultLane(void *lane){
    while(true){
        long way;
        way = (long)lane;
        pthread_mutex_lock(&street);
        if(way == 1){
            addCar(way);
            cout << eastQueue.size() << endl;
        }else if(way == 3){
            addCar(way);
            cout << "lane: 3 " << westQueue.size() << endl;
        }
        pthread_mutex_unlock(&street);
        pthread_sleep(1);
    }
}

void *police(void*){
    cout << "police thread running" << endl;
    bool laneEmpty = false;
    int max = 0;
    int busyLane = 0;
    if(max < eastQueue.size()){
        max = eastQueue.size();
        busyLane = 1;
    }if(max < southQueue.size()){
        max = southQueue.size();
        busyLane = 2;
    }if(max < westQueue.size()){
        max = westQueue.size();
        busyLane = 3;
    }if (max < northQueue.size()){
        max = northQueue.size();
        busyLane = 0;
    }
    int laneNum = busyLane;
    while(true){
       
        pthread_mutex_lock(&street);
        if(laneNum == 0){
            if(northQueue.size() > 0){
                northQueue.pop();
                pthread_sleep(1);
                if(northQueue.size() == 0){
                    laneEmpty = true;
                }
            }  
        }
        else if(laneNum == 1){
            if(eastQueue.size() > 0){
                eastQueue.pop();
                pthread_sleep(1);
                if(northQueue.size() == 0){
                    laneEmpty = true;
                }
            }
        }
        else if(laneNum == 2){
            if(southQueue.size() > 0){
                southQueue.pop();
                pthread_sleep(1);
                if(northQueue.size() == 0){
                    laneEmpty = true;
                }
            }
        }
        else if(laneNum == 3){
            if(westQueue.size() > 0){
                westQueue.pop();
                pthread_sleep(1);
                if(northQueue.size() == 0){
                    laneEmpty = true;
                }
            }
        }
        if(max < eastQueue.size()){
            max = eastQueue.size();
            busyLane = 1;
        }if(max < southQueue.size()){
            max = southQueue.size();
            busyLane = 2;
        }if(max < westQueue.size()){
            max = westQueue.size();
            busyLane = 3;
        }if (max < northQueue.size()){
            max = northQueue.size();
            busyLane = 0;
        }
        pthread_mutex_unlock(&street);
        if(max >= 5 || laneEmpty){
            laneNum = busyLane;
        }
    }
}

