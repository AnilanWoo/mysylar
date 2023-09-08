#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <list>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include "singleton.h"

#define SYLAR_LOG_LEVEL(logger, level) \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent( \
        logger, level, __FILE__, __LINE__, sylar::GetThreadId(), 234, sylar::GetFiberId(), time(0)) \
    )).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)
#define SYLAR_LOG_NAME(name) sylar::Mgr::GetInstance()->getLogger(name)
namespace sylar {

class Logger;

// 日志等级
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);

};

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,const char *file, uint32_t line, uint32_t thread
    , uint32_t elapse, uint32_t fiber, uint64_t time);

    const char* getFile() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThread() const { return m_threadid; }
    uint32_t getFiber() const { return m_fiberid; }
    uint64_t getTime() const { return m_time; }
    const std::string getContent() const { return m_ss.str(); }
    std::stringstream& getSS() { return m_ss; }
    // void setContext(std::string context) { m_context = context; } 
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    LogLevel::Level getLevel() const { return m_level; }
private:
    const char  *m_file    = nullptr;
    uint32_t    m_line     = 0;
    uint32_t    m_elapse   = 0;          // 程序启动到现在的毫秒数
    uint32_t    m_threadid = 0;
    uint32_t    m_fiberid  = 0;
    uint64_t    m_time     = 0;
    LogLevel::Level m_level;

    std::stringstream m_ss; // 日志输出流 
    std::shared_ptr<Logger> m_logger;
};

class LogEventWrap {
public:
    typedef std::shared_ptr<LogEventWrap> ptr;
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    std::stringstream &getSS() const { return m_event->getSS(); }
private:
    LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter {

public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string &pattern);
    LogFormatter();

    class FormatterItem {
    public:
        typedef std::shared_ptr<FormatterItem> ptr;
        virtual ~FormatterItem() {}
        virtual void format(std::ostream &os, std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };


   std::string format(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event);
   std::string getPartten() const { return m_pattern; }
    
private:
   std::string m_pattern;
    std::vector<FormatterItem::ptr> m_items;
   void init();
};


// 日志输出地，到时候由子类来进行继承
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;

    virtual ~LogAppender() {}
    // 定义纯虚函数，子类将其重写 
    virtual void log(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    
    void setFormatter(LogFormatter::ptr formatter);

    LogFormatter::ptr getFormatter() const { return m_formatter; }
    void setLevel(LogLevel::Level);
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;

};

// 输出到控制台的StdoutAppender
class StdoutLoggerAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLoggerAppender> ptr;

    void log(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) override;
};

// 输出到文件的FileAppender
class FileoutLoggerAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileoutLoggerAppender> ptr;
    void log(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) override;
    
    // 构造函数，需要一个文件名
    FileoutLoggerAppender(const std::string &filename);

    void reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};


// 日志输出器
class Logger : public std::enable_shared_from_this<Logger>{
// frined class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");

    // Logger类的核心函数
    void log(LogLevel::Level level, LogEvent::ptr event);

    // 以下五个函数都调用log方法进行输出
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();


    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }

    void setFormatter(const std::string pattern);
    void setFormatter(LogFormatter::ptr formatter);


    std::string getName() const { return m_name; }
private:
    std::string m_name;                       // 日志名称
    LogLevel::Level m_level;                  // 日志级别
    LogFormatter::ptr m_formatter;
    std::list<LogAppender::ptr>  m_appenders; // 一个logger有一串Appender，这里用list管理       
};

class LoggerManager {
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string &name);
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root; // 主日志器

};

typedef sylar::Singleton<LoggerManager> Mgr;

}


#endif
