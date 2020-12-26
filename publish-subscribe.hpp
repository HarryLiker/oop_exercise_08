#pragma once

#include <mutex>
#include <queue>

template <class T>
class PublishSubscribe {
private:
    std::mutex QueueOfMessages;
    std::queue<T> QueueMutex;
public:
    explicit PublishSubscribe() noexcept: QueueOfMessages(), QueueMutex() {}
    bool Empty() {
        QueueMutex.lock();
        bool result = QueueOfMessages.front();
        QueueMutex.unlock();
        return result;
    }

    T Front() {
        QueueMutex.lock();
        T top_element = QueueOfMessages.front();
        QueueMutex.unlock();
        return top_element;
    }

    void Push(cosnt T &message) {
        QueueMutex.lock();
        QueueOfMessages.push(message);
        QueueMutex.unlock();
    }

    void Pop() {
        QueueMutex.lock();
        QueueOfMessages.pop();
        QueueMutex.unlock();
    }
    
    ~PublishSubscribe() {}
};