#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include <ucontext.h>

namespace loongserver {
  class Scheduler;

  class Fiber : public std::enable_shared_from_this<Fiber> {
    friend class Scheduler;
  public:
    using spFIBER = std::shared_ptr<Fiber>;

    /**
     * @brief fiber status
     */
    enum State {
      INIT,
      HOLD,
      EXEC,
      TERM,
      READY,
      EXCEPT
    };
  private:
    /**
     * @brief constructor without params
     * @attention the first fiber each thread
     */
    Fiber();

  public:
    /**
     * @brief constructor with params
     * @param[in] function
     * @param[in] stack size
     * @param[in] be called on main fiber
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    /**
     * @brief deconstructor
     */
    ~Fiber();

    /**
     * @brief reset status
     * @pre getStatus() == INIT/ TERM/ EXCEPT
     * @post getStatus() == INIT
     */
    void reset(std::function<void()> cb);

    /**
     * @brief swap current fiber running
     * @pre getStatus() != EXEC
     * @post getStatus() == EXEC
     */
    void swapIn();

    /**
     * @brief swap current fiber to background
     * @pre getStatus == EXEC
     * @post getStatus != EXEC
     */
    void swapOut();

    /**
     * @brief swap main fiber running
     * @pre for the main fiber from one thread
     */
    void call();

    /**
     * @brief swap current fiber background, then switch to main fiber
     * @pre work on current fiber
     * @post back to main fiber in thread
     */
    void back();

    /**
     * @brief return fiber id
     */
    uint64_t getId() const {return m_id;}

    /**
     * @brief return fiber state
     */
    State getState() const {return m_state;}

  public:
    /**
     * @brief set fiber of current thread
     */
    static void SetThis(Fiber* f);

    /**
     * @brief get fiber of current thread
     */
    static Fiber::spFIBER GetThis();

    /**
     * @brief swap current fiber to background, then set it to READY
     * @post getStatus() = READY 
     */
    static void YieldToReady();

    /**
     * @brief swap current fiber to background, then set it to HOLD
     * @post getStatus() = HOLD
     */
    static void YieldToHold();

    /**
     * @brief return total numbers of fibers
     */
    static uint64_t TotalFibers();

    /**
     * @brief execution function of fiber
     * @post back to main fiber after excution completes
     */
    static void MainFunc();

    /**
     * @brief execution function of fiber
     * @post back to thread caller after excution completes
     */
    static void CallerMainFunc();

    /**
     * @brief get the id of current fiber
     */
    static uint64_t GetFiberId();
  private:
    /// @brief fiber id
    uint64_t m_id = 0;
    /// @brief stack size 
    uint32_t m_stacksize = 0;
    /// @brief fiber status
    State m_state = INIT;
    /// @brief fiber context
    ucontext_t m_ctx;
    /// @brief fiber running stack pointer
    void* m_stack = nullptr;
    /// @brief fiber execution function
    std::function<void()> m_cb;
  };
}

#endif
