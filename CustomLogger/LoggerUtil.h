/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   13-07-2018
* @Purpose: implements utility structs and constant expressions
*/

#include <string>
#include <thread>

namespace CustomLogger
{
#if defined(_WIN32)
#   define DECLSPEC_EXPORT __declspec(dllexport)
#   define DECLSPEC_IMPORT __declspec(dllimport)
	//
	//  HAS_DECLSPEC_IMPORT_EXPORT defined only for compilers with distinct
	//  declspec for IMPORT and EXPORT
#   define HAS_DECLSPEC_IMPORT_EXPORT
#elif defined(__GNUC__)
#   define DECLSPEC_EXPORT __attribute__((visibility ("default")))
#   define DECLSPEC_IMPORT __attribute__((visibility ("default")))
#elif defined(__SUNPRO_CC)
#   define DECLSPEC_EXPORT __global
#   define DECLSPEC_IMPORT /**/
#else
#   define DECLSPEC_EXPORT /**/
#   define DECLSPEC_IMPORT /**/
#endif

#ifndef LOGGER_DLL
#   ifdef LOGGER_DLL_IMPORTS
#       define LOGGER_DLL LOGGER_DECLSPEC_IMPORT
#   elif defined(LOGGER_STATIC_LIBS)
#       define LOGGER_DLL /**/
#   else
#       define LOGGER_DLL DECLSPEC_EXPORT
#   endif
#endif

#define DEFAULT_LOG_PATH "C:\\CustomLogger\\"
#define QUEUE_CAPACITY 150

	enum LogLevel {
		Log_Entry,
		Log_Debug,
		Log_Information,
		Log_Warning,
		Log_Error,
		Log_Exception
	};

	struct LogData {
		LogData() :
			m_logLevel(Log_Debug), m_timeStampStr(""), m_threadID(), m_logMessage("") {}
		LOGGER_DLL LogData(LogLevel logLevel, const std::string& timeStampStr, std::thread::id threadID, const std::string& logMessage) :
			m_logLevel(logLevel), m_timeStampStr(timeStampStr), m_threadID(threadID), m_logMessage(logMessage) {}

		LogLevel m_logLevel;
		std::string m_timeStampStr;
		std::thread::id m_threadID;
		std::string m_logMessage;
	};
}
