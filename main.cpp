#include <iostream>
#include <zconf.h>
#include <unistd.h>
#include <functional>

#define SCHED_FILE      "/sys/block/sda/queue/scheduler"
#define SCHED_DEADLINE  "deadline"
#define SCHED_NOOP      "noop"
#define SCHED_CFQ       "cfq"

void change_io_scheduler(const std::string &io_scheduler);

void start_processes(size_t amount, void (*worker)());

void test_1();


int main(int argc, char *argv[]) {

    if (!access(SCHED_FILE, R_OK)) {
        std::cerr << "Cannot change I/O scheduler. Permission denied.\n";
        std::cerr << "Exiting..." << std::endl;
        exit(-1);
    }

    change_io_scheduler(SCHED_DEADLINE);
    start_processes(1, &test_1);
    change_io_scheduler(SCHED_CFQ);
    change_io_scheduler(SCHED_NOOP);

    return 0;
}


void change_io_scheduler(const std::string &io_scheduler) {
    system(("echo " + io_scheduler + " > " + SCHED_FILE).c_str());
}

void start_processes(size_t amount, void (*worker)()) {

    for (size_t i = 0; i < amount; i++) {
        pid_t pid = fork();

        if (pid == 0) {

        } else if (pid > 0) {
            worker();
        } else {
            std::cerr << "fork() failed.\n Exiting..."; << std::endl;
            exit(-1);
        }

    }

}

void test_1() {
    std::cout << "hola" << std::endl;
}