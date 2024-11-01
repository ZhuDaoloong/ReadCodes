#ifndef __LOONGSERVER_NONCOPYABLE_H__
#define __LOONGSERVER_NONCOPYABLE_H__

namespace loongserver{
  class Noncopyable{
  public:
    /**
     * @brief default constructor and deconstructor
     */
    Noncopyable() = default;
    ~Noncopyable() = default;
    /**
     * @brief delete copy constructor and assignment function
     */
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
  };
}

#endif