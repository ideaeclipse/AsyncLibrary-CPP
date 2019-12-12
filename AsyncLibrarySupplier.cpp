//
// Created by myles on 2019-12-10.
//

#include <random>
#include <list>
#include "AsyncLibrarySupplier.h"

/**
 * Used in tandum with add_task_with_auto_execute_callback. See that functions docs for more info
 *
 * @param fptr function you wish to execute
 * @param cptr callback function to call on fptr completion
 */
template<typename R>
void AsyncLibrarySupplier<R>::execute_task_separate_thread(const std::function<R()> &fptr,
                                                           const std::function<void(R)> &cptr) {
    std::future<R> future = std::async(fptr);
    future.wait();

    std::thread callback_thread = std::thread(cptr, future.get());
    callback_thread.join();

    this->number_of_detached_threads -= 1;
}

/**
 * @return a random id for the creation of tasks
 */
template<typename R>
long AsyncLibrarySupplier<R>::l_rand() {
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<unsigned long> distr;
    return distr(eng);
}

/**
 * Garbage collection for the defined maps above
 *
 * @param id task id.
 */
template<typename R>
void AsyncLibrarySupplier<R>::delete_function(const long id) {
    this->map_of_functions->erase(id);
    this->map_of_results->erase(id);
}

/**
 * Creates a task based on a function passed.
 *
 * @param fptr function pointer
 * @return the task id that was generated for this task
 */
template<typename R>
long AsyncLibrarySupplier<R>::add_task(const std::function<R()> &fptr) {

    long random_long;

    while (this->map_of_functions->count((random_long = l_rand())) != 0);

    this->map_of_functions->insert(std::make_pair(random_long, std::move(fptr)));

    return random_long;
}

/**
 * Auto executes a task, with this function nothing gets stored and your callback function gets auto executed on completion of the function you passed.
 * This function should only be used if your tasks have nothing to do with order, if you need to have path control use add_task and the execute_tasks
 * functions to control the execution flow and return response of your async tasks.
 *
 * @param fptr function you wish to execute on a separate thread
 * @param cptr function you wish to pass the result of fptr to on completion
 */
template<typename R>
void
AsyncLibrarySupplier<R>::add_task_with_auto_execute_callback(const std::function<R()> &fptr,
                                                             const std::function<void(R)> &cptr) {
    std::thread{this->execute_task_separate_thread, this, std::move(fptr), std::move(cptr)}.detach();
    this->number_of_detached_threads += 1;
}

/**
 * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
 *
 * @param id task id
 */
template<typename R>
void AsyncLibrarySupplier<R>::execute_single_task(const long id) {
    if (this->map_of_functions->count(id) == 1) {
        this->map_of_results->insert(std::make_pair(id, std::move(std::async(this->map_of_functions->at(id)))));
    } else {
        throw "The id you passed is not a valid task";
    }
}

/**
 * Executes all tasks in the map
 */
template<typename R>
void AsyncLibrarySupplier<R>::execute_all_tasks() {
    for (std::pair<long, std::function<R()>> pair : *this->map_of_functions) {
        this->map_of_results->insert(std::make_pair(pair.first, std::move(std::async(pair.second))));
    }
}

/**
 * Returns the result from the task id passed. Throws an exception if the task doesn't exist
 *
 * @param id task id
 * @return result from task based on task id
 */
template<typename R>
R AsyncLibrarySupplier<R>::get_result_from_task(const long id) {
    if (this->map_of_results->count(id) == 1) {

        std::future<R> future = std::move(this->map_of_results->at(id));

        R result = future.get();

        this->delete_function(id);

        return result;
    }

    throw "Id does not exist";
}

/**
 * Executes a task by an id and passes it to a designated callback function as parameter number two
 *
 * @param id task id
 * @param cptr callback function
 */
template<typename R>
void AsyncLibrarySupplier<R>::get_result_from_task_with_callback(const long id, const std::function<void(R)> &cptr) {
    if (this->map_of_results->count(id) == 1) {

        std::future<R> future = std::move(this->map_of_results->at(id));

        R result = future.get();

        this->delete_function(id);

        cptr(result);

        return;
    }

    throw "Id does not exist";
}

/**
 * Waits for the detached threads to be completed.
 * This gets called on the de-constructor but if you execute a set of tasks and you need to wait for them to complete
 * you can use this function to wait for them to complete. If you need to have control of their flow you should
 * see the add_task functions and the execute_task functions.
 */
template<typename R>
void AsyncLibrarySupplier<R>::wait() {
    while (this->number_of_detached_threads > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

/**
 * Kills all tasks and garbage collects all data from the maps
 */
template<typename R>
void AsyncLibrarySupplier<R>::shutdown_now() {
    auto *list_of_ids = new std::list<long>();
    for (std::pair<long, std::function<R()>> pair : *this->map_of_functions) {

        long id = pair.first;

        list_of_ids->push_back(id);

        if (this->map_of_results->count(id) == 1) {
            std::future<R> future = std::move(this->map_of_results->at(id));

            if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                future.get();
        }

    }

    for (auto x: *list_of_ids)
        this->delete_function(x);

    delete list_of_ids;
}

/**
 * De-constructor. Frees the maps and calls shutdown_now
 */
template<typename R>
AsyncLibrarySupplier<R>::~AsyncLibrarySupplier() {
    this->wait();
    this->shutdown_now();
    delete this->map_of_functions;
    delete this->map_of_results;
}
