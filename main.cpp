#include <functional>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <numeric>
#include <chrono>
#include <thread>
#include <execution>
#include <omp.h>

using namespace std::chrono;

class Number {
public:
    Number(int i) : value(i) {}
    Number() = default;

    operator int() const {
        //std::this_thread::sleep_for(nanoseconds(1));
        return value;
    }

private:
    int value = 0;
};

using numberType = int;

std::vector<numberType> vec;

class Sum {
public:
    Sum(std::function<int(const std::vector<numberType> &)> sumStrategy)
        : sumStrategy_(sumStrategy) {}

    int result(const std::vector<numberType> &vec) const {
        return sumStrategy_(vec);
    }

private:
    std::function<int(const std::vector<numberType> &)> sumStrategy_;
};

void Calculate(const Sum &sumStrategy, const std::string &name) {
    std::chrono::milliseconds time{};
    int result;
    constexpr auto times = 100;
    for (int i = 0; i < times; ++i) {
        auto start = high_resolution_clock::now();
        result = sumStrategy.result(vec);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        time += duration;
    }
    std::cout << name << " = " << result << '\n';
    std::cout << "average time taken by function: " << time.count() / times << " milliseconds\n";
}

int main() {
    vec = std::vector<numberType>(1'000'000);
    std::iota(vec.begin(), vec.end(), 0);

    Sum sum1([](const auto &vec) -> int {
                 auto size = vec.size();
                 if (!size) {
                     return 0;
                 }
                 return (vec[0] + vec[size - 1]) * size / 2; //from the formula for an arithmetic sequence
             });
    Sum sum2([](const auto &vec)
             { return std::accumulate(vec.begin(), vec.end(), 0); });
    Sum sum3([](const auto &vec)
             { return std::reduce(std::execution::seq, vec.begin(), vec.end(), 0); });
    Sum sum4([](const auto &vec)
             { return std::reduce(std::execution::par_unseq, vec.begin(), vec.end(), 0); });
    Sum sum5([](const auto &vec) {
                 int result = 0;
                 const auto size = vec.size();
#pragma omp parallel
                 {
                     int local_result;
#pragma omp for
                     for (int i = 0; i < size; ++i) {
                         local_result = vec[i];
#pragma omp critical
                         result += local_result;
                     }
                 }
                 return result;
             });

    Calculate(sum1, "aritmetic ");
    Calculate(sum2, "accumulate");
    Calculate(sum3, "reduce1   ");
    Calculate(sum4, "reduce2   ");
    Calculate(sum5, "omp       ");

    return 0;
}
