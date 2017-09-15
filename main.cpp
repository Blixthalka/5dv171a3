#include <iostream>
#include <zconf.h>
#include <unistd.h>
#include <wait.h>
#include <vector>
#include <fstream>

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

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)());

void test_1();



int main(int argc, char *argv[]) {
    printf("%-10s | %-4s | %-10s \n", "scheduler", "proc", "time");

    vector<string> schedulers = {SCHED_NOOP, SCHED_DEADLINE, SCHED_CFQ};

    run_test(schedulers, 20, &test_1);

    return 0;
}


void change_io_scheduler(const string &io_scheduler) {
    system(("echo " + io_scheduler + " > " + SCHED_FILE).c_str());
}

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)()) {

    for (auto &scheduler : schedulers) {
        auto children = vector<pid_t>();
        change_io_scheduler(scheduler);

        clock_t begin = clock();

        for (size_t i = 0; i < nr_processes; i++) {
            children.push_back(fork());

            if (children[i] == 0) {
                test();
                exit(0);
            } else if (children[i] > 0) {

            } else {
                cerr << "fork() failed.\n Exiting...\n";
                exit(-1);
            }
        }

        for (size_t i = 0; i < nr_processes; i++) {
            waitpid(children[i], nullptr, 0);
        }

        clock_t end = clock();
        double elapsed = double(end - begin) / CLOCKS_PER_SEC;
        printf("%-10s | %4ld | %-10f \n", scheduler.c_str(), nr_processes, elapsed);
    }
}

void test_1() {
    ifstream file;
    file.open("main.cpp");

    char output[100];

    if (file.is_open()) {
        while (!file.eof()) {
            file >> output;
        }
    }
    file.close();
}