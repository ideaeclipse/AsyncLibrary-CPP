//
// Created by myles on 2019-12-10.
//

#include <random>
#include <list>
#include "AsyncLibrarySupplier.h"

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
long AsyncLibrarySupplier<R>::add_task(const AsyncLibrarySupplier::function_pointer_type fptr) {

    long random_long;

    while (this->map_of_functions->count((random_long = l_rand())) != 0);

    this->map_of_functions->insert(std::make_pair(random_long, std::move(fptr)));

    return random_long;
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
    for (std::pair<long, function_pointer_type> pair : *this->map_of_functions) {
        this->map_of_results->insert(std::make_pair(pair.first, std::move(std::async(pair.second))));
    }
}

/**
 * Kills all tasks and garbage collects all data from the maps
 */
template<typename R>
void AsyncLibrarySupplier<R>::shutdown_now() {
    auto *list_of_ids = new std::list<long>();
    for (std::pair<long, function_pointer_type> pair : *this->map_of_functions) {

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
    printf("Size = %lu and %lu\n", this->map_of_functions->size(), this->map_of_results->size());
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
 * @param callback callback function
 */
template<typename R>
void AsyncLibrarySupplier<R>::get_result_from_task_with_callback(const long id, void (*callback)(R)) {
    if (this->map_of_results->count(id) == 1) {

        std::future<R> future = std::move(this->map_of_results->at(id));

        R result = future.get();

        this->delete_function(id);

        callback(result);

        return;
    }

    throw "Id does not exist";
}

/**
 * De-constructor. Frees the maps and calls shutdown_now
 */
template<typename R>
AsyncLibrarySupplier<R>::~AsyncLibrarySupplier() {
    this->shutdown_now();
    delete this->map_of_functions;
    delete this->map_of_results;
}
