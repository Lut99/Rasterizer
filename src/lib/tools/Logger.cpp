/* LOGGER.cpp
 *   by Lut99
 *
 * Created:
 *   25/07/2021, 14:11:06
 * Last edited:
 *   23/08/2021, 14:51:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Logger class, which logs different things based on the
 *   severity level.
**/

#include "Logger.hpp"

using namespace std;
using namespace Tools;


/***** LOGGER::FATAL CLASS *****/
/* Constructor for the Fatal exception, which takes a message describing what happened. */
Logger::Fatal::Fatal(const std::string& message) :
    message(message)
{}





/***** LOGGER CLASS *****/
/* Constructor for the Logger class, which takes an output stream to write its non-messages to, an output stream to write error messages to and a verbosity level. Optionally takes a default channel name. */
Logger::Logger(std::ostream& stdos, std::ostream& erros, Verbosity verbosity, const std::string& channel) :
    stdos(stdos),
    erros(erros),
    verbosity(verbosity),
    channel(channel)
{}

/* Constructor for the Logger class, which takes its setup data as an InitData struct. Optionally takes a default channel name. */
Logger::Logger(const InitData& init_data, const std::string& channel) :
    stdos(init_data.stdos),
    erros(init_data.erros),
    verbosity(init_data.verbosity),
    channel(channel)
{}

/* Copy constructor for the Logger class. */
Logger::Logger(const Logger& other) :
    stdos(other.stdos),
    erros(other.erros),

    verbosity(other.verbosity),
    channel(other.channel)
{}

/* Move constructor for the Logger class. */
Logger::Logger(Logger&& other) :
    stdos(std::move(other.stdos)),
    erros(std::move(other.erros)),

    verbosity(std::move(other.verbosity)),
    channel(std::move(other.channel))
{}

/* Destructor for the Logger class. */
Logger::~Logger() {}



/* Swap operator for the Logger class. */
void Tools::swap(Logger& l1, Logger& l2) {
    using std::swap;

    #ifndef NDEBUG
    if (&l1.stdos != &l2.stdos) { throw std::runtime_error("Cannot swap loggers with different output streams."); }
    if (&l1.erros != &l2.erros) { throw std::runtime_error("Cannot swap loggers with different error streams."); }
    #endif

    swap(l1.verbosity, l2.verbosity);
    swap(l1.channel, l2.channel);
}
