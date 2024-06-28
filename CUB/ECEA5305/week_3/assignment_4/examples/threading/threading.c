#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // Casting the thread_param argument to the function as a pointer to a struct of type thread_data, named as "tp"
    struct thread_data* tp = (struct thread_data*) thread_param;
    // Setting the struct member as false as an initial value. When it completes properly, the value will be changed to true
    tp->thread_complete_success = false;

    // Creating an placeholder struct to hold the time specification values. the timespec struct is a specifically designed struct specified in the time.h header file. The memvers tv_sec specify the suspend time in seconds, and the tv_nsec specifies the suspend time in nanoseconds.
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ((long) tp->wait_to_obtain_ms) * 1000000L;

    // The second argument to nanosleep is redundant, generally NULL in most cases.
    int res = nanosleep(&ts,&ts);
    if(res) {
        perror("interrupted sleep");
        return thread_param;
    }

    int rc = pthread_mutex_lock(tp->mutex);
    if(rc) {
        perror("cannot access mutex lock");
        return thread_param;
    }

    ts.tv_nsec = ((long) tp->wait_to_release_ms) * 1000000L;
    res = nanosleep(&ts,&ts);
    if(res) {
        perror("interrupted sleep");
        return thread_param;
    }

    rc = pthread_mutex_unlock(tp->mutex);
    if(rc) {
        perror("cannot unlock mutex lock");
        return thread_param;
    }

    tp->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
     // Allocate a data member equal to the size of the struct thred_data defined in the header file
     struct thread_data* td = malloc(sizeof(struct thread_data));
     if(!td) {
         perror("malloc failed");
         return false;
     }
     // The mutex passed to the start_thread_obtaining_mutex function is set to be pointed to by the mutex member in the thread_data struct that was allocated earlier. The mutex will be locked and unlocked at certain times, and the mutex itself will block the calling thread. 
     td->mutex=mutex;
     td->wait_to_obtain_ms = wait_to_obtain_ms;
     td->wait_to_release_ms = wait_to_release_ms;

     //This is the thread execution point. The thread is created with it's specific arguments: the first argument "thread", is a pointer or reference to a previously declared thread object in the code. The second argument is NULL as this typically described the thread attributes, which are only modified in rare cases (to redefine stack size, schedulizing parameters, etc.), the threadfunc is a function callback the is started within the thread specified in the first parameter, and the "td" value is the sole argument that is passed to the thread function (which is why it has to match the type)
     int ret = pthread_create(thread, NULL, threadfunc, td);
     // The ret value will return a nonzero error code if an error arises with the thread creation
     if(ret) {
         perror("Could not create thread");
         free(td);
         return false;
     }
     return true;
}
