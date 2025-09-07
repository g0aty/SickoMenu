#include "pch-il2cpp.h"
#include "logger.h"
#include <sstream>
#include <iostream>
#include "utility.h"

SickoLogger Log;

void SickoLogger::Create()
{
	const auto path = getModulePath(NULL);
	const auto logPath = path.parent_path() / "sicko-log.txt";
	const auto prevLogPath = path.parent_path() / "sicko-prev-log.txt";

	std::error_code errCode;
	std::filesystem::remove(prevLogPath, errCode);
	std::filesystem::rename(logPath, prevLogPath, errCode);
	std::filesystem::remove(logPath, errCode);

	this->filePath = logPath;
}

void SickoLogger::Write(std::string_view verbosity, std::string_view source, std::string_view message, bool write)
{
	std::stringstream ss;
	// FIXME: std::chrono::current_zone requires Windows 10 version 1903/19H1 or later.
	// ss << std::format("[{:%EX}]", std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()));
	/*auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm = {};
	localtime_s(&tm, &t);  // Safe version of localtime
	ss << std::put_time(&tm, "[%H:%M:%S]"); // Replace UTC time with local time*/ // Causes lag on lower-end devices
	ss << std::format("[{:%EX}]", std::chrono::system_clock::now());
	ss << "[" << verbosity << " - " << source << "] " << message << std::endl;
	std::cout << ss.str();

	if (write) {
		std::ofstream file(this->filePath, std::ios_base::app);
		file << ss.str();
		file.close();
	}
}

void SickoLogger::Debug(std::string_view source, std::string_view message, bool write)
{
	Write("DEBUG", source, message, write);
}

void SickoLogger::Error(std::string_view source, std::string_view message)
{
	Write("ERROR", source, message);
}

void SickoLogger::Info(std::string_view source, std::string_view message)
{
	Write("INFO", source, message);
}

void SickoLogger::Debug(std::string_view message)
{
	Debug("SICKO", message);
}

void SickoLogger::Error(std::string_view message)
{
	Error("SICKO", message);
}

void SickoLogger::Info(std::string_view message)
{
	Info("SICKO", message);
}