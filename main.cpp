#include <iostream>
#include <zconf.h>
#include <unistd.h>
#include <wait.h>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

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
 * Test 1. Reads a file and writes the result to an own file.
 */
void test_1();
void test_1_init();
void test_1_cleanup();

int main(int argc, char *argv[]) {
    std::cout << "scheduler, proc, time" << std::endl;
    vector<string> schedulers = {SCHED_NOOP, SCHED_DEADLINE, SCHED_CFQ};

    for(size_t i = 1; i < 100; i += 5) {
        std::cerr << "Starting test with: " << i  << " processes." << std::endl;
        test_1_init();
        run_test(schedulers, i, test_1, 10);
        test_1_cleanup();
    }

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
            auto children = vector<pid_t>(processes);
            auto start = std::chrono::system_clock::now();

            for (size_t i = 0; i < processes; i++) {
                children[i] = fork();

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
        std::cout << scheduler << ", " << processes << ", " << time_count / iterations << std::endl;
    }

}


void test_1_init() {
    system("fallocate -l 1G test_1");
}


void test_1() {
    system(("dd if=test_1  conv=fdatasync bs=2M count=30 of=test_1_out"  + std::to_string(getpid()) + " status=noxfer > /dev/null 2>&1").c_str());
}

void test_1_cleanup() {
    system("rm test_1");
    system("rm test_1_out*");
}
