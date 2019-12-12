#include <iostream>
#include <thread>
#include <list>
#include <future>
#include <fstream>
#include "AsyncLibrarySupplier.cpp"

/**
 * Example task 1
 * @return task1
 */
std::string task1() {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return "task1";
}

/**
 * Example task 2
 * @return task2
 */
std::string task2() {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return "task2";
}

/**
 * Example task 3
 * @return task3
 */
std::string task3() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return "task3";
}

int main() {
    auto *asyncLibrary = new AsyncLibrarySupplier<std::string>();

    auto function = [](std::string string) {
        std::cout << string << std::endl;
    };

    asyncLibrary->add_task_with_auto_execute_callback(task1, function);
    asyncLibrary->add_task_with_auto_execute_callback(task2, function);
    asyncLibrary->add_task_with_auto_execute_callback(task3, function);

    std::cout << "Executing un-ordered tasks" << std::endl;

    asyncLibrary->wait();

    std::cout << "Executing controlled output. in order of 1 2 3" << std::endl;

    auto *list_of_ids = new std::list<long>();

    list_of_ids->push_back(asyncLibrary->execute_single_task(task1));
    list_of_ids->push_back(asyncLibrary->execute_single_task(task2));
    list_of_ids->push_back(asyncLibrary->execute_single_task(task3));

    std::cout << "Pushed all tasks" << std::endl;

    for (auto x: *list_of_ids) {
        try {
            asyncLibrary->get_result_from_task_with_callback(x, function);
        } catch (const char *msg) {
            std::cout << msg << std::endl;
        }
    }

    // garbage collection
    delete list_of_ids;

    delete asyncLibrary;

    printf("Program complete");

    return 0;
}