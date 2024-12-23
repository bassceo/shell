#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <chrono>
#include <sys/resource.h>

using namespace std;

vector<string> parseCommand(const string& input) {
    istringstream iss(input);
    vector<string> tokens;
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void printExecutionTime(const chrono::high_resolution_clock::time_point& start, const chrono::high_resolution_clock::time_point& end, const struct rusage& usage) {
    auto real_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    auto user_time = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000;

    auto sys_time = usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000;

    cout << "real: " << real_time << "ms, user: " << user_time << "ms, sys: " << sys_time << "ms" << endl;
}

int execute(const vector<string>& args, bool measureTime) {
    if (args.empty()) return -1;

    auto start = chrono::high_resolution_clock::now();
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error: Failed to create a new process");
        return -1;
    } else if (pid == 0) {
        vector<char*> c_args;
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("Error: Command execution failed");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        struct rusage usage;

        if (wait4(pid, &status, 0, &usage) == -1) {
            perror("Error: waitpid failed");
            return -1;
        }

        auto end = chrono::high_resolution_clock::now();

        if (measureTime) {
            printExecutionTime(start, end, usage);
        }

        return WEXITSTATUS(status);
    }
    return 0;
}

void process() {
    while (true) {
        cout << "> ";
        string command;
        getline(cin, command);

        vector<string> args = parseCommand(command);

        if (args.empty()) continue;
        if (args[0] == "quit") break;

        bool measureTime = false;

        if (args[0] == "time" && args.size() > 1) {
            measureTime = true;
            args.erase(args.begin());
        }

        if (args[0] == "cd") {
            if (args.size() < 2) {
                cout << "Usage: cd <directory>" << endl;
            } else {
                if (chdir(args[1].c_str()) != 0) {
                    perror("Error: Failed to change directory");
                }
            }
        } else {
            int status = execute(args, measureTime);
            if (status != 0) {
                cout << "Command exited with status: " << status << endl;
            }
        }
    }
}

int main() {
    process();
    return 0;
}