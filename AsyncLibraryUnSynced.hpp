#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-avoid-const-params-in-decls"
//
// Created by myles on 2019-12-12.
//

#ifndef CPP_ASYNC_ASYNCLIBRARYTEST_H
#define CPP_ASYNC_ASYNCLIBRARYTEST_H

#include <utility>
#include <future>
#include <mutex>
#include <condition_variable>

template<typename R, typename... P>
class AsyncLibraryUnSynced {
private:

    /**
     * Declaration for lambda function parameter
     */
    using Function = std::function<R(P...)>;

    /**
     * Declaration for lambda callback parameter
     */
    using Callback = std::function<void(R)>;

    /**
     * Number of detached threads. This is used to denote how many threads are being executed currently.
     */
    std::atomic<int> number_of_detached_threads{0};

    /**
     * Mutex for use in the wait function
     */
    std::mutex mutex;

    /**
     * Condition variable for use in the wait function
     */
    std::condition_variable condition_variable;


    /**
     * Used in tandum with add_task_with_auto_execute_callback. See that functions docs for more info
     *
     * @param fptr function you wish to execute
     * @param params for fptr
     * @param cptr callback function to call on fptr completion
     */
    void execute_task_separate_thread(const Function fptr, const P... params, const Callback cptr){
        std::future<R> future = std::async(fptr, params...);
        future.wait();

        std::thread callback_thread = std::thread(cptr, future.get());
        callback_thread.join();

        this->number_of_detached_threads -= 1;

        if (this->number_of_detached_threads == 0) {
            std::unique_lock<std::mutex> lock(this->mutex);
            lock.unlock();
            this->condition_variable.notify_one();
        }
    }

public:

    /**
     * Auto executes a task, with this function nothing gets stored and your callback function gets auto executed on completion of the function you passed.
     * This function should only be used if your tasks have nothing to do with order, if you need to have path control execute_tasks
     * functions to control the execution flow and return response of your async tasks.
     *
     * @param fptr function you wish to execute on a separate thread
     * @param params params for fptr
     * @param cptr function you wish to pass the result of fptr to on completion
     */
    void add_task_with_auto_execute_callback(const Function fptr, const P... params, const Callback cptr){
        std::thread{&AsyncLibraryUnSynced<R, P...>::execute_task_separate_thread, this, fptr, params..., cptr}.detach();
        this->number_of_detached_threads += 1;
    }

    /**
     * Waits for the detached threads to be completed.
     * This gets called on the de-constructor but if you execute a set of tasks and you need to wait for them to complete
     * you can use this function to wait for them to complete. If you need to have control of their flow you should
     * see the execute_task functions.
     */
    void wait(){
        if (this->number_of_detached_threads > 0) {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->condition_variable.wait(lock);
        }
    }

    /**
     * Calls wait to ensure all detached tasks are completed.
     */
    ~AsyncLibraryUnSynced(){
        this->wait();
    }
};


#endif //CPP_ASYNC_ASYNCLIBRARYTEST_H

#pragma clang diagnostic pop