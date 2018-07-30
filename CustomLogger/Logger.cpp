//***Example code snippet taken from http ://stackoverflow.com/questions/39247778/boost-log-how-to-prevent-the-output-will-be-duplicated-to-all-added-streams-whe

#include "Logger.h"

#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <chrono>

namespace bl = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

namespace CustomLogger
{
#define LOG_NAME_SUFFIX "_%d-%m-%Y.log"
#define START_INDICATOR "_ENTRANCE_"
#define ARROW_STR " -> "
#define TAB_STR "	"
#define SLASH '\\'

	BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string);

	using logger_type = bl::sources::severity_logger<bl::trivial::severity_level>;
	static logger_type boostLogger;

	const std::string log_format = "%Message%"; //"[%TimeStamp%] -> %Message%";  //"[%TimeStamp%] (%LineID%) [%Severity%] [%Tag%]: %Message%";

	Logger::Logger(std::string logFileName, std::string logFolderPath) :
		m_threadExited(false),
		m_loggingFinished(false),
		m_folderPath(logFolderPath),
		m_fileName(logFileName)
	{
		if (m_folderPath.back() != SLASH)
			m_folderPath.push_back(SLASH);

		init();
		initBoostLogger();

		BOOST_LOG_SCOPED_THREAD_TAG("Tag", logFileName);
		BOOST_LOG_SEV(boostLogger, bl::trivial::info)
			<< "[" << boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()) << "]"
			<< ARROW_STR << START_INDICATOR;

		m_consumerThread = std::shared_ptr<std::thread>(new std::thread(&Logger::consumeLoggedMessages, this));
		m_consumerThread->detach();
	}

	Logger::~Logger()
	{
		m_loggingFinished = true;
		m_logLevelMap.clear();
		m_consumerThread.reset();
	}

	void Logger::init()
	{
		//by default all the log levels are disabled!
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Entry, false));
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Debug, false));
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Information, false));
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Warning, false));
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Error, false));
		m_logLevelMap.insert(EnabledLog_Pair(LogLevel::Log_Exception, false));
	}

	void Logger::initBoostLogger()
	{
		std::string filePath = m_folderPath + m_fileName + LOG_NAME_SUFFIX;

		using backend_type = bl::sinks::text_file_backend;
		using sink_type = bl::sinks::synchronous_sink < backend_type >;

		auto backend = boost::make_shared<backend_type>
			(
				keywords::file_name = filePath,
				keywords::open_mode = (std::ios::out | std::ios::app),
				keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), //rotate to the new file at midnight!
				keywords::format = log_format,
				keywords::auto_flush = true
				);

		if (m_demoTestMode) //also, write the logs to the console!
			bl::add_console_log(std::cout, bl::keywords::format = log_format);

		auto sink = boost::make_shared<sink_type>(backend);
		sink->set_formatter(bl::parse_formatter(log_format));
		sink->set_filter(tag_attr == m_fileName);

		bl::core::get()->add_sink(sink);

		bl::add_common_attributes();
	}

	void Logger::enableLogLevel(LogLevel logLevel, bool enable)
	{
		if (enable)
		{
			EnabledLog_Iter iter = m_logLevelMap.find(logLevel);
			if (iter != m_logLevelMap.end())
			{
				iter->second = enable;
			}
		}
	}

	void Logger::stopLogger()
	{
		m_loggingFinished = true;
		while (!m_threadExited)
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	void Logger::writeToLog(LogLevel logLevel, std::string logMessage)
	{
		m_logQueue.push(
			new LogData(
				logLevel,
				boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()),
				std::this_thread::get_id(),
				logMessage));
	}

	void Logger::consumeLoggedMessages()
	{
		LogData* logDataPtr;
		while (!m_loggingFinished)
		{
			while (m_logQueue.empty() && !m_loggingFinished)
				std::this_thread::sleep_for(std::chrono::seconds(1));

			while (m_logQueue.pop(logDataPtr)) //returns true if pop successful & false if the queue is empty!
			{
				if (logDataPtr != NULL)
				{
					EnabledLog_Iter iter = m_logLevelMap.find(logDataPtr->m_logLevel);
					if (iter != m_logLevelMap.end() && iter->second) //that log level is enabled to be written into the log file!
					{
						//write the log into the log file!
						BOOST_LOG_SCOPED_THREAD_TAG("Tag", m_fileName);
						BOOST_LOG_SEV(boostLogger, bl::trivial::info)
							<< "[" << logDataPtr->m_timeStampStr << "]" << ARROW_STR
							<< getLogLevelAsString(logDataPtr->m_logLevel)
							<< "Thread{" << logDataPtr->m_threadID << "}"
							<< TAB_STR << logDataPtr->m_logMessage;
					}
				}
				delete logDataPtr;
			}
		}
		m_threadExited = true;
	}

	std::string Logger::getLogLevelAsString(LogLevel logLevel)
	{
		switch (logLevel)
		{
		case Log_Entry:
			return " --- Entry       ";
		case Log_Debug:
			return " --- Debug       ";
		case Log_Information:
			return " --- Information ";
		case Log_Warning:
			return " --- Warning     ";
		case Log_Error:
			return " *** Error       ";
		case Log_Exception:
			return " !!! Exception   ";
		default:
			return " ??? Default     ";
		}
	}
}
