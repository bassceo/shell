#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>

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

int execute(vector<string> args) {
    if (args.empty()) return -1;

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Error: Failed to create a new process." << endl;
        return -1;
    } else if (pid == 0) {
        vector<char*> c_args;
        for (auto& arg : args) {
            c_args.push_back(&arg[0]);
        }
        c_args.push_back(nullptr);

        if (execvp(c_args[0], c_args.data()) == -1) {
            cerr << "Error: Command execution failed." << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        return status;
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
