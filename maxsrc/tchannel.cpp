//
// Created by max on 07.07.2020.
//

#ifndef WRITELEVEL_CHANNELS_H
#define WRITELEVEL_CHANNELS_H
#include <condition_variable>
#include <queue>
#include <mutex>

template<class T> class channel_unlimited {
    std::queue<T>           inp;
    std::mutex              m;
    std::condition_variable cv;
    bool                    closed=false;
public:
    bool read(T &data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.empty()) {
            if(closed) {
                cv.notify_one();
                return false;
            }
            else
                cv.wait(lk);
        }

        data=inp.front();
        inp.pop();
        lk.unlock();
        cv.notify_one();  // optional for single reader
        return true;
    }

    bool send(T data) {
        std::lock_guard<std::mutex> lk(m);
        if(closed) return false;
        inp.push(data);
        cv.notify_one();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lk(m);
        closed=true;
    }

};

template<class T, int limit> class channel_limited {
    std::queue<T>               inp;  // TODO: array queue ??
    std::mutex                  m;
    std::condition_variable     waitWrite,
            waitRead;
    bool                        closed=false;

public:
    bool read(T &data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.empty())
            if(closed) {
                waitWrite.notify_one();
                return false;
            } else
                waitWrite.wait(lk);
        data=inp.front();
        inp.pop();
        if(!inp.empty()) {
            waitWrite.notify_one();  // optional for single reader
        }
        lk.unlock();
        waitRead.notify_one();
        return true;
    }

    bool send(T data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.size()>limit) if(closed) return false; else waitRead.wait(lk);
        inp.push(data);
        if(!inp.empty()) {
            waitWrite.notify_one();  // optional for single reader
        }
        lk.unlock();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lk(m);
        closed=true;
    }

};

#endif //WRITELEVEL_CHANNELS_H
