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

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)());

void test_1();



int main(int argc, char *argv[]) {
    printf("%s, %s, %s\n", "scheduler", "proc", "time");

    vector<string> schedulers = {SCHED_NOOP, SCHED_DEADLINE, SCHED_CFQ};

    for(int i = 0; i < 1; i++) {
        run_test(schedulers, 5, &test_1);
    }

    return 0;
}


void change_io_scheduler(const string &io_scheduler) {
    system(("echo " + io_scheduler + " > " + SCHED_FILE).c_str());
}

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)()) {

    for (auto &scheduler : schedulers) {
        auto children = vector<pid_t>();
        change_io_scheduler(scheduler);

        auto start = std::chrono::system_clock::now();

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

        auto end = std::chrono::system_clock::now();


        std::chrono::duration<double> elapsed = end - start;
        printf("%s, %ld, %f\n", scheduler.c_str(), nr_processes, elapsed.count());
    }
}

void test_1() {
    ifstream file;
    file.open("file", ios::in);

    std::cout << "lol" << std::endl;
    char output[1000];
    
    if (file.is_open()) {
        while (!file.eof()) {
            file >> output;
        }
    }
    file.close();
}