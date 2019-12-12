//
// Created by myles on 2019-12-10.
//

#include <random>
#include <list>
#include "AsyncLibrarySynced.h"

/**
 * @return a random id for the creation of tasks
 */
template<typename R, typename... P>
long AsyncLibrarySynced<R, P...>::l_rand() {
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
template<typename R, typename... P>
void AsyncLibrarySynced<R, P...>::delete_function(const long id) {
    this->map_of_results->erase(id);
}

/**
 * Initialize maps.
 */
template<typename R, typename... P>
AsyncLibrarySynced<R, P...>::AsyncLibrarySynced() {
    this->map_of_results = new std::unordered_map<long, Future>();
}

/**
 * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
 *
 * @param id task id
 */
template<typename R, typename... P>
long AsyncLibrarySynced<R, P...>::execute_single_task(const Function fptr, const P... params) {

    long random_id;

    while (this->map_of_results->count((random_id = l_rand())) != 0);

    this->map_of_results->insert(std::make_pair(random_id, std::async(fptr, params...)));

    return random_id;
}

/**
 * Returns the result from the task id passed. Throws an exception if the task doesn't exist
 *
 * @param id task id
 * @return result from task based on task id
 */
template<typename R, typename... P>
R AsyncLibrarySynced<R, P...>::get_result_from_task(const long id) {
    if (this->map_of_results->count(id) == 1) {

        Future future = std::move(this->map_of_results->at(id));

        future.wait();

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
template<typename R, typename... P>
void
AsyncLibrarySynced<R, P...>::get_result_from_task_with_callback(const long id, const Callback cptr) {
    if (this->map_of_results->count(id) == 1) {

        Future future = std::move(this->map_of_results->at(id));

        future.wait();

        R result = future.get();

        this->delete_function(id);

        cptr(result);

        return;
    }

    throw "Id does not exist";
}

/**
 * Kills all tasks and garbage collects all data from the maps
 */
template<typename R, typename... P>
void AsyncLibrarySynced<R, P...>::shutdown_now() {
    auto *list_of_ids = new std::list<long>();

    for (auto x : *this->map_of_results) {
        list_of_ids->push_back(x.first);

        Future shared_future = x.second;
        shared_future.wait();
        shared_future.get();
    }

    for (auto x: *list_of_ids)
        this->delete_function(x);

    delete list_of_ids;
}

/**
 * De-constructor. Frees the maps and calls shutdown_now
 */
template<typename R, typename... P>
AsyncLibrarySynced<R, P...>::~AsyncLibrarySynced() {
    this->shutdown_now();
    delete this->map_of_results;
}
