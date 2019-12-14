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
#include <random>

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
    long l_rand(){
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
    void delete_function(const long id){
        this->map_of_results->erase(id);
    }

public:

    /**
     * Initialize maps.
     */
    AsyncLibrarySynced(){
        this->map_of_results = new std::unordered_map<long, Future>();
    }

    /**
     * Executes a task by an id. Make sure to catch the execption as if the id doesn't exist an exception will be throw
     *
     * @param fptr function you wish to execute on a separate thread and get their result
     * @param params list of params for fptr
     *
     * @return the id that was created with that associated function
     */
    long execute_single_task(const Function fptr, const P... params){
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
    R get_result_from_task(const long id){
        if (this->map_of_results->count(id) == 1) {

            Future *future = &this->map_of_results->at(id);

            future->wait();

            R result = future->get();

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
    void get_result_from_task_with_callback(const long id, const Callback callback){
        if (this->map_of_results->count(id) == 1) {

            Future *future = &this->map_of_results->at(id);

            future->wait();

            R result = future->get();

            this->delete_function(id);

            callback(result);

            return;
        }

        throw "Id does not exist";
    }

    /**
     * Kills all tasks and garbage collects all data from the maps
     */
    void shutdown_now(){
        auto *list_of_ids = new std::vector<long>();

        for (auto x : *this->map_of_results) {
            list_of_ids->push_back(x.first);

            Future *future = &x.second;
            future->get();
        }

        for (auto x: *list_of_ids)
            this->delete_function(x);

        delete list_of_ids;
    }

    /**
     * De-constructor. Frees the maps and calls shutdown_now
     */
    ~AsyncLibrarySynced(){
        this->shutdown_now();
        delete this->map_of_results;
    }

};


#endif //CPP_ASYNC_ASYNCLIBRARY_H
