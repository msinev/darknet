//
// Created by max on 07.07.2020.
// multiple consumers are ok
// multiple producers should work as well but close operation is tricky
// my comp benchmarks: 1-10M tps depending on number of consumers (multiple is a bit slower due to sync)
// and size of data
//
#ifndef WRITELEVEL_CHANNELS_H
#define WRITELEVEL_CHANNELS_H
#include <condition_variable>
#include <queue>
#include <mutex>
//#include <cassert>

template<class T> class channel_unlimited {
    std::queue<T>           inp;
    std::mutex              m;
    std::condition_variable cv;
    volatile bool  closed=false;
public:
    bool read(T &data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.empty()) {
            if(closed) {
                cv.notify_one();
                lk.unlock();
                return false;
            }
            else
                cv.wait(lk);
        }

        data=inp.front();
        inp.pop();
        cv.notify_one();  // optional for single reader
        lk.unlock();
        return true;
    }

    bool send(T data) {
        std::lock_guard<std::mutex> lk(m);
        if(closed) {
            return false;
        }
        inp.push(data);
        cv.notify_one();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lk(m);
        closed=true;
        cv.notify_one();
    }

};

template<class T, int limit, bool check=true> class circular {
    volatile T buf[limit];
    volatile int n=0, i=0;
public:
    inline bool get(T &t) {
        if(check) if(empty()) {  return false; }
        t=buf[i];
        i=(i+1)%limit;
        n--;
        return true;
    }

    inline bool put(T t) {
        if(check) if(available()<=0) {  return false; }
        buf[(i+n++)%limit]=t;
        return true;
    }

/*    bool put(T &&t) {
        if(check) if(available()<=0) return false;
        buf[(i+n++)%limit]=t;
        return true;
    }
*/

    inline int available() {
        return limit-n;
    }

    inline bool empty() {
        return n==0;
    }

};


template<class T, int limit> class channel_limited {
    circular<T, limit, false>   inp;
    std::mutex                  m;
    std::condition_variable     waitWrite,
            waitRead;
    volatile bool    closed=false;

public:
    bool read(T &data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.empty())
            if(closed) {
                waitRead.notify_one();
                lk.unlock();
                return false;
            } else
                waitRead.wait(lk);
        inp.get(data);
        if(inp.available()==1) {
            waitWrite.notify_one();
        }
        if(!inp.empty()) {
            waitRead.notify_one();
        }
        lk.unlock();
        return true;
    }

    bool send(T data) {
        std::unique_lock<std::mutex> lk(m);
        while(inp.available()<=0) {
            if (closed) {
                waitWrite.notify_one();
                lk.unlock();
                return false;
            } else
                waitWrite.wait(lk);
        }
        inp.put(data);
        waitRead.notify_one();  // optional for single writer
        lk.unlock();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lk(m);
        closed=true;
        waitWrite.notify_one();  // optional for single reader
        waitRead.notify_one();  // optional for single reader
    }

};

#endif //WRITELEVEL_CHANNELS_H
