#include <utility>

//
// Created by myles on 2019-12-10.
//

#ifndef CPP_ASYNC_ASYNCLIBRARYSUPPLIER_H
#define CPP_ASYNC_ASYNCLIBRARYSUPPLIER_H

#include <map>
#include <future>
#include <unordered_map>
#include <vector>


/**
 * This class is used to manage async tasks. That return a result.
 * See method descriptions for descriptions to use them
 *
 * @tparam R Return type of your functions. If you want to have seperate types you need to use seperate suppliers
 */
template<typename R>
class AsyncLibrarySupplier {
private:

    /**
     * Map of functions results with their id. Use get_result to get a tasks result by id.
     */
    std::unordered_map<long, std::shared_future<R>> *map_of_results;

    /**
     * Number of detached threads. This is used to denote how many threads are being executed currently.
     */
    int number_of_detached_threads = 0;

    /**
     * Used in tandum with add_task_with_auto_execute_callback. See that functions docs for more info
     *
     * @param fptr function you wish to execute
     * @param cptr callback function to call on fptr completion
     */
    void execute_task_separate_thread(const std::function<R()> fptr, const std::function<void(R)> cptr);

    /**
     * @return a random id for the creation of tasks
     */
    long l_rand();

    /**
     * Garbage collection for the defined maps above
     *
     * @param id task id.
     */
    void delete_function(const long id);

public:

    /**
     * Initialize maps.
     */
    AsyncLibrarySupplier() {
        this->map_of_results = new std::unordered_map<long, std::shared_future<R>>();
    }

    /**
     * Auto executes a task, with this function nothing gets stored and your callback function gets auto executed on completion of the function you passed.
     * This function should only be used if your tasks have nothing to do with order, if you need to have path control execute_tasks
     * functions to control the execution flow and return response of your async tasks.
     *
     * @param fptr function you wish to execute on a separate thread
     * @param cptr function you wish to pass the result of fptr to on completion
     */
    void add_task_with_auto_execute_callback(const std::function<R()> fptr, const std::function<void(R)> cptr);

    /**
     * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
     *
     * @param fptr function you wish to execute on a separate thread and get their result
     *
     * @return the id that was created with that associated function
     */
    long execute_single_task(const std::function<R()> fptr);

    /**
    * Returns the result from the task id passed. Throws an exception if the task doesn't exist
    *
    * @param id task id
    * @return result from task based on task id
    */
    R get_result_from_task(const long id);

    /**
     * Executes a task by an id and passes it to a designated callback function as parameter number two
     *
     * @param id task id
     * @param cptr callback function
     */
    void get_result_from_task_with_callback(const long id, const std::function<void(R)> callback);

    /**
     * Waits for the detached threads to be completed.
     * This gets called on the de-constructor but if you execute a set of tasks and you need to wait for them to complete
     * you can use this function to wait for them to complete. If you need to have control of their flow you should
     * see the execute_task functions.
     */
    void wait();

    /**
     * Kills all tasks and garbage collects all data from the maps
     */
    void shutdown_now();

    /**
     * De-constructor. Frees the maps and calls shutdown_now
     */
    ~AsyncLibrarySupplier();

};


#endif //CPP_ASYNC_ASYNCLIBRARYSUPPLIER_H
