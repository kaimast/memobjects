#include "memobjects.h"
#include <iostream>
#include <chrono>
#include <mutex> 

struct foo : public memory::object
{
   int the_answer;
};
    
int main()
{
   foo *obj = memory::createObject<foo>("test");
   obj->the_answer = -1;

   int res = fork();   

   if(res == 0)
   {
       // child process
       obj->lock(); 
       std::cout << "I wonder what the purpose of life is..." << std::endl;

       while(obj->the_answer == -1)
       {
          obj->wait();
       }       
   
       std::cout << "The answer is: " << obj->the_answer << std::endl;
       obj->unlock();
   }
   else if(res > 0)
   {
       // parent process
       obj->lock();
       obj->the_answer = 42;
       obj->notify();
       std::cout << "Set the answer to: " << obj->the_answer << std::endl;
       obj->unlock();
   }
   else
   {
       throw std::runtime_error("fork() failed: " + std::string(strerror(errno)));
   }

   memory::freeObject(obj);

   return 0;
}

