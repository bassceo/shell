#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <n> <command> [args...]" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    vector<string> cmdArgs(argv + 2, argv + argc);

    parallelExecute(n, cmdArgs);

    return 0;
}