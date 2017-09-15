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
void change_io_scheduler(const std::string &io_scheduler);

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)());

void test_1();
void test_2();



int main(int argc, char *argv[]) {
    test_1();
    if (!access(SCHED_FILE, R_OK)) {
        std::cerr << "Cannot change I/O scheduler. Permission denied.\n";
        std::cerr << "Exiting...\n";
        exit(1);
    }

    vector<string> schedulers = {SCHED_NOOP, SCHED_DEADLINE, SCHED_CFQ};

    run_test(schedulers, 5, &test_1);


    return 0;
}


void change_io_scheduler(const string &io_scheduler) {
    system(("echo " + io_scheduler + " > " + SCHED_FILE).c_str());
}

void run_test(const vector<string> &schedulers, const size_t &nr_processes, void (*test)()) {
    auto children = vector<pid_t>();

    for (auto &scheduler : schedulers) {
        cout << "Starting test. " << " Scheduler: ";
        cout << scheduler << ". Processes:  " << nr_processes << ".\n";
        change_io_scheduler(scheduler);

        for (size_t i = 0; i < nr_processes; i++) {
            children[i] = fork();

            if (children[i] == 0) {
                test();
                exit(0);
            } else if (children[i] > 0) {

            } else {
                std::cerr << "fork() failed.\n Exiting...\n";
                exit(-1);
            }
        }

        for (size_t i = 0; i < nr_processes; i++) {
            waitpid(children[i], nullptr, 0);
        }
    }
}

void test_1() {
    ifstream file;
    file.open("main.cpp");

    char output[100];

    if(file.is_open()) {
        while (!file.eof()) {
            file >> output;
            cout << output;
        }
    }
    file.close();
}