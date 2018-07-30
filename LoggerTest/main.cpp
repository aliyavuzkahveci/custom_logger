// LoggerTest.cpp : Defines the entry point for the console application.
//
#include <string>
#include <memory>
#include <Logger.h>

constexpr auto HELP = "-h";
constexpr auto FILE_ARG = "-fileName";
constexpr auto FOLDER_ARG = "-folderPath";
constexpr auto SPACE = 0x20;
constexpr auto ZERO = 0x30;
constexpr auto FIVE = 0x35;
constexpr auto UC_Q = 0x51;
constexpr auto UC_Y = 0x59;
constexpr auto LC_Q = 0x71;
constexpr auto LC_Y = 0x79;

int main(int argc, char* argv[])
{
	using namespace CustomLogger;

	//LoggerText.exe -h -> argument count is 2
	//LoggerTest.exe -fileName asdasdasdasd -folderPath dfsdfsdfsdf -> argument count is 5
	int returnValue = 0;

	if (argc == 2 && std::strcmp(argv[1], HELP) == 0)
	{
		returnValue = 1; //HELP requested!
	}
	else if (argc != 3 && argc != 5)
	{
		std::cout << "You have entered wrong inputs..." << std::endl;
	}
	else
	{
		std::string fileName, folderPath;
		bool folderGiven = false;
		if (argc == 3 && std::strcmp(argv[1], FILE_ARG) == 0)
		{
			fileName = std::string(argv[2]);
		}
		else if (argc == 5)
		{
			folderGiven = true;
			if (std::strcmp(argv[1], FILE_ARG) == 0) //-fileName
			{
				fileName = std::string(argv[2]);
				if (std::strcmp(argv[3], FOLDER_ARG) == 0) //-folderPath
				{
					folderPath = std::string(argv[4]);
				}
				else
				{
					std::cout << "Parameter(-folderPath) is missing!" << std::endl;
					returnValue = -1; //folderPath parameter missing!
				}
			}
			else if (std::strcmp(argv[3], FILE_ARG) == 0) //-fileName
			{
				fileName = std::string(argv[4]);
				if (std::strcmp(argv[1], FOLDER_ARG) == 0) //-folderPath
				{
					folderPath = std::string(argv[2]);
				}
				else
				{
					std::cout << "Parameter(-folderPath) is missing!" << std::endl;
					returnValue = -1; //folderPath parameter missing!
				}
			}
		}
		else
		{
			std::cout << "Parameter(-fileName) is missing!" << std::endl;
			returnValue = -2; //fileName parameter missing!
		}

		//Starting the execution of the real program!
		if (returnValue == 0) //parameters have been entered correctly!
		{
			Logger_Ptr logger;
			if(folderGiven)
				logger = Logger_Ptr(new Logger(fileName, folderPath));
			else
				logger = Logger_Ptr(new Logger(fileName));

			//enable log levels
			char yOrN = 0x00;
			std::cout << "Do you want to enable Log Level 5 (Exception)? [y/n]: ";
			std::cin >> yOrN;
			if(yOrN == LC_Y || yOrN == UC_Y)
				logger->enableLogLevel(LogLevel::Log_Exception);

			std::cout << "Do you want to enable Log Level 4 (Error)? [y/n]: ";
			std::cin >> yOrN;
			if (yOrN == LC_Y || yOrN == UC_Y)
				logger->enableLogLevel(LogLevel::Log_Error);

			std::cout << "Do you want to enable Log Level 3 (Warning)? [y/n]: ";
			std::cin >> yOrN;
			if (yOrN == LC_Y || yOrN == UC_Y)
				logger->enableLogLevel(LogLevel::Log_Warning);

			std::cout << "Do you want to enable Log Level 2 (Information)? [y/n]: ";
			std::cin >> yOrN;
			if (yOrN == LC_Y || yOrN == UC_Y)
				logger->enableLogLevel(LogLevel::Log_Information);

			std::cout << "Do you want to enable Log Level 1 (Debug)? [y/n]: ";
			std::cin >> yOrN;
			if (yOrN == LC_Y || yOrN == UC_Y)
				logger->enableLogLevel(LogLevel::Log_Debug);

			logger->enableLogLevel(LogLevel::Log_Entry);

			
			std::string received;
			std::cout
				<< "Please write \"Q\" to quit application..." << std::endl
				<< "Enter Log Level & Text to be written into the log file..." << std::endl 
				<< "Level & Text" << std::endl;

			std::getline(std::cin, received); //receives empty string due to std::endl in the previous line!

			while (std::getline(std::cin, received))
			{
				if (received.length() == 1 && (received.at(0) == UC_Q || received.at(0) == LC_Q))
					break;

				if (received.empty() || received.at(0) < ZERO || received.at(0) > FIVE || received.at(1) != SPACE)
				{
					std::cout << "Please select a valid Log Level (0 - 1 - 2 - 3 - 4 - 5)!" << std::endl;
					continue;
				}

				LogLevel logLevel = static_cast<LogLevel>(received.at(0) - ZERO);

				received = received.substr(1);
				unsigned int pos = received.find_first_not_of(SPACE);
				if (pos == std::string::npos)
				{
					std::cout << "Please enter a valid log message after the Log Level!" << std::endl;
					continue;
				}
				received = received.substr(pos);

				logger->writeToLog(logLevel, received);

				std::cout
					<< "Please write \"Q\" to quit application..." << std::endl
					<< "Enter Log Level & Text to be written into the log file..." << std::endl
					<< "Level & Text" << std::endl;
			}

			logger->stopLogger();
			logger.reset();
		}
	}

	if (returnValue != 0)
	{
		std::cout << "There 2 formats for the correct execution of the program:" << std::endl;
		std::cout << "	1. LoggerTest.exe -folderPath ~pathToFolder~ -fileName ~nameOfLogFile~" << std::endl;
		std::cout << "	2. LoggerTest.exe -fileName ~nameOfLogFile~" << std::endl;
	}

    return returnValue;
}

