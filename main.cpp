#include <iostream>
#include <thread>
#include <list>
#include <future>
#include "AsyncLibrarySupplier.cpp"

/**
 * Example task 1
 * @return task1
 */
std::string task1() {
    return "task1";
}

/**
 * Example task 2
 * @return task2
 */
std::string task2() {
    return "task2";
}

/**
 * Example task 3
 * @return task3
 */
std::string task3() {
    return "task3";
}

int main() {
    auto *asyncLibrary = new AsyncLibrarySupplier<std::string>();

    auto *list_of_ids = new std::list<long>();

    list_of_ids->push_back(asyncLibrary->add_task(task1));
    list_of_ids->push_back(asyncLibrary->add_task(task2));
    list_of_ids->push_back(asyncLibrary->add_task(task3));

    // Load each task
    for (auto x: *list_of_ids)
        try {
            asyncLibrary->execute_single_task(x);
        } catch (const char *msg) {
            std::cout << msg << std::endl;
        }

    // See the result of a task that doesn't exist
    try {
        asyncLibrary->execute_single_task(0);
    } catch (const char *msg) {
        std::cout << msg << std::endl;
    }

    // Get result from each task loaded in
    for (auto x : *list_of_ids)
        try {
            asyncLibrary->get_result_from_task_with_callback(x, [](std::string string) {
                std::cout << string << std::endl;
            });
        } catch (const char *msg) {
            std::cout << msg << std::endl;
        }


    // Gets the result from an unknown task
    try {
        std::cout << asyncLibrary->get_result_from_task(0) << std::endl;
    } catch (const char *msg) {
        std::cout << msg << std::endl;
    }


    // garbage collection
    delete list_of_ids;

    delete asyncLibrary;

    printf("Program complete");

    return 0;
}