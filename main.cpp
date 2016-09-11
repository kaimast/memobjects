#include "memobjects.h"
#include <iostream>
#include <chrono>
#include <mutex> 

using std::chrono::system_clock;

constexpr size_t NUM_OBJECTS = 1000 * 1000;

struct foo : public memory::object
{
     int bar;
};
    

inline void bench_creation()
{
   std::cout << "Creation" << std::endl;

   auto start1 = system_clock::now();

   for(size_t i = 0; i < NUM_OBJECTS; ++i)
   {
   	foo *obj = memory::createObject<foo>("test");
   	obj->bar = 42;
        memory::freeObject(obj);
   }

   auto end1 = system_clock::now();
   auto start2 = system_clock::now();

   for(size_t i = 0; i < NUM_OBJECTS; ++i)
   {
        foo *obj = new foo();
        obj->bar = 42;
        delete obj;
   }

   auto end2 = system_clock::now();

   auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
   auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);

   std::cout << dur1.count() << " vs " << dur2.count() << std::endl;
}

inline void bench_locking()
{
   std::cout << "Locking" << std::endl;

   foo *obj = memory::createObject<foo>("test");
   
   auto start1 = system_clock::now(); 


   for(size_t i = 0; i < NUM_OBJECTS; ++i)
   {
        obj->lock();
   	obj->bar = 42;
        obj->unlock();
   }

   auto end1 = system_clock::now();
   memory::freeObject(obj);
   std::mutex m;  
 
   obj = new foo();
   auto start2 = system_clock::now();

   for(size_t i = 0; i < NUM_OBJECTS; ++i)
   {
       m.lock();
       obj->bar = 42;
       m.unlock();
   }

   auto end2 = system_clock::now();
   delete obj;

   auto dur1 = static_cast<float>((end1 - start1).count())/ NUM_OBJECTS;
   auto dur2 = static_cast<float>((end2 - start2).count()) / NUM_OBJECTS;

   std::cout << dur1 << " vs " << dur2 << std::endl;
}

int main()
{
   bench_creation();
   bench_locking();
   return 0;
}

