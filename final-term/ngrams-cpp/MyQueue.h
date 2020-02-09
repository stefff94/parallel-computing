//
// Created by Stefano Vannucchi on 23/12/2019.
//

#ifndef BIGRAM_FINAL_TERM_CPP_MYQUEUE_H
#define BIGRAM_FINAL_TERM_CPP_MYQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

class MyQueue {

private:
    std::queue<std::string> data_queue;
    std::mutex mut;
    std::condition_variable data_cond;

public:

    /**
     * Method used to push a new value into the shared queue,
     * with a mutually exclusive execution.
     *
     * @param new_value: string to insert
     */
    void push(const std::string& new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    /**
     * Method used to pop a value from the shared queue.
     * If it is empty, the thread wait until it is notified.
     *
     * @param value: reference to string object to insert the extracted value.
     */
    void wait_and_pop(std::string& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]{return !data_queue.empty();});
        value = data_queue.front();
        data_queue.pop();
    }

    /**
     * Method used to know if the queue is empty, in mutually exclusive mode.
     *
     * @return
     */
    bool empty()
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

#endif //BIGRAM_FINAL_TERM_CPP_MYQUEUE_H

