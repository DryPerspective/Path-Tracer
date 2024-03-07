#ifndef DP_THREADPOOL
#define DP_THREADPOOL

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <atomic>
#include <type_traits>

namespace dp {


    /*
    *   A very basic thread-safe queue.
    *   Serial access using a lock on members rather than attemping to engineer a more complex concurrent
    *   solution, or even a lock-free. But it performs acceptably for this use-case.
    */
    template<typename T>
    class ts_queue {
        std::queue<T> m_dat;

        mutable std::mutex m_mut;
        std::condition_variable m_cond;

    public:
        ts_queue() = default;
        ts_queue(const ts_queue& rhs) {
            auto lck{ std::lock_guard{rhs.m_mut} };
            m_dat = rhs.m_dat;
        }

        ts_queue& operator=(const ts_queue& rhs) {
            if (this != &rhs) {
                auto lck{ std::scoped_lock{m_mut, rhs.m_mut} };
                m_dat = rhs.m_dat;
            }
            return *this;
        };

        void push(T in_val) {
            auto lck{ std::lock_guard{m_mut} };
            m_dat.push(std::move(in_val));
            m_cond.notify_one();
        }

        //Bind to a given value via an in-param
        //Avoids awkward race conditions
        //Wait_pop to block until there is a value to get
        void wait_pop(T& in_val) {
            auto lck{ std::unique_lock{m_mut} };
            m_cond.wait(lck, [this]() {return !m_dat.empty(); });
            in_val = std::move(m_dat.front());
            m_dat.pop();
        }

        //Try_pop to succeed or fail getting a value without waiting
        bool try_pop(T& in_val) {
            auto lck{ std::lock_guard{m_mut} };
            if (m_dat.empty()) return false;
            in_val = std::move(m_dat.front());
            m_dat.pop();
            return true;
        }

        bool empty() const {
            auto lck{ std::lock_guard{m_mut} };
            return m_dat.empty();
        }

    };

    //We need our own function wrapper class so we can use packaged_task and futures
    //more easily, as packaged tasks are moveable but not copyable
    //Switch to std::move_only_function when available (C++23)
    class move_function_wrapper {
        struct base {
            virtual void exec() = 0;
            virtual ~base() = default;
        };
        template<typename Func>
        struct impl : base {
            Func m_f;
            template<typename F = Func>
            impl(F&& in_func) : m_f{ std::forward<F>(in_func) } {}
            void exec() override {
                std::invoke(m_f);
            }
        };

        std::unique_ptr<base> m_func;

    public:

        move_function_wrapper() = default;

        template<typename Func>
        move_function_wrapper(Func&& in_func) : m_func{ std::make_unique<impl<Func>>(std::forward<Func>(in_func)) } {}

        move_function_wrapper(move_function_wrapper&&) = default;
        move_function_wrapper& operator=(move_function_wrapper&&) = default;

        inline void operator()() { m_func->exec(); }
    };


    /*
    * And the thread pool. A bit crude, but functional for our purposes
    * Note that it does not support interdependent tasks
    */
    class thread_pool {

        std::atomic<bool> m_done;
        ts_queue<move_function_wrapper> m_queue;
        std::vector<std::thread> m_threads;

        void worker_thread() {
            while (!m_done.load(std::memory_order_acquire)) {
                move_function_wrapper current_task;
                if (m_queue.try_pop(current_task)) {
                    current_task();
                }
                else {
                    std::this_thread::yield();
                }
            }
        }

    public:
        thread_pool() : m_done(false) {
            try {
                const auto max_threads{ std::thread::hardware_concurrency() };
                m_threads.resize(max_threads);
                for (auto& thread : m_threads) {
                    thread = std::thread{ &thread_pool::worker_thread, this };
                }
            }
            catch (...) {
                m_done.store(true, std::memory_order_release);
                throw;
            }
        }
        ~thread_pool() {
            m_done.store(true, std::memory_order_release);
            for (auto& thread : m_threads) {
                thread.join();
            }
        }

        template<typename Func>
        auto submit(Func in_func) {
            using result_t = std::invoke_result_t<Func>;
            std::packaged_task<result_t()> task{ std::move(in_func) };
            auto res{ task.get_future() };
            m_queue.push(std::move(task));
            return res;
        }

    };

}


#endif
