#include <stdbool.h>
#include <pthread.h>

/**
 * This structure should be dynamically allocated and passed as
 * an argument to your thread using pthread_create.
 * It should be returned by your thread so it can be freed by
 * the joiner thread.
 */

// The whole point of this struct is to isolate the mutex lock to a specific struct (an "account" in the banking example in the text "Linux Systems Programming" in Chapter 7). This is an example of a fine-grained lock, as it only locks the access to teh data in the account
struct thread_data{
    /*
     * TODO: add other values your thread will need to manage
     * into this structure, use this structure to communicate
     * between the start_thread_obtaining_mutex function and
     * your thread implementation.
     */
     // The mutex is a lock that all threads accessing this struct will have to invoke in order to abide by a locking mechanism. It this mutex member is locked by another thread, it will block until the mutex becomes available (when it is unlocked by the previous occupant) and it will induce it's own lock to prevent other threads from engaging until it performs its own unlock. Mutex locks are to be respected in the entirety of the code or race conditions can still occur
     pthread_mutex_t* mutex;
     int wait_to_obtain_ms;
     int wait_to_release_ms;

    /**
     * Set to true if the thread completed with success, false
     * if an error occurred.
     */
    bool thread_complete_success;
};


/**
* Start a thread which sleeps @param wait_to_obtain_ms number of milliseconds, then obtains the
* mutex in @param mutex, then holds for @param wait_to_release_ms milliseconds, then releases.
* The start_thread_obtaining_mutex function should only start the thread and should not block
* for the thread to complete.
* The start_thread_obtaining_mutex function should use dynamic memory allocation for thread_data
* structure passed into the thread.  The number of threads active should be limited only by the
* amount of available memory.
* The thread started should return a pointer to the thread_data structure when it exits, which can be used
* to free memory as well as to check thread_complete_success for successful exit.
* If a thread was started succesfully @param thread should be filled with the pthread_create thread ID
* coresponding to the thread which was started.
* @return true if the thread could be started, false if a failure occurred.
*/
bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms);
