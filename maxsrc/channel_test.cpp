#include <cstdint>
#include <iostream>
#include <chrono>
#include <netinet/in.h>
#include <thread>
#include "Channels.h"





int test1() {
    std::cerr << "Order test limited 10" << std::endl;
    channel_limited< int, 10> sender;
    std::thread reader([&sender]() { int i, n=0; while(sender.read(i)) {
                           std::cout << "Received 1 "<< i  << std::endl;
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                           n++;
                       }
                           std::cerr << "Exit reader 1 " << n << std::endl;
                       }
    );

    std::thread reader2([&sender]() { int i, n=0; while(sender.read(i)) {
                            std::cout << "Received 2 "<< i  << std::endl;
                            //                  std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            n++;
                        }
                            std::cout << "Exit reader 2 " << n << std::endl;
                        }
    );

// little endian if true

    auto t1 = std::chrono::high_resolution_clock::now();
    const long maxN=200;
    for (int i=0; i<maxN; i++) {
        sender.send(i);
        std::cout << "Sent "<< i  << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    sender.close();
    std::cout << "Done sender "  << std::endl;
    reader.join();
    reader2.join();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Complete in  "<< duration  << std::endl;

    return 0;
}


int test2() {
    std::cerr << "Performance test" << std::endl;
    channel_limited< int, 1000> sender;
    std::thread reader([&sender]() { int i, n=0; while(sender.read(i)) {
                           n++;
                       }
                       }
    );

    std::thread reader2([&sender]() { int i, n=0; while(sender.read(i)) {
                            n++;
                        }
                        }
    );

// little endian if true

    auto t1 = std::chrono::high_resolution_clock::now();
    const long maxN=200000;
    for (int i=0; i<maxN; i++) {
        sender.send(i);
    }

    sender.close();
    reader.join();
    reader2.join();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Complete in  "<< duration  << std::endl;
    std::cout << "n per microsecond  "<< maxN/(double)duration  << std::endl;



    return 0;
}

int test01() {
    std::cerr << "Order test" << std::endl;
    channel_unlimited< int> sender;
    std::thread reader([&sender]() { int i, n=0; while(sender.read(i)) {
                           std::cout << "Received 1 "<< i  << std::endl;
                           std::this_thread::sleep_for(std::chrono::milliseconds(1));
                           n++;
                       }
                           std::cerr << "Exit reader 1 " << n << std::endl;
                       }
    );

    std::thread reader2([&sender]() { int i, n=0; while(sender.read(i)) {
                            std::cout << "Received 2 "<< i  << std::endl;
                            //                  std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            n++;
                        }
                            std::cout << "Exit reader 2 " << n << std::endl;
                        }
    );

// little endian if true

    auto t1 = std::chrono::high_resolution_clock::now();
    const long maxN=200;
    for (int i=0; i<maxN; i++) {
        sender.send(i);
        std::cout << "Sent "<< i  << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    sender.close();
    std::cout << "Done sender "  << std::endl;
    reader.join();
    reader2.join();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Complete in  "<< duration  << std::endl;

    return 0;
}


int test02() {
    std::cerr << "Performance test unlimited" << std::endl;
    channel_unlimited<int> sender;
    std::thread reader([&sender]() { int i, n=0; while(sender.read(i)) {
                           n++;
                       }
                       }
    );

    std::thread reader2([&sender]() { int i, n=0; while(sender.read(i)) {
                            n++;
                        }
                        }
    );

// little endian if true

    auto t1 = std::chrono::high_resolution_clock::now();
    const long maxN=200000;
    for (int i=0; i<maxN; i++) {
        sender.send(i);
    }

    sender.close();
    reader.join();
    reader2.join();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Complete in  "<< duration  << std::endl;
    std::cout << "n per microsecond  "<< maxN/(double)duration  << std::endl;



    return 0;
}

int main() {
//    for(int i=0; i<1000; i++) {
    test01();
    test1();
    test02();
    test2();
//    }
}