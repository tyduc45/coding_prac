#include <iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<functional>

class ThreadB{
public:
    ThreadB()
    {
        worker = std::thread(&ThreadB::run, this);
    }
    ~ThreadB(){
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_one();
        worker.join();
        std::cout << "RAII神力：线程B析构函数调用完成\n";
    }
    void post_task(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(task);
        }
        cv.notify_one();
    }
private:
    void run(){
        int counter = 0;
        while(true)
        {
            std::cout << "第" << counter + 1<< "次进入循环\n";
        
            std::function<void()> task;
        
            {
                
                //wait for thread stop or has tasks comming in queue.
                std::unique_lock<std::mutex> lock(mtx);
                
                cv.wait(lock, [this]()
                        { return stop || !tasks.empty(); });
            
                if (stop && tasks.empty())
                {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
                
            }
            counter++;
            task();
        }
    }
    std::thread worker;
    std::condition_variable cv;
    std::mutex mtx;
    std::queue<std::function<void()>> tasks;
    bool stop;
};

void method(int val)
{
    std::cout << "B 线程正在执行方法，参数为: " << val
              << " 线程ID: " << std::this_thread::get_id() << std::endl;
}

int main()
{
    ThreadB objB;

    // A 线程（主线程）想调用 B 的方法
    std::cout << "A 线程 ID: " << std::this_thread::get_id() << std::endl;

    objB.post_task([]()
                  { method(42); });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
