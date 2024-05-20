#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <atomic>
#include <vector>
#define EPS 0.001
#define PI 3.141592653589793
#define POINTS_AMMOUNT 200000


double random(double min, double max) {
    double r = (double)rand() / RAND_MAX;
    return min + r*(max - min);
}

double f_value(double x) {
    double value = 8.0 + 2.0*x - pow(x,2.0);
    return value;
}

double MCIntegration(double x1, double x2, double y1, double y2, double S, unsigned int threads_ammount){
    srand(time(NULL));
    std::atomic<int> points_total = 0;
    std::vector<std::thread> threads;
    threads.reserve(threads_ammount);

    for(int i = 0; i < threads_ammount; i++){
        threads.emplace_back([&,i](){
            int points_local = 0;
            double y_rand, x_rand, f;
            for (int i = 0; i < POINTS_AMMOUNT/threads_ammount; i++) {
                x_rand = random(x1, x2);
                y_rand = random(y1, y2);
                f = f_value(x_rand);
                if (f > y_rand && f > 0 && y_rand > 0) {
                    points_local++;
                }
                else if (f < y_rand && f < 0 && y_rand < 0) {
                    points_local--;
                }
            }
            points_local += points_local;
        });
        }


    for (auto& thread : threads) {
        thread.join();
    }
    return (static_cast<double>(points_total) / POINTS_AMMOUNT) * S;
    
}

int main(int argc, char* argv[]){
    unsigned int threads_ammount = 0;
    if(argc != 2){
        std::cout << "Try again\n";
        return 1;
    }
    else{
        threads_ammount = std::stoi(argv[1]);
    }
    double x1 = -2.0, x2 = 4.0, y1 = -10.0, y2 = 10.0;
    double S = (x2 - x1) * (y2 -y1);
    int launches = 100;
    double expected_result = 36.0;
    int bad_approx = 0;
    std::chrono::milliseconds total_time {0};

    for(int i = 0; i < launches; i++){
        auto start = std::chrono::high_resolution_clock::now();
        double res = MCIntegration(x1, x2, y1, y2, S, threads_ammount);
        auto end = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (expected_result - res >= EPS * S) {
            bad_approx++;
        }
    }

    double value_estimation = expected_result * (S - expected_result) / (POINTS_AMMOUNT * pow(EPS, 2) * pow(S, 2));
    std::cout << "Average time for " << POINTS_AMMOUNT << " points: " << static_cast<double>(total_time.count()) / launches <<" ms" << std::endl;
    if (static_cast<double>(bad_approx) / launches <= value_estimation) {
        std::cout << "The estimation is correct" << std::endl;
    } 
}