#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <chrono>

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

int execute(const vector<string>& args) {
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
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error: waitpid failed");
            return -1;
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "\nExecution time: " << duration.count() << "ms" << endl;
        return WEXITSTATUS(status);
    }
    return 0;
}

void parallelExecute(int n, const vector<string>& args) {
    vector<pid_t> pids;
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error: Failed to create a new process");
            return;
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
            pids.push_back(pid);
        }
    }

    for (pid_t pid : pids) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error: waitpid failed");
            return;
        }
        if (WEXITSTATUS(status) != 0) {
            cout << "Command exited with status: " << WEXITSTATUS(status) << endl;
        }
    }
}

void process() {
    while (true) {
        cout << "> ";
        string command;
        getline(cin, command);

        vector<string> args = parseCommand(command);

        if (args.empty()) continue;
        if (args[0] == "quit") break;

        if (args[0] == "parallel") {
            if (args.size() < 3) {
                cout << "Usage: parallel <n> <command>" << endl;
                continue;
            }
            int n = stoi(args[1]);
            vector<string> cmdArgs(args.begin() + 2, args.end());
            parallelExecute(n, cmdArgs);
        } else {
            int status = execute(args);
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
