#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

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

        int status = execute(args);
        if (status != 0) {
            cout << "Command exited with status: " << status << endl;
        }
    }
}

int main() {
    process();
    return 0;
}
