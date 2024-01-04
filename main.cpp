#include <vector>
#include <algorithm>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <future>
#include <random>
#include <iostream>

std::vector<int> randNum(int range, int N) 
{
    std::vector<int> res;
    for (int It = 0; It < N; It++) 
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, range);
        res.push_back(dist(gen));
    }
    return res;
}

template <class T>
void print(const std::vector<T>& vec) 
{
    auto it = vec.begin();
    while (it != vec.end())
    {
        std::cout << *it << " ";
        ++it;
    }
    std::cout << "\n";
}

void findMin(std::vector<int>& data, std::promise<std::vector<int>::iterator>& promiseObj) 
{
    std::vector<int>::iterator min = std::min_element(data.begin(), data.end());
    promiseObj.set_value(min);
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russia");

    //создадим вектор случайных чисел
    std::vector<int> unsortVec = randNum(100, 30);//для сортировка в одном потоке
    std::vector<int> unsortVec_p = unsortVec;// для сортировка promise
    std::cout << "\nНеотсортированный вектор: \n";
    print(unsortVec);

    std::vector<int> sortVec;//для сортировка в одном потоке
    std::vector<int> sortVec_p;//для сортировка promise

    //отсиртируем в одном потоке
    while (unsortVec_p.size() > 0)
    {
        //создаем promise для передачи результата
        std::promise<std::vector<int>::iterator> promiseObj;

        //получаем future, связанный с promise
        std::future<std::vector<int>::iterator> futureeObj = promiseObj.get_future();

        //запускаем функцию asyncFunc в асинхронном режиме,
        //передавая вектор и promise по ссылке
        std::thread t(findMin, std::ref(unsortVec_p), std::ref(promiseObj));

        //ждем завершения результата и получаем результат
        std::vector<int>::iterator min = futureeObj.get();
        sortVec_p.push_back(*min);
        unsortVec_p.erase(min);
        t.join();
    }

    std::cout << "\nОтсортированный вектор (promise thread): \n";
    print(sortVec_p);

    return 0;
}