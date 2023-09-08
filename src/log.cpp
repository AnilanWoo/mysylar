#include "log.h"
#include "util.h"
// #include "config.h"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <functional>
#include <cstdio>

namespace sylar {

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, uint32_t line, uint32_t thread
    , uint32_t elapse, uint32_t fiber, uint64_t time)
    : m_logger(logger)
    , m_level(level)
    , m_file(file)
    , m_line(line)
    , m_threadid(thread)
    , m_elapse(elapse)
    , m_fiberid(fiber)
    , m_time(time) {}


Logger::Logger(const std::string& name)
    :m_name(name)
    ,m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("NAME:%c%T TIME:%d{%Y-%m-%d %H:%M:%S}%T ELAPSE:%r%T THREADID:%t%T THRADNMAE:%N%T FIBERID:%F%T LEVEL:[%p]%T FILE_LINE:%f:%l%T%m%n"));
}

class MessageFormatterItem : public LogFormatter::FormatterItem {
public:
    MessageFormatterItem(const std::string& str = "") {}
    // ~MessageFormatterItem () override {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatterItem : public LogFormatter::FormatterItem {
public:
    LevelFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatterItem : public LogFormatter::FormatterItem {
public:
    ElapseFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatterItem : public LogFormatter::FormatterItem {
public:
    NameFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatterItem : public LogFormatter::FormatterItem {
public:
    ThreadIdFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThread();
    }
};

class FiberIdFormatterItem : public LogFormatter::FormatterItem {
public:
    FiberIdFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiber();
    }
};

class ThreadNameFormatterItem : public LogFormatter::FormatterItem {
public:
    ThreadNameFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThread();
    }
};

class DateTimeFormatterItem : public LogFormatter::FormatterItem {
public:
    DateTimeFormatterItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatterItem : public LogFormatter::FormatterItem {
public:
    FilenameFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatterItem : public LogFormatter::FormatterItem {
public:
    LineFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatterItem : public LogFormatter::FormatterItem {
public:
    NewLineFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};


class StringFormatterItem : public LogFormatter::FormatterItem {
public:
    StringFormatterItem(const std::string& str)
        :m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatterItem : public LogFormatter::FormatterItem {
public:
    TabFormatterItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};


const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string& str) {
    #define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
    #undef XX
}


struct LogAppenderDefine {
    int type = 0; // File: 1, Stdut: 2
    LogLevel::Level level;
    std::string formatter;
    std::string filename;

    bool operator==(const LogAppenderDefine &oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && filename == oth.filename;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine &oth) const {
        return name == oth.name 
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine &oth) const {
        return name < oth.name;
    }
};

// template<>
// class LexicalCast<std::string, std::set<LogDefine> > {
// public:
//     std::set<LogDefine> operator()(const std::string& v) {
//         YAML::Node node = YAML::Load(v);
//         std::set<LogDefine> vec;
//         for(size_t i = 0; i < node.size(); i++) {
//             auto n = node[i];
//             if(!n["name"].IsDefined()) {
//                 std::cout << "log config error: name is null, " << n
//                         << std::endl;
//                 throw std::logic_error("log config name is null");
//             }
//             LogDefine ld;
//             ld.name = n["name"].as<std::string>();
//             // ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
//             if(n["formatter"].IsDefined()) {
//                 ld.formatter = n["formatter"].as<std::string>();
//             }

//             if(n["appenders"].IsDefined()) {
//                 //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
//                 for(size_t x = 0; x < n["appenders"].size(); ++x) {
//                     auto a = n["appenders"][x];
//                     if(!a["type"].IsDefined()) {
//                         std::cout << "log config error: appender type is null, " << a
//                                 << std::endl;
//                         continue;
//                     }
//                     std::string type = a["type"].as<std::string>();
//                     LogAppenderDefine lad;
//                     if(type == "FileLogAppender") {
//                         lad.type = 1;
//                         if(!a["file"].IsDefined()) {
//                             std::cout << "log config error: fileappender file is null, " << a
//                                 << std::endl;
//                             continue;
//                         }
//                         lad.filename = a["file"].as<std::string>();
//                         if(a["formatter"].IsDefined()) {
//                             lad.formatter = a["formatter"].as<std::string>();
//                         }
//                     } else if(type == "StdoutLogAppender") {
//                         lad.type = 2;
//                         if(a["formatter"].IsDefined()) {
//                             lad.formatter = a["formatter"].as<std::string>();
//                         }
//                     } else {
//                         std::cout << "log config error: appender type is invalid, " << a
//                                 << std::endl;
//                         continue;
//                     }

//                     ld.appenders.push_back(lad);
//                 }
//             }
//             vec.insert(ld);
//         }

//         return vec;
//     }
// };

// template<>
// class LexicalCast<std::set<LogDefine>, std::string> {
// public:
//     std::string operator()(const std::set<LogDefine>& v) {
//         YAML::Node node;
//         for(auto& i : v) {
//             YAML::Node n;
//             n["name"] = i.name;
//             if(i.level != LogLevel::UNKNOW) {
//                 // n["level"] = LogLevel::ToString(i.level);
//             }
//             if(!i.formatter.empty()) {
//                 n["formatter"] = i.formatter;
//             }

//             for(auto& a : i.appenders) {
//                 YAML::Node na;
//                 if(a.type == 1) {
//                     na["type"] = "FileLogAppender";
//                     na["file"] = a.filename;
//                 } else if(a.type == 2) {
//                     na["type"] = "StdoutLogAppender";
//                 }
//                 if(a.level != LogLevel::UNKNOW) {
//                     // na["level"] = LogLevel::ToString(a.level);
//                 }

//                 if(!a.formatter.empty()) {
//                     na["formatter"] = a.formatter;
//                 }

//                 n["appenders"].push_back(na);
//             }
//             node.push_back(n);
//         }
//         std::stringstream ss;
//         ss << node;
//         return ss.str();
//     }
// };

// sylar::ConfigVar<std::set<sylar::LogDefine> >::ptr g_log_defines =
//     sylar::Config::Lookup("logs", std::set<sylar::LogDefine>{}, "logs config");

// struct LogIniter {
//     LogIniter() {
//         g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine> &old_value,
//                     const std::set<LogDefine> &new_value) {
//         // std::cout << "here" << std::endl;
                    
//         for(auto &i : new_value) {
//             auto it = old_value.find(i);
//             if (it == old_value.end()) {
//                 Logger::ptr logger(new Logger(i.name));
//                 logger->setLevel(i.level);
//                 if (!i.formatter.empty()) {
//                     logger->setFormatter(i.formatter);
//                 }
//                 logger->clearAppenders();
//                 for(auto &a : i.appenders) {
//                     LogAppender::ptr ap;
//                     if (a.type == 1) {
//                         ap.reset(new FileoutLoggerAppender{a.filename});
//                     } else if (a.type == 2) {
//                         ap.reset(new StdoutLoggerAppender);
//                     }
//                     ap->setLevel(a.level);
//                     logger->addAppender(ap);
//                 }
//             } else {
//                 if (!(i == *it)) {
//                     Logger::ptr logger = SYLAR_LOG_NAME(i.name);
//                     logger->setLevel(i.level);
//                     if (!i.formatter.empty()) {
//                         logger->setFormatter(i.formatter);
//                     }
//                     logger->clearAppenders();
//                     for(auto &a : i.appenders) {
//                         LogAppender::ptr ap;
//                         if (a.type == 1) {
//                             ap.reset(new FileoutLoggerAppender{a.filename});
//                         } else if (a.type == 2) {
//                             ap.reset(new StdoutLoggerAppender);
//                         }
//                         ap->setLevel(a.level);
//                         logger->addAppender(ap);
//                     }
//                 }
//             }
//             for(auto &i : old_value) {
//             auto it = new_value.find(i);
//             if (it == new_value.end()) {
//                 // 删除logger，新的里面没有，老得地方有
//                 auto logger = SYLAR_LOG_NAME(i.name);
//                 logger->setLevel((LogLevel::Level)100);
//                 logger->clearAppenders(); // 没有appender默认用root来输出
//             }
//         }
//         }});
//         // 新增
//         SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << "111";
//         }
// };

LogEventWrap::LogEventWrap(LogEvent::ptr e) 
    :  m_event(e){

}
LogEventWrap::~LogEventWrap() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    auto self = shared_from_this();
    if (level >= m_level) {
        for (auto& appender : m_appenders) {
            appender->log(self, level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender) {
    if (!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    m_appenders.clear();
}

void Logger::setFormatter(const std::string pattern) {
    LogFormatter::ptr formatter{new sylar::LogFormatter{pattern}};
    m_formatter = formatter;
}
void Logger::setFormatter(LogFormatter::ptr formatter) {
    m_formatter = formatter;
}

// Logger类的方法结束
// LoggerAppender类开始

void LogAppender::setLevel(LogLevel::Level level) {
    m_level = level;
}

void StdoutLoggerAppender::log (std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::cout << m_formatter->format(Logger, level, event);
    }
}

void FileoutLoggerAppender::log(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        // m_formatter->format()返回一个std::string类型
        m_filestream << m_formatter->format(Logger, level, event);
    }
}

void LogAppender::setFormatter(LogFormatter::ptr formatter) {
    m_formatter = formatter;
}

FileoutLoggerAppender::FileoutLoggerAppender(const std::string &filename)
 : m_filename(filename) {
    reopen();
}


void FileoutLoggerAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::out);
}

// Formatter的方法定义开始

LogFormatter::LogFormatter(const std::string &pattern) 
:m_pattern(pattern) {
    init();
}



std::string LogFormatter::format(std::shared_ptr<Logger> Logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto& item: m_items) {
        item->format(ss, Logger, level, event);
    }
    return ss.str();
}

// 对pattern的解析
void LogFormatter::init() {
// //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]); // 如果不是百分号，就放进去
            continue;
        }

        // 转义字符
        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            // isalpha表示是否为字字母
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                // 名称已经确定了
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            // 格式错误
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatterItem::ptr(const std::string& str)>> s_format_items = { 
#define XX(str, C) {#str, [](const std::string& fmt) { return FormatterItem::ptr(new C(fmt));}}

        XX(m, MessageFormatterItem),           //m:消息
        XX(p, LevelFormatterItem),             //p:日志级别
        XX(r, ElapseFormatterItem),            //r:累计毫秒数
        XX(c, NameFormatterItem),              //c:日志名称
        XX(t, ThreadIdFormatterItem),          //t:线程id
        XX(n, NewLineFormatterItem),           //n:换行
        XX(d, DateTimeFormatterItem),          //d:时间
        XX(f, FilenameFormatterItem),          //f:文件名
        XX(l, LineFormatterItem),              //l:行号
        XX(T, TabFormatterItem),               //T:Tab
        XX(F, FiberIdFormatterItem),           //F:协程id
        XX(N, ThreadNameFormatterItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatterItem::ptr(new StringFormatterItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatterItem::ptr(new StringFormatterItem("<<error_format %" + std::get<0>(i) + ">>")));
                // m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }

}
LoggerManager::LoggerManager() {
    m_root.reset(new Logger{"root"});
    m_root->addAppender(LogAppender::ptr{new StdoutLoggerAppender});
    m_loggers["root"] = m_root;
}

Logger::ptr LoggerManager::getLogger(const std::string &name) {
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }
    Logger::ptr logger{new Logger{name}};
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLoggerAppender));
    m_loggers[name] = logger;
    return logger;
}


}














