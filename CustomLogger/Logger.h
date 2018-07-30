/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   13-07-2018
* @Purpose: manages logging operations
			class makes use of Boost's logging capabilities
*/

#include <atomic>
#include <thread>
#include <memory>
#include <iostream>
#include <map>

#include <boost/lockfree/queue.hpp>

#include "LoggerUtil.h"

namespace CustomLogger
{
	using EnabledLog_Map = std::map<LogLevel, bool>;
	using EnabledLog_Iter = EnabledLog_Map::iterator;
	using EnabledLog_Pair = std::pair<LogLevel, bool>;

	class LOGGER_DLL Logger
	{
	public:
		Logger(std::string logFileName, std::string logFolderPath = DEFAULT_LOG_PATH);

		virtual ~Logger();

		void enableLogLevel(LogLevel logLevel, bool enable = true);
		void stopLogger();

		void writeToLog(LogLevel, std::string);

	private:
		void init();
		void initBoostLogger();

		void consumeLoggedMessages(); //run by consumerThread seperately!

		std::string getLogLevelAsString(LogLevel logLevel);

		EnabledLog_Map m_logLevelMap;
		std::atomic<bool> m_threadExited;
		std::atomic<bool> m_loggingFinished;
		boost::lockfree::queue<LogData*, boost::lockfree::capacity<QUEUE_CAPACITY>> m_logQueue;
		std::shared_ptr<std::thread> m_consumerThread;
		std::string m_folderPath;
		std::string m_fileName;
		bool m_demoTestMode;
	};
	using Logger_Ptr = std::shared_ptr<Logger>;
}

