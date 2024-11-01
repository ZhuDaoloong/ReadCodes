#include "fiber.h"
#include "config.h"
#include "log.h"

#include <atomic>

namespace loongserver{

  static Logger::spLOGGER g_logger = LOONGSERVER_LOG_NAME("system");

  static std::atomic<uint64_t> s_fiber_id {0};
  static std::atomic<uint64_t> s_fiber_count {0};

  static thread_local Fiber* t_fiber = nullptr;
  static thread_local Fiber::spFIBER t_threadFiber = nullptr;

  static ConfigVar<uint32_t>::spCV g_fiber_stack_size = 
    Config::Lookup<uin32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

  class MallocStackAllocator {
    public:
      static void* Alloc(size_t size) {
        return malloc(size);
      }

      static void Dealloc(void* vp, size_t size) {
        return free(vp);
      }
  };

  using StackAllocator = MallocStackAllocator;

  uint64_t Fiber::GetFiberId() {
    if(t_fiber){
      return t_fiber->getId();
    }
    return 0;
  }

  Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
      LOONGSERVER_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    LOONGSERVER_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
  }

  Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
      ++s_fiber_count;
      m_stacksize = stacksize? stacksize : g_fiber_stack_size->getValue();

      m_stack = StackAllocator::Alloc(m_stacksize);
      if(getcontext(&m_ctx)) {
        LOONGSERVER_ASSERT2(false, "getcontext");
      }
      m_ctx.uc_link = nullptr;
      m_ctx.uc_stack.ss_sp = m_stack;
      m_ctx.uc_stack.ss_size = m_stacksize;

      if(!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
      }else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
      }

      LOONGSERVER_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
    }


}
