```JSON
{
  target_os: 'Linux',
  fork_kind: 'fork',
  bench: {
    0: 'ema-sort-int',
    1: 'bin-search'
  }
}
```


![https://lh7-rt.googleusercontent.com/docsz/AD_4nXd_YrDODeZ-NFo3QP0GVUtvK7G3cuynZuKwdWW1cn0NnSo7B2iTr6tKKbLJnM_VDMdPWsj-RtC5WGgEt3KUZpC_WDtFSc3vaSx83IyF6xkyymgXepqk5Ebc6tFA6YVcKoUhOC9BGg?key=oNVTorgbSB7Nwyw5HIdzqDWT](https://lh7-rt.googleusercontent.com/docsz/AD_4nXd_YrDODeZ-NFo3QP0GVUtvK7G3cuynZuKwdWW1cn0NnSo7B2iTr6tKKbLJnM_VDMdPWsj-RtC5WGgEt3KUZpC_WDtFSc3vaSx83IyF6xkyymgXepqk5Ebc6tFA6YVcKoUhOC9BGg?key=oNVTorgbSB7Nwyw5HIdzqDWT)

## Часть 1. Запуск программ

Вам предстоит разработать собственную оболочку командной строки — **shell**. Выбор операционной системы остаётся за вами. **Shell** должен позволять пользователю запускать программы на компьютере с передачей аргументов командной строки. По завершении выполнения программы необходимо отображать реальное время её работы (рассчитывается как разница между «временем завершения» и «временем запуска»).

```C++
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

```


## Часть 2. Мониторинг и профилирование

Предстоит разработать комплекс программ-нагрузчиков согласно варианту, указанному преподавателем. Каждый нагрузчик должен принимать как минимум один параметр, определяющий количество повторений для алгоритма. Программы должны нагружать вычислительную систему, дисковую подсистему или обе одновременно. Компиляция должна происходить без использования опций оптимизации компилятора.

### Этапы работы:

1. ***Постарайтесь оценить время работы программы-нагрузчика или её результаты (если необходимо измерение). Обоснуйте свои предположения, основываясь на знании ОС, опыте и характеристиках оборудования.***

#### Бинарный поиск

- **%USER**: Высокий, так как основная часть работы связана с вычислениями и сравнениями элементов массива.

- **%SYS**: Низкий, так как алгоритм не требует частых системных вызовов.

- **%WAIT**: Низкий, так как алгоритм не зависит от ввода-вывода и работы диска.

- **Реальное время выполнения**: Зависит от размера массива и количества повторений поиска, ожидаемое время выполнения может составлять десятки миллисекунд.

- **Переключения контекста**: Минимальные, так как алгоритм работает в основном в пользовательском пространстве.

#### Внешняя сортировка чисел

- **%USER**: Умеренный, так как часть работы связана с сортировкой блоков в оперативной памяти.

- **%SYS**: Высокий из-за частых обращений к файловой системе для чтения и записи блоков данных.

- **%WAIT**: Высокий из-за работы диска, особенно при слиянии блоков.

- **Реальное время выполнения**: Значительно больше, чем для бинарного поиска, и может составлять несколько секунд или минут в зависимости от объема данных и скорости дисковой подсистемы.

- **Переключения контекста**: Частые из-за необходимости управления вводом-выводом и системными вызовами.

2. ***Запустите программу-нагрузчик и зафиксируйте её метрики с помощью инструментов профилирования. Сравните результаты с ожидаемыми и постарайтесь их объяснить.***

#### Бинарный поиск

```Shell
time ./load_tester bin-search 1 100000
```


![Screenshot 2024-12-16 at 14.34.28.png](https://cdn.buildin.ai/s3/2897d127-d505-4677-84f5-0e0b9e7782da/Screenshot%2B2024-12-16%2Bat%2B14.34.28.png?time=1734898500&token=4d6773219d6984b4ef6499d794582ca2&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./load_tester bin-search 1 100000
```


![Screenshot 2024-12-16 at 14.36.13.png](https://cdn.buildin.ai/s3/af2ad3d3-22e1-4730-80e2-8c2e32a82da6/Screenshot%2B2024-12-16%2Bat%2B14.36.13.png?time=1734898500&token=f97907dd9ef21c842a21e4c1b8657a19&role=sharePaid)

#### Внешняя сортировка чисел

```Shell
time ./load_tester ema-sort-int 1 100000
```


![Screenshot 2024-12-16 at 14.40.35.png](https://cdn.buildin.ai/s3/82c29061-62b8-455e-b626-4d48c34ba7fd/Screenshot%2B2024-12-16%2Bat%2B14.40.35.png?time=1734898500&token=8d75e220b4e982544b66a7087f942559&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./load_tester ema-sort-int 1 100000
```


![Screenshot 2024-12-16 at 14.41.28.png](https://cdn.buildin.ai/s3/2a3325a9-7b7b-4fb4-b6cc-3268f1fddaee/Screenshot%2B2024-12-16%2Bat%2B14.41.28.png?time=1734898500&token=9782a47ddc7e8dcce2c53ef9b2d8e913&role=sharePaid)

3. ***Определите количество нагрузчиков, эффективно использующих все ядра процессора. Изучите распределение времени USER%, SYS%, WAIT%, реальное время выполнения и количество переключений контекста.***

 

4. ***Увеличьте количество нагрузчиков в два, три и четыре раза. Как это повлияло на предыдущие метрики?***

5. ***Объедините нагрузчики в одну программу с потоками, чтобы один нагрузчик эффективно использовал все ядра системы. Как изменились времена выполнения при том же объеме вычислений? Запустите одну, две, три такие программы.***

6. ***Включите опции агрессивной оптимизации компилятора. Как изменились времена выполнения? Насколько сократилось реальное время работы нагрузчика?***



