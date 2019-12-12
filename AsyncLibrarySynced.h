#include <utility>

//
// Created by myles on 2019-12-10.
//

#ifndef CPP_ASYNC_ASYNCLIBRARY_H
#define CPP_ASYNC_ASYNCLIBRARY_H

#include <map>
#include <future>
#include <unordered_map>
#include <vector>

/**
 * This class is used to manage async tasks. That return a result.
 * See method descriptions for descriptions to use them
 *
 * @param R Return type of your functions. If you want to have seperate types you need to use seperate suppliers
 * @param P variable number of parameters for task function parameters
 */
template<typename R, typename ...P>
class AsyncLibrarySynced {
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
     * Declaration for future return based on return parameter
     */
    using Future = std::shared_future<R>;

    /**
     * Map of functions results with their id. Use get_result to get a tasks result by id.
     */
    std::unordered_map<long, Future> *map_of_results;

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
    AsyncLibrarySynced();

    /**
     * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
     *
     * @param fptr function you wish to execute on a separate thread and get their result
     * @param params list of params for fptr
     *
     * @return the id that was created with that associated function
     */
    long execute_single_task(const Function fptr, const P... params);

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
    void get_result_from_task_with_callback(const long id, const Callback callback);

    /**
     * Kills all tasks and garbage collects all data from the maps
     */
    void shutdown_now();

    /**
     * De-constructor. Frees the maps and calls shutdown_now
     */
    ~AsyncLibrarySynced();

};


#endif //CPP_ASYNC_ASYNCLIBRARY_H
