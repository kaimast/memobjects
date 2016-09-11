//#include <sys/memfd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/file.h>

#define USE_SEMAPHORES

namespace memory
{
    struct object;

    inline int memfd_create(const char *name, int flags)
    {
        long ret = syscall(__NR_memfd_create, name,  flags);
        return static_cast<int>(ret);
    }

    template<typename T> requires __is_base_of(memory::object, T)
    T* openObject(const int fd)
    {
        void *obj = mmap(NULL, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

        if(obj == MAP_FAILED)
        {
            throw std::runtime_error("failed to memory-map object: " + std::string(strerror(errno)));
        }

        return reinterpret_cast<T*>(obj);
    }

    
    template<typename T> requires __is_base_of(memory::object, T)
    void freeObject(T *obj)
    {
        int fd = obj->fd;
        munmap(obj, obj->size);
	close(fd);
    }

    template<typename T> requires __is_base_of(memory::object, T)
    T* createObject(const char* name)
    {
        int fd = memfd_create(name, 0);
        
        if(fd <= 0)
        {
            throw std::runtime_error("failed create memfd: " + std::string(strerror(errno)));
        }

        int ret = ftruncate(fd, sizeof(T));
    
        if (ret < 0)
        {
            throw std::runtime_error("failed resize memfd: " + std::string(strerror(errno)));
        }

        T *obj = openObject<T>(fd);
        obj->fd = fd;
        obj->size = sizeof(T);
 
        sem_init(&obj->sem, 1, 1);
        sem_init(&obj->condition, 1, 1);

        return obj;
    }

    struct object
    {
    public:
        int fd;
        size_t size;
        
        sem_t sem;
        sem_t condition;
         
        void wait()
        {
            unlock();
            sem_post(&this->condition);
            lock();
        }

        void notify()
        {
            int val = -1;
            sem_getvalue(&this->condition, &val);

            if(val > 0) 
            {
                sem_wait(&this->condition);
            }
        }

        void lock()
        {
            sem_wait(&this->sem);
        } 
 
        void unlock()
        {
            sem_post(&this->sem);
        }

        
   };
}
