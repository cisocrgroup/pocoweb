#ifndef pcw_Logger_hpp__
#define pcw_Logger_hpp__

#include <syslog.h>
#include <crow/logging.h>

namespace pcw {
	class Syslogger: public crow::ILogHandler {
	public:
		Syslogger() {
			openlog("pcwd", LOG_NDELAY, LOG_USER);
		}
		~Syslogger() noexcept {
			closelog();
		}
		virtual void log(std::string message,
				crow::LogLevel level) override {
			syslog(loglevel(level), "%s", message.data());
		}

	private:
		static int loglevel(crow::LogLevel level) noexcept {
			switch (level) {
			case crow::LogLevel::Error:
				return LOG_ERR;
			case crow::LogLevel::Warning:
				return LOG_WARNING;
			case crow::LogLevel::INFO:
				return LOG_INFO;
			case crow::LogLevel::DEBUG:
				return LOG_DEBUG;
			case crow::LogLevel::CRITICAL:
				return LOG_CRIT;
			default:
				return LOG_CRIT;
			}
		}
	};
}

#endif // pcw_Logger_hpp__
