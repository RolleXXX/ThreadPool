#include<iostream>
#include <unistd.h>
#include "threadpool.h"
#include"locker.h"


class Task{
public:
    Task(){};
    void process(){
        std::cout << "thread is runing" << std::endl;
    }

};

int main(){
    threadpool<Task> pool;
    Task task_array[5];
    // std::cout << "test" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        pool.append(&task_array[i]);
        std::cout << "test" << std::endl;
        
    }
     while(1)  
    {  
        printf("there are still %d tasks need to process\n", pool.size());  
        if (pool.size() == 0)
        {  
            
            printf("Now I will exit from main\n"); 
            exit(0);   
        }  
        sleep(2);  
    }  
    

    
}
