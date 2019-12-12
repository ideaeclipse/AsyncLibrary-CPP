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
     * Type definition of the function that needs to be passed.
     */
    typedef R(*function_pointer_type)();

    /**
     * Map of functions with their id. use add_task to use this
     */
    std::unordered_map<long, function_pointer_type> *map_of_functions;

    /**
     * Map of functions results with their id. Use get_result to get a tasks result by id.
     */
    std::unordered_map<long, std::future<R>> *map_of_results;

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
        this->map_of_functions = new std::unordered_map<long, function_pointer_type>();
        this->map_of_results = new std::unordered_map<long, std::future<R>>();
    }

    /**
     * Creates a task based on a function passed.
     *
     * @param fptr function pointer
     * @return the task id that was generated for this task
     */
    long add_task(const function_pointer_type fptr);

    /**
     * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
     *
     * @param id task id
     */
    void execute_single_task(const long id);

    /**
     * Executes all tasks in the map
     */
    void execute_all_tasks();

    /**
     * Kills all tasks and garbage collects all data from the maps
     */
    void shutdown_now();

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
     * @param callback callback function
     */
    void get_result_from_task_with_callback(const long id, void(*callback)(R));

    /**
     * De-constructor. Frees the maps and calls shutdown_now
     */
    ~AsyncLibrarySupplier();

};


#endif //CPP_ASYNC_ASYNCLIBRARYSUPPLIER_H
