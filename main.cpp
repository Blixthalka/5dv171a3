#include <iostream>
#include <zconf.h>
#include <unistd.h>
#include <wait.h>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

#define ITERATIONS 10 // the number of times to run each test
#define SCHED_FILE      "/sys/block/sda/queue/scheduler"
#define SCHED_DEADLINE  "deadline"
#define SCHED_NOOP      "noop"
#define SCHED_CFQ       "cfq"






/**
 * Changes the I/O scheduler of the system.
 * @param io_scheduler The name of the scheduler.
 */
void change_io_scheduler(const string &io_scheduler);

/**
 * Runs the inputted test.
 * @param schedulers    List of schedulers to be tested.
 * @param processes     The number of processes to be run during the test.
 * @param test          The function that does the actual test.
 * @param iterations    The number of times the test is going to be run.
 */
void run_test(const vector<string> &schedulers, const size_t &processes, void (*test)(), const size_t &iterations);

/**
 * Test 1. Reads a file that is 1 GB big.
 */


void test_1();
void test_1_init(int processes);
void test_1_cleanup(int processes);


#define RUN_TEST(test, schedulers, processes)   test ## _init(processes);                              \
                                                run_test(schedulers, processes, test, ITERATIONS);     \
                                                test ## _cleanup(processes);


int main(int argc, char *argv[]) {
    vector<string> schedulers = {SCHED_NOOP, SCHED_DEADLINE, SCHED_CFQ};
    printf("scheduler, proc, time\n");


    RUN_TEST(test_1, schedulers, 5)
    RUN_TEST(test_1, schedulers, 10)
    RUN_TEST(test_1, schedulers, 20)

    return 0;
}


void change_io_scheduler(const string &io_scheduler) {
    system(("echo " + io_scheduler + " > " + SCHED_FILE).c_str());
}

void run_test(const vector<string> &schedulers, const size_t &processes, void (*test)(), const size_t &iterations) {


    for (auto &scheduler : schedulers) {
        double time_count = 0.0;
        change_io_scheduler(scheduler);

        for (size_t j = 0; j < iterations; j++) {
            auto children = vector<pid_t>();
            auto start = std::chrono::system_clock::now();

            for (size_t i = 0; i < processes; i++) {
                children.push_back(fork());

                if (children[i] == 0) {
                    // this is the child, run test and exit
                    test();
                    exit(0);
                } else if (children[i] > 0) {

                } else {
                    cerr << "fork() failed.\n Exiting...\n";
                    exit(-1);
                }
            }

            for (size_t i = 0; i < processes; i++) {
                waitpid(children[i], nullptr, 0);
            }

            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            time_count += elapsed.count();
        }
        printf("%s, %ld, %f\n", scheduler.c_str(), processes, time_count);
    }
}

void test_1_init(int processes) {
   // std::cout << "init" << std::endl;
}

void test_1_cleanup(int processes) {
    //std::cout << "cleaning up" << std::endl;
}

void test_1() {
    ifstream file;
    file.open("file", ios::in);


    char output[100000];

    if (file.is_open()) {
        while (!file.eof()) {
            file >> output;
        }
    }
    file.close();
}