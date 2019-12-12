#include <iostream>
#include <thread>
#include <list>
#include <future>
#include <fstream>
#include "AsyncLibrarySynced.cpp"
#include "AsyncLibraryUnSynced.cpp"

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

/**
 * Example task 1 U
 * @return sum
 */
int task1U(int a, int b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return a + b;
}

/**
 * Example task 2 U
 * @return minus
 */
int task2U(int a, int b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return a - b;
}

/**
 * Example task 3 U
 * @return product
 */
int task3U(int a, int b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return a * b;
}


int main() {

    auto *asyncLibrarySynced = new AsyncLibrarySynced<std::string>();

    auto function = [](std::string string) {
        std::cout << string << std::endl;
    };

    auto function2 = [](int sum) {
        std::cout << sum << std::endl;
    };

    auto *asyncLibraryUnSynced = new AsyncLibraryUnSynced<int, int, int>();

    asyncLibraryUnSynced->add_task_with_auto_execute_callback(task1U, 5, 6, function2);
    asyncLibraryUnSynced->add_task_with_auto_execute_callback(task2U, 4, 3, function2);
    asyncLibraryUnSynced->add_task_with_auto_execute_callback(task3U, 7, 8, function2);

    std::cout << "Executing un-ordered tasks" << std::endl;

    asyncLibraryUnSynced->wait();

    std::cout << "Executing controlled output. in order of 1 2 3" << std::endl;

    auto *list_of_ids = new std::list<long>();

    list_of_ids->push_back(asyncLibrarySynced->execute_single_task(task1));
    list_of_ids->push_back(asyncLibrarySynced->execute_single_task(task2));
    list_of_ids->push_back(asyncLibrarySynced->execute_single_task(task3));

    std::cout << "Pushed all tasks" << std::endl;

    for (auto x: *list_of_ids) {
        try {
            asyncLibrarySynced->get_result_from_task_with_callback(x, function);
        } catch (const char *msg) {
            std::cout << msg << std::endl;
        }
    }

    // garbage collection
    delete list_of_ids;

    delete asyncLibraryUnSynced;

    delete asyncLibrarySynced;

    printf("Program complete");

    return 0;
}