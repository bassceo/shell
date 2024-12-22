# Лабораторная работа №1
## Лукьянчук Ярослав P3323 (367361)
```JSON
{
  "target_os": "Linux",
  "fork_kind": "fork",
  "bench": {
    "0": "ema-sort-int",
    "1": "bin-search"
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

```


## Часть 2. Мониторинг и профилирование

Предстоит разработать комплекс программ-нагрузчиков согласно варианту, указанному преподавателем. Каждый нагрузчик должен принимать как минимум один параметр, определяющий количество повторений для алгоритма. Программы должны нагружать вычислительную систему, дисковую подсистему или обе одновременно. Компиляция должна происходить без использования опций оптимизации компилятора.

### Этапы работы:

1. ***Постарайтесь оценить время работы программы-нагрузчика или её результаты (если необходимо измерение). Обоснуйте свои предположения, основываясь на знании ОС, опыте и характеристиках оборудования.***

#### Бинарный поиск

- **%USER**: высокий, так как основная часть работы связана с вычислениями и сравнениями элементов массива.

- **%SYS**: низкий, так как алгоритм не требует частых системных вызовов.

- **%WAIT**: низкий, так как алгоритм не зависит от ввода-вывода и работы диска.

- **Реальное время выполнения**: log(n)/10^9 с, для n = 100000, log(100000)/10^9 ≈ 0.000000005 с, плюс время, затраченное на создание массива для поиска в нём (оно линейно).

- **Переключения контекста**: минимальные.

#### Внешняя сортировка чисел

- **%USER**: умеренный, так как часть работы связана с сортировкой блоков в оперативной памяти.

- **%SYS**: высокий из-за частых обращений к файловой системе для чтения и записи блоков данных.

- **%WAIT**: высокий из-за работы диска, особенно при слиянии блоков.

- **Реальное время выполнения**: O(n log n), где n — количество элементов.

- **Переключения контекста**: частые из-за необходимости управления вводом-выводом и системными вызовами.

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

#### Бинарный поиск

```Shell
time ./parallels 3 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-22 at 23.54.15.png](https://cdn.buildin.ai/s3/0f03ecc6-fa80-4e0c-b2cf-0ffdf36557f2/Screenshot%2B2024-12-22%2Bat%2B23.54.15.png?time=1734900300&token=4e37dbf59e784b2c4f615b3115344242&role=sharePaid)

![Screenshot 2024-12-22 at 23.55.51.png](https://cdn.buildin.ai/s3/1014610f-4113-45c1-af31-fb8d36585930/Screenshot%2B2024-12-22%2Bat%2B23.55.51.png?time=1734900300&token=8552179f2c3a712d6ab287bfbd7ea3b7&role=sharePaid)

```Shell
time ./parallels 5 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-22 at 23.56.10.png](https://cdn.buildin.ai/s3/4f71cf70-0227-4330-a931-1aff38c754ba/Screenshot%2B2024-12-22%2Bat%2B23.56.10.png?time=1734900300&token=8ba60e27a1618eba49251f1ff842b118&role=sharePaid)

![Screenshot 2024-12-22 at 23.56.38.png](https://cdn.buildin.ai/s3/72cbedb0-cfc7-4471-a84b-eefe295d49c3/Screenshot%2B2024-12-22%2Bat%2B23.56.38.png?time=1734900300&token=e9d076c79dcee1c89ffa2a0a7f0a9b9f&role=sharePaid)

```Shell
time ./parallels 6 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-22 at 23.57.10.png](https://cdn.buildin.ai/s3/ba25d592-8f3b-4815-9727-840ce67aebbf/Screenshot%2B2024-12-22%2Bat%2B23.57.10.png?time=1734900300&token=2e5221dc46eae16eac7a476c20e39ef9&role=sharePaid)

![Screenshot 2024-12-22 at 23.58.36.png](https://cdn.buildin.ai/s3/1d27bb45-d2be-41fe-b103-4889e3ac981d/Screenshot%2B2024-12-22%2Bat%2B23.58.36.png?time=1734900300&token=1b9af9e2762b762ff087940849353faa&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./parallels 6 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-22 at 23.59.48.png](https://cdn.buildin.ai/s3/764fe058-8ff2-489c-a597-ed21abdbb620/Screenshot%2B2024-12-22%2Bat%2B23.59.48.png?time=1734900300&token=817d0c24142f6fed630c5f3450532520&role=sharePaid)

#### Внешняя сортировка чисел

```Shell
time ./parallels 3 ./load_tester ema-sort-int 1 1000000
```


![Screenshot 2024-12-23 at 00.50.47.png](https://cdn.buildin.ai/s3/4162f113-b9aa-4568-9ab3-64cbe31d787b/Screenshot%2B2024-12-23%2Bat%2B00.50.47.png?time=1734903900&token=b90666c1afcdd2a3d182d675cd6e0e5a&role=sharePaid)

![Screenshot 2024-12-23 at 00.55.42.png](https://cdn.buildin.ai/s3/bc9ce4d6-942e-4f87-aefb-8b489a6c9c5b/Screenshot%2B2024-12-23%2Bat%2B00.55.42.png?time=1734903900&token=8f782e72ea693775c497994234e77e64&role=sharePaid)

```Shell
time ./parallels 6 ./load_tester ema-sort-int 1 1000000
```


![Screenshot 2024-12-23 at 01.02.21.png](https://cdn.buildin.ai/s3/b57fd6b7-a34b-49a8-8858-33c7566ec513/Screenshot%2B2024-12-23%2Bat%2B01.02.21.png?time=1734904800&token=a4bad97582c7257dc26f726f3afe5436&role=sharePaid)

![Screenshot 2024-12-23 at 01.02.47.png](https://cdn.buildin.ai/s3/f6a34e52-c4f0-4602-929e-c12d8bd6fd4a/Screenshot%2B2024-12-23%2Bat%2B01.02.47.png?time=1734904800&token=a13d530f629e46113ad6bcad5322b0c4&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./parallels 6 ./load_tester ema-sort-int 1 1000000
```


![Screenshot 2024-12-23 at 01.09.44.png](https://cdn.buildin.ai/s3/cc73450b-f446-437d-bbd6-f5392adf8b2d/Screenshot%2B2024-12-23%2Bat%2B01.09.44.png?time=1734904800&token=e17dd84c341f9c9717efa008390e792c&role=sharePaid)

4. ***Увеличьте количество нагрузчиков в два, три и четыре раза. Как это повлияло на предыдущие метрики?***

#### Бинарный поиск

```Shell
time ./parallels 12 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-23 at 01.14.48.png](https://cdn.buildin.ai/s3/4d381b93-6055-4c54-be72-3a3babdc4ae6/Screenshot%2B2024-12-23%2Bat%2B01.14.48.png?time=1734904800&token=65447485f25747e06b95808766ce86e8&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./parallels 24 ./load_tester bin-search 1 1000000
```


![Screenshot 2024-12-23 at 01.16.16.png](https://cdn.buildin.ai/s3/919c11f8-01f1-4f6b-bc5d-77174ce40d4a/Screenshot%2B2024-12-23%2Bat%2B01.16.16.png?time=1734905700&token=c7522ca0c6d48178875f986c32581e51&role=sharePaid)

![Screenshot 2024-12-23 at 01.18.11.png](https://cdn.buildin.ai/s3/14364806-216e-4765-a365-8889c2a694ec/Screenshot%2B2024-12-23%2Bat%2B01.18.11.png?time=1734905700&token=8ac37f184d5d69f2670392c75e55958a&role=sharePaid)

#### Внешняя сортировка чисел

```Shell
time ./parallels 12 ./load_tester ema-sort-int 1 100000
```


![Screenshot 2024-12-23 at 01.24.31.png](https://cdn.buildin.ai/s3/8ccca6ed-ccaa-41c4-868f-a559c37e68a0/Screenshot%2B2024-12-23%2Bat%2B01.24.31.png?time=1734905700&token=f074a325d792c6d0e42131d436e89ed2&role=sharePaid)

![Screenshot 2024-12-23 at 01.24.49.png](https://cdn.buildin.ai/s3/1f971cf5-e868-46c2-b13f-14eb6339d9a4/Screenshot%2B2024-12-23%2Bat%2B01.24.49.png?time=1734905700&token=760757c1aded5053e66702fda8e9f69a&role=sharePaid)

```Shell
sudo perf stat -e task-clock,context-switches,cpu-migrations,page-faults,cycles,instructions ./parallels 24 ./load_tester ema-sort-int 1 1000000
```


![Screenshot 2024-12-23 at 01.26.00.png](https://cdn.buildin.ai/s3/02909502-354b-44dd-8e47-73b0f1997810/Screenshot%2B2024-12-23%2Bat%2B01.26.00.png?time=1734905700&token=d82dd4c9af7d5f28a1ca7482d0653b11&role=sharePaid)

![Screenshot 2024-12-23 at 01.47.04.png](https://cdn.buildin.ai/s3/2232fd27-62f2-429d-b052-1945c41c632b/Screenshot%2B2024-12-23%2Bat%2B01.47.04.png?time=1734908400&token=3f160449dd922f0b74f473f7ebc1c9ab&role=sharePaid)

![Screenshot 2024-12-23 at 01.47.20.png](https://cdn.buildin.ai/s3/7379d88b-d2a8-4b98-9e24-47394050f1e9/Screenshot%2B2024-12-23%2Bat%2B01.47.20.png?time=1734907500&token=925ca26a83b0de62f052b6a661b1a3c1&role=sharePaid)

5. ***Объедините нагрузчики в одну программу с потоками, чтобы один нагрузчик эффективно использовал все ядра системы. Как изменились времена выполнения при том же объеме вычислений? Запустите одну, две, три такие программы.***

```Shell
time ./parallels 1 ./load_tester bin-search 3 1000000
```


![Screenshot 2024-12-23 at 01.55.04.png](https://cdn.buildin.ai/s3/112591c0-88d3-4a6b-8142-f86d323e3ab7/Screenshot%2B2024-12-23%2Bat%2B01.55.04.png?time=1734907500&token=7da4e0f38ede74ae64e195e39d5ff6ae&role=sharePaid)

```Shell
time ./parallels 3 ./load_tester bin-search 3 1000000
```


![Screenshot 2024-12-23 at 02.12.06.png](https://cdn.buildin.ai/s3/31b22b7d-1c4c-4dad-9a9b-940c0bfaeb1b/Screenshot%2B2024-12-23%2Bat%2B02.12.06.png?time=1734908400&token=5a7bbb6d45b87413ee59e0d36d7fba24&role=sharePaid)

```Shell
time ./parallels 1 ./load_tester ema-sort-int 3 1000000
```


![Screenshot 2024-12-23 at 02.02.51.png](https://cdn.buildin.ai/s3/0acb9c0b-0221-4844-a710-c11db4d99496/Screenshot%2B2024-12-23%2Bat%2B02.02.51.png?time=1734910200&token=a14985349c3ed07bf88320925fe0205f&role=sharePaid)

![Screenshot 2024-12-23 at 02.03.09.png](https://cdn.buildin.ai/s3/1d22a3c3-1f6b-4049-9682-eab163eb82cd/Screenshot%2B2024-12-23%2Bat%2B02.03.09.png?time=1734908400&token=822f7f20f2d3ee5a18e67335967cc7d6&role=sharePaid)

```Shell
time ./parallels 3 ./load_tester ema-sort-int 3 1000000
```


![Screenshot 2024-12-23 at 02.23.35.png](https://cdn.buildin.ai/s3/d1764b03-389f-4452-844c-a2852732fee9/Screenshot%2B2024-12-23%2Bat%2B02.23.35.png?time=1734909300&token=87c5cc51f4a3bb7f03bd5b4725087e71&role=sharePaid)

6. ***Включите опции агрессивной оптимизации компилятора. Как изменились времена выполнения? Насколько сократилось реальное время работы нагрузчика?***

```Shell
time ./parallels 1 ./load_tester_opt bin-search 3 1000000
```


![Screenshot 2024-12-23 at 02.06.34.png](https://cdn.buildin.ai/s3/b850dc0c-de91-43d4-9038-61af4485fa12/Screenshot%2B2024-12-23%2Bat%2B02.06.34.png?time=1734908400&token=edf69fda5a5776f20fa2acd5a485e404&role=sharePaid)

```Shell
time ./parallels 1 ./load_tester_opt ema-sort-int 3 1000000
```


![Screenshot 2024-12-23 at 02.08.31.png](https://cdn.buildin.ai/s3/c4cfd0d2-c05b-47db-9a91-986c80d9fcf1/Screenshot%2B2024-12-23%2Bat%2B02.08.31.png?time=1734908400&token=48eb6394c610aca351f54403c953182b&role=sharePaid)

### Вывод

- В ходе выполнения лабораторной работы была разработана оболочка shell и программы-нагрузчики на языке C++. Для сбора метрик программ-нагрузчиков использовались утилиты Linux.

