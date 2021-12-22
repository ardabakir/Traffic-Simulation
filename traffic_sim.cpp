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
pthread_mutex_t cars;
pthread_cond_t carArrive;
pthread_attr_t attributes;

enum lane{NORTH,EAST,SOUTH,WEST}; // 0,1,2,3

int carID;

struct Car{
    int id;
    time_t arrival;
    time_t departure;
    time_t wait;
    lane lane;
};

    std::queue<Car> northQueue;
    std::queue<Car> eastQueue;
    std::queue<Car> southQueue;
    std::queue<Car> westQueue;

int sim_time = 10;
double probability = 0;
int time_stamp = 10;

void* defaultLane(void* lane);
void* police(void*);
using namespace std;
int main(int argc, char **argv){
    srand((int)time(0));
    
    carID = 0;
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
        }else if(strcmp(argv[i],"-t")==0){
            time_stamp = atoi(argv[i+1]);
        }
    }
    Car car0;
    car0.id=carID;
    car0.lane = NORTH;
    car0.arrival = time(0);
    northQueue.push(car0);
    carID++;
    
    Car car1;
    car1.id=carID;
    car1.lane = EAST;
    car1.arrival = time(0);
    eastQueue.push(car1);
    carID++;

    Car car2;
    car2.id=carID;
    car2.lane = SOUTH;
    car2.arrival = time(0);
    southQueue.push(car2);
    carID++;
    
    Car car3;
    car3.id=carID;
    car3.lane = WEST;
    car3.arrival = time(0);
    westQueue.push(car3);
    carID++;


    cout << "sim_time: " << sim_time << endl;
    cout << "probability: " << probability << endl;
    cout << time_stamp << endl;

    pthread_create(&policeThread,NULL,police,NULL);
    pthread_create(&laneThreads[0],NULL,defaultLane,(void *)NORTH);
    pthread_create(&laneThreads[1],NULL,defaultLane,(void *)EAST);
    pthread_create(&laneThreads[2],NULL,defaultLane,(void *)SOUTH);
    pthread_create(&laneThreads[3],NULL,defaultLane,(void *)WEST);
    
    

    while(duration < sim_time){
        pthread_mutex_lock(&street);
        if( clock() - prev_sec > CLOCKS_PER_SEC) {
            prev_sec = ++sec * CLOCKS_PER_SEC; 

            if(time_stamp == sec){
                time_t currTime = time(0);
                string c_time = asctime(localtime(&currTime));
                cout << c_time.substr(11, 18) <<endl;
                cout<<"   "<<northQueue.size()<<endl;
                cout<<westQueue.size()<<"     "<<eastQueue.size()<<endl;
                cout<<"   "<<southQueue.size()<<endl;
            }else if(time_stamp + 1 == sec){
                time_t currTime = time(0);
                string c_time = asctime(localtime(&currTime));
                cout << c_time.substr(11, 18) <<endl;
                cout<<"   "<<northQueue.size()<<endl;
                cout<<westQueue.size()<<"     "<<eastQueue.size()<<endl;
                cout<<"   "<<southQueue.size()<<endl;
            }else if(time_stamp + 2 == sec){
                time_t currTime = time(0);
                string c_time = asctime(localtime(&currTime));
                cout << c_time.substr(11, 18) <<endl;
                cout<<"   "<<northQueue.size()<<endl;
                cout<<westQueue.size()<<"     "<<eastQueue.size()<<endl;
                cout<<"   "<<southQueue.size()<<endl;
            }

        }
        
        duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
        pthread_mutex_unlock(&street);
    }

    exit(0);
    return 0;
}

void addCar(int lane){
    Car car;
    pthread_mutex_lock(&street);
    car.id = carID;
    car.arrival = time(0);
    if(lane==0 ){
        if((double)rand() / (double)RAND_MAX<1-probability){
            carID++;
            car.lane = NORTH;
            northQueue.push(car);
            pthread_cond_signal(&carArrive);
        }else{
            pthread_sleep(19);
        }
    }else if(lane == 1 ){
        if((double)rand() / (double)RAND_MAX<probability){
            carID++;
            car.lane = EAST;
            eastQueue.push(car);
            pthread_cond_signal(&carArrive);
        }
    }else if(lane == 2 ){
        if((double)rand() / (double)RAND_MAX<probability){
            carID++;
            car.lane = SOUTH;
            southQueue.push(car);
            pthread_cond_signal(&carArrive);
        }
    }else if(lane == 3){
        if((double)rand() / (double)RAND_MAX<probability){
            carID++;
            car.lane = WEST;
            westQueue.push(car);
            pthread_cond_signal(&carArrive);
        }
    }
    pthread_mutex_unlock(&street);
}

using namespace std;
void *defaultLane(void *lane){
    int carNum = 0;
    while(true){
        long way;
        way = (long)lane;
        
        if(way == 0){
            pthread_mutex_lock(&street);
            addCar(way);
            pthread_mutex_unlock(&street);
           
        }else if(way == 1){
            pthread_mutex_lock(&street);
            addCar(way);
            pthread_mutex_unlock(&street);
        }else if(way == 2){
            pthread_mutex_lock(&street);
            addCar(way);
            pthread_mutex_unlock(&street);
        }else if(way == 3){
            pthread_mutex_lock(&street);
            addCar(way);
            pthread_mutex_unlock(&street);
        }
        
        pthread_sleep(1);
    }
}

void *police(void*){

    FILE* carFile;
    carFile = fopen("car.log","w");
    fprintf(carFile, "\tCarID \t Direction \t Arrival-Time \t Cross-Time \t Wait-Time \n");
    fprintf(carFile, "------------------------------------------------------------------------\n");

    FILE* policeFile;
    policeFile = fopen("police.log","w");
    fprintf(policeFile, "\tTime \t Event\n");
    fprintf(policeFile, "-------------------------\n");
    

    bool laneEmpty = false;
    int max = 0;
    int busyLane = 0;
    bool beating = false;
    if (max < northQueue.size()){
        max = northQueue.size();
        busyLane = 0;
    }if(max < eastQueue.size()){
        max = eastQueue.size();
        busyLane = 1;
    }if(max < southQueue.size()){
        max = southQueue.size();
        busyLane = 2;
    }if(max < westQueue.size()){
        max = westQueue.size();
        busyLane = 3;
    }
    int laneNum = busyLane;
    while(true){
       
        pthread_mutex_lock(&cars);
        int prev = carID;
        
        if(max == 0){
            time_t curr = time(0);
            string c_time = asctime(localtime(&curr));
            
            fprintf(policeFile,"%s \t Phone \n", c_time.substr(11,18).c_str());
            while(prev == carID){
                pthread_cond_wait(&carArrive,&cars);
            }
            
            curr = time(0);
            c_time = asctime(localtime(&curr));
            
            fprintf(policeFile,"%s \t Phone \n", c_time.substr(11,18).c_str());
            pthread_sleep(3);
        }
        pthread_mutex_unlock(&cars);
        pthread_mutex_lock(&street);
        if(laneNum == 0){
            if(northQueue.size() > 0){
                Car car = northQueue.front();
                car.departure = time(0);
                time_t wait_time = car.departure - car.arrival;
                string ar_time = asctime(localtime(&car.arrival));
                string dep_time = asctime(localtime(&car.departure));
                fprintf(carFile,"\t %d \t N \t %s \b %s \b\t %d \n", car.id, ar_time.substr(11,18).c_str(),dep_time.substr(11,18).c_str(),(int)wait_time);

                northQueue.pop();
                if(northQueue.size() == 0){
                    laneEmpty = true;
                }else{
                    laneEmpty = false;
                }
                pthread_sleep(1);
            }
        }
        else if(laneNum == 1){
            if(eastQueue.size() > 0){
                Car car = eastQueue.front();
                car.departure = time(0);
                time_t wait_time = car.departure - car.arrival;
                string ar_time = asctime(localtime(&car.arrival));
                string dep_time = asctime(localtime(&car.departure));
                fprintf(carFile,"\t %d \t E \t %s \t %s \t %d \n", car.id, ar_time.substr(11,18).c_str(),dep_time.substr(11,18).c_str(),(int)wait_time);

                eastQueue.pop();
                
                if(eastQueue.size() == 0){
                    laneEmpty = true;
                }else{
                    laneEmpty = false;
                }
                pthread_sleep(1);
            }
        }
        else if(laneNum == 2){
            if(southQueue.size() > 0){
                Car car = southQueue.front();
                car.departure = time(0);
                time_t wait_time = car.departure - car.arrival;
                string ar_time = asctime(localtime(&car.arrival));
                string dep_time = asctime(localtime(&car.departure));
                fprintf(carFile,"\t %d \t S \t %s \t %s \t %d \n", car.id, ar_time.substr(11,18).c_str(),dep_time.substr(11,18).c_str(),(int)wait_time);

                southQueue.pop();
                if(southQueue.size() == 0){
                    laneEmpty = true;
                }else{
                    laneEmpty = false;
                }
                pthread_sleep(1);
            }
        }
        else if(laneNum == 3){
            if(westQueue.size() > 0){
                Car car = westQueue.front();
                car.departure = time(0);
                time_t wait_time = car.departure - car.arrival;
                string ar_time = asctime(localtime(&car.arrival));
                string dep_time = asctime(localtime(&car.departure));
                fprintf(carFile,"\t %d \t W \t %s \t %s \t %d \n", car.id, ar_time.substr(11,18).c_str(),dep_time.substr(11,18).c_str(),(int)wait_time);

                westQueue.pop();
                if(westQueue.size() == 0){
                    laneEmpty = true;
                }else{
                    laneEmpty = false;
                }
                pthread_sleep(1);
            }
        }
        max = 0;
        if (max < northQueue.size()){
            max = northQueue.size();
            busyLane = 0;
        }
        if(max < eastQueue.size()){
            max = eastQueue.size();
            busyLane = 1;
        }
        if(max < southQueue.size()){
            max = southQueue.size();
            busyLane = 2;
        }
        if(max < westQueue.size()){
            max = westQueue.size();
            busyLane = 3;
        }

        int impatient = 0;
        if(!northQueue.empty()){
            if((time(0) - northQueue.front().arrival)/(double) CLOCKS_PER_SEC >= 20){
                beating = true;
                busyLane = 0;
                impatient++;
            }
        }
        else if(!eastQueue.empty()){
            if((time(0) - eastQueue.front().arrival)/(double) CLOCKS_PER_SEC >= 20){
                beating = true;
                busyLane = 1;
                impatient++;
            }
        }else if(!southQueue.empty()){
            if((time(0) - southQueue.front().arrival)/(double) CLOCKS_PER_SEC >= 20){
                beating = true;
                busyLane = 2;
                impatient++;
            }
        }else if(!westQueue.empty()){
            if((time(0) - westQueue.front().arrival)/(double) CLOCKS_PER_SEC >= 20){
                beating = true;
                busyLane = 3;
                impatient++;
            }
        }else{
            beating = false;
        }

        if(impatient == 0){
            beating = false;
        }
        
        pthread_mutex_unlock(&street);
        if(max >= 5 || laneEmpty || beating){
            
            laneNum = busyLane;
        }
    }
}

