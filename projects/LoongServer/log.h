#ifndef __LOONGSERVER_LOG_H__
#define __LOONGSERFER_LOG_H__

#include <iostream>
#include <memory>
#include <mutex>
#include <fstream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>

#include "thread.h"
#include "singleton.h"

/**
 * @brief put log into logger using stream
 */
inline void LOONGSERVER_LOG_LEVEL(std::shared_ptr<loongserver::Logger> logger, loongserver::LogLevel::Level level){
  if(logger->getLevel() <= level){
    loongserver::LogEventWrap(loongserver::LogEvent::spLE(new loongserver::LogEvent(logger, level, 
      __FILE__, __LINE__, 0, 
      loongserver::GetThreadId(),loongserver::GetFiberId(), time(0), loongserver::Thread::GetName()))).getSS();
  }
}

inline void LOONGSERVER_LOG_DEBUG(std::shared_ptr<loongserver::Logger> logger) { LOONGSERVER_LOG_LEVEL(logger, loongserver::LogLevel::DEBUG); }
inline void LOONGSERVER_LOG_INFO(std::shared_ptr<loongserver::Logger> logger) { LOONGSERVER_LOG_LEVEL(logger, loongserver::LogLevel::INFO); }
inline void LOONGSERVER_LOG_WARN(std::shared_ptr<loongserver::Logger> logger) { LOONGSERVER_LOG_LEVEL(logger, loongserver::LogLevel::WARN); }
inline void LOONGSERVER_LOG_ERROR(std::shared_ptr<loongserver::Logger> logger) { LOONGSERVER_LOG_LEVEL(logger, loongserver::LogLevel::ERROR); }
inline void LOONGSERVER_LOG_FATAL(std::shared_ptr<loongserver::Logger> logger) { LOONGSERVER_LOG_LEVEL(logger, loongserver::LogLevel::FATAL); }

/**
 * @brief put log into logger using formatter
 */
inline void LOONGSEVER_LOG_FMT_LEVEL(std::shared_ptr<loongserver::Logger> logger, loongserver::LogLevel::Level level, std::shared_ptr<loongserver::LogFormatter> fmt, ...) {
  if (logger->getLevel() <= level) {
    loongserver::LogEventWrap(loongserver::LogEvent::spLE(new loongserver::LogEvent(logger, level, 
      __FILE__, __LINE__, 0, 
      loongserver::GetThreadId(),loongserver::GetFiberId(), time(0), loongserver::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__);
  }
}

inline void LOONGSEVER_LOG_FMT_DEBUG(std::shared_ptr<loongserver::Logger> logger, std::shared_ptr<loongserver::LogFormatter> fmt, ...){
  LOONGSEVER_LOG_FMT_LEVEL(logger, loongserver::LogLevel::DEBUG, fmt, __VA_ARGS__);
}
inline void LOONGSEVER_LOG_FMT_INFO(std::shared_ptr<loongserver::Logger> logger, std::shared_ptr<loongserver::LogFormatter> fmt, ...){
  LOONGSEVER_LOG_FMT_LEVEL(logger, loongserver::LogLevel::INFO, fmt, __VA_ARGS__);
}
inline void LOONGSEVER_LOG_FMT_WARN(std::shared_ptr<loongserver::Logger> logger, std::shared_ptr<loongserver::LogFormatter> fmt, ...){
  LOONGSEVER_LOG_FMT_LEVEL(logger, loongserver::LogLevel::WARN, fmt, __VA_ARGS__);
}
inline void LOONGSEVER_LOG_FMT_ERROR(std::shared_ptr<loongserver::Logger> logger, std::shared_ptr<loongserver::LogFormatter> fmt, ...){
  LOONGSEVER_LOG_FMT_LEVEL(logger, loongserver::LogLevel::ERROR, fmt, __VA_ARGS__);
}
inline void LOONGSEVER_LOG_FMT_FATAL(std::shared_ptr<loongserver::Logger> logger, std::shared_ptr<loongserver::LogFormatter> fmt, ...){
  LOONGSEVER_LOG_FMT_LEVEL(logger, loongserver::LogLevel::FATAL, fmt, __VA_ARGS__);
}

/**
 * @brief get root logger
 */
inline auto LOONGSERVER_LOG_ROOT(){
  return loongserver::sLOGGERMGR::getInstance()->getRoot();
}

/**
 * @brief get logger by name
 */
inline auto LOONGSERVER_LOG_NAME(std::string name) {
  return loongserver::sLOGGERMGR::GetInstance()->getLogger(name);
}
namespace loongserver{
class Logger;
class LoggerManager;

/**
 * @brief LOG LEVEL
 */
class LogLevel {
public:
/**
 * @enum LEVEL
 * @brief LOG LEVEL ENUMERATION
 */
  enum Level {
    /** unkown */
    UNKONWN = 0,
    /** debug */
    DEBUG = 1,
    /** info */
    INFO = 2,
    /** warn */
    WARN = 3,
    /** error */
    ERROR = 4,
    /** fatal */
    FATAL = 5,
  };

  /**
   * @brief convert log level to content
   * @param[in] level
   */
  static const char* ToString(LogLevel::Level level);

  /**
   * @brief convert content to log
   * @param[in] string
   */
  static LogLevel::Level ToLog(const std::string& str);
};

class LogEvent {
public:
  using spLE = std::shared_ptr<LogEvent>;
  using LOGLEVEL = LogLevel::Level;

  /**
   * @brief constructor function
   * @param[in] total params are nine
   * @param[in] logger
   * @param[in] level
   * @param[in] file
   * @param[in] line
   * @param[in] elapse
   * @param[in] thread_id
   * @param[in] fiber_id
   * @param[in] time
   * @param[in] thread_name
   */
  LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);
  /**
   * @brief return filename
   */
  const char* getfile() const{ return m_file; };
  /**
   * @brief return line NO.
   */
  uint32_t getLine() const { return m_line; };
  /**
   * @brief return elapse
   */
  uint64_t getElapse() const { return m_elapse; };
  /**
   * @brief return thread id
   */
  uint32_t getThreadId() const { return m_threadId; };
  /**
   * @brief return fiber id
   */
  uint32_t getFiberId() const { return m_fiberId; };
  /**
   * @brief return time
   */
  uint64_t getTime() const { return m_time; };
  /**
   * @brief return thread name
   */
  const std::string& getThreadName() const { return m_threadName; };
  /**
   * @brief return LOG content
   */
  std::string getContent() const { return m_ss.str(); };
  /**
   * @brief return logger
   */
  std::shared_ptr<Logger> getLogger() const { return m_logger; };
  /**
   * @brief return log level
   */
  LOGLEVEL getLevel() const{ return m_level; };
  /**
   * @brief return string stream
   */
  std::stringstream& getSS() { return m_ss; }
  /**
   * @brief format written LOG content/ user interface
   */
  void format(const char* fmt, ...);
  /**
   * @brief format written LOG content/ actual implementation 
   */
  void format(const char* fmt, va_list al);

private:
  const char*       m_file {nullptr};
  int32_t           m_line {0};
  uint64_t          m_elapse {0};
  uint32_t          m_threadId {0};
  uint32_t          m_fiberId {0};
  uint64_t          m_time {0};
  std::string       m_threadName;
  std::shared_ptr<Logger>  m_logger;
  LOGLEVEL          m_level;
  std::stringstream m_ss;
};

class LogEventWrap {
public:
  LogEventWrap(LogEvent::spLE e);
  ~LogEventWrap();

  /**
   * @brief 返回日志事件
   */
  LogEvent::spLE getEvent() const{ return m_event; }
  /** 
   * @brief 返回日志内容流
   */
  std::stringstream& getSS();

private:
  LogEvent::spLE m_event;
};

class LogFormatter {
public:
  using spLF = std::shared_ptr<LogFormatter>;
  /**
   * @brief logformatter constructor
   * @param[in] pattern
   * @details 
     *  %m message
     *  %p level
     *  %r elapse
     *  %c log name
     *  %t thread id
     *  %n next line
     *  %d time
     *  %f file name
     *  %l line number
     *  %T tab
     *  %F fiber id
     *  %N thread name
     *
     *  default format "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
   */
  LogFormatter(const std::string& pattern);
  /**
   * @brief return formatted log content
   * @param[in] logger
   * @param[in] level
   * @param[in] event
   */
  std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::spLE event);
  std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level, LogEvent::spLE event);
public:
  class FormatItem {
    public:
      using spFI = std::shared_ptr<FormatItem>;
      virtual ~FormatItem() = default;
    public:
      /**
       * @brief foramt log stream
       */
      virtual void format(std::ostream& os, std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::spLE event) = 0;
  };
  /**
   * @brief init log template
   */
  void init();
  /**
   * @brief return true if error
   */
  bool isError() const {return m_error;}
  /**
   * @brief return pattern
   */
  const std::string getPattern() const {return m_pattern; }
private:
  std::string                   m_pattern;
  std::vector<FormatItem::spFI> m_items;
  bool                          m_error {false};
};

class LogAppender {
  friend class Logger;

public:
  using spLA = std::shared_ptr<LogAppender>;
  using MUTEXTYPE = Spinlock;

  virtual ~LogAppender() {};

  /**
   * @brief do log
   * @param[in] logger
   * @param[in] level
   * @param[in] event
   */
  virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::spLE event) = 0;
  /**
   * @brief convert to Yaml string
   */
  virtual std::string toYamlString() = 0;

  /**
   * @brief set formatter
   */
  void setFormatter(LogFormatter::spLF val);

  /**
   * @brief get formatter
   */
  LogFormatter::spLF getFormatter();

  /**
   * @brief get level
   */
  LogLevel::Level getLevel() const { return m_level; }

  /**
   * @brief set level
   */
  void setLevel(LogLevel::Level val) { m_level = val; }

protected:
  MUTEXTYPE           m_mutex;
  bool                m_hasFormatter {false};
  LogLevel::Level     m_level {LogLevel::DEBUG};
  LogFormatter::spLF  m_formatter;
};

class Logger : public std::enable_shared_from_this<Logger> {
  friend class LoggerManager;
public:
  using spLOGGER = std::shared_ptr<Logger>;
  using MUTEXTYPE = Spinlock;
  /**
   * @brief cosntructor logger
   */
  Logger(const std::string& name = "root");
  /**
   * @brief do log
   * @param[in] level
   * @param[in] event
   */
  void log(LogLevel::Level level, LogEvent::spLE event);
  /**
   * @brief write debug log
   * @param[in] event
   */
  void debug(LogEvent::spLE event);
  /**
   * @brief write info log
   * @param[in] event
   */
  void info(LogEvent::spLE event);
  /**
   * @brief write warn log
   * @param[in] event
   */
  void warn(LogEvent::spLE event);
  /**
   * @brief write error log
   * @param[in] event
   */
  void error(LogEvent::spLE event);
  /**
   * @brief write fatal log
   * @param[in] event
   */
  void fatal(LogEvent::spLE event);
  /**
   * @brief add log appender
   * @param[in] appender
   */
  void addAppender(LogAppender::spLA appender);
  /**
   * @brief delete log appender
   * @param[in] appender
   */
  void delAppender(LogAppender::spLA appender);
  /**
   * @brief delete all appenders
   */
  void clearAppenders();
  /**
   * @brief return level
   */
  LogLevel::Level getLevel() const { return m_level; }
  /**
   * @brief set level
   */
  void setLevel(LogLevel::Level val) { m_level = val; }
  /**
   * @brief return log name
   */
  const std::string& getName() const { return m_name; }
  /**
   * @brief set log formatter
   * @param[in] formatter
   */
  void setFormatter(LogFormatter::spLF val);
  /**
   * @brief set log foramt template
   * @param[in] string
   */
  void setFormatter(const std::string& val);
  /**
   * @brief get formatter
   */
  LogFormatter::spLF getFormatter();
  /**
   * @brief convert formatter to yaml string
   */
  std::string toYamlString();

private:
  std::string                   m_name;
  LogLevel::Level               m_level;
  MUTEXTYPE                     m_mutex;
  std::list<LogAppender::spLA>  m_appenders;
  LogFormatter::spLF            m_formatter;
  Logger::spLOGGER              m_root;
};
/**
 * @brief output to the console
 */
class StdoutLogAppender : public LogAppender {
public:
  using spSA = std::shared_ptr<StdoutLogAppender>;
  void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::spLE event) override;
  std::string toYamlString() override;
};

/**
 * @brief output to the file
 */
class FileLogAppender : public LogAppender {
public:
  using spFA = std::shared_ptr<FileLogAppender>;
  FileLogAppender(const std::string& filename);
  void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::spLE event) override;
  std::string toYamlString() override;
  /**
   * @brief reopen log file
   * @return success -> true
   */
  bool reopen();

private:
  std::string m_filename;
  std::ofstream m_filestream;
  uint64_t m_lastTime = 0;
};

class LoggerManager {
public:
  using MUTEXTYPE = Spinlock;

  LoggerManager();
  /**
   * @brief get logger
   * @param[in] string name of logger
   */
  Logger::spLOGGER getLogger(const std::string& name);
  /**
   * @brief init
   */
  void init();
  /**
   * @brief return root logger
   */
  Logger::spLOGGER getRoot() const{ return m_root; }
  /**
   * @brief convert all logger config to yaml
   */
  std::string toYamlString();

private:
  MUTEXTYPE                               m_mutex;
  std::map<std::string, Logger::spLOGGER> m_loggers;
  Logger::spLOGGER                        m_root;
};

using sLOGGERMGR = loongserver::Singleton<LoggerManager>;

}
#endif