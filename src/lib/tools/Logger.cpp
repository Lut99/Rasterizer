/* LOGGER.cpp
 *   by Lut99
 *
 * Created:
 *   25/07/2021, 14:11:06
 * Last edited:
 *   24/08/2021, 21:47:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Logger class, which logs different things based on the
 *   severity level.
**/

#include <iostream>

#include "Logger.hpp"

using namespace std;
using namespace Tools;


/***** GLOBALS *****/
/* Global instance of the Logger everyone uses. */
Logger logger(std::cout, std::cerr, Verbosity::none);





/***** LOGGER::FATAL CLASS *****/
/* Constructor for the Fatal exception, which takes a message describing what happened. */
Logger::Fatal::Fatal(const std::string& message) :
    message(message)
{}





/***** LOGGER CLASS *****/
/* Constructor for the Logger class, which takes an output stream to write its non-messages to, an output stream to write error messages to and a verbosity level. */
Logger::Logger(std::ostream& stdos, std::ostream& erros, Verbosity verbosity) :
    stdos(&stdos),
    erros(&erros),
    verbosity(verbosity)
{}

/* Copy constructor for the Logger class. */
Logger::Logger(const Logger& other) :
    stdos(other.stdos),
    erros(other.erros),

    verbosity(other.verbosity)
{}

/* Move constructor for the Logger class. */
Logger::Logger(Logger&& other) :
    stdos(std::move(other.stdos)),
    erros(std::move(other.erros)),

    verbosity(std::move(other.verbosity))
{}

/* Destructor for the Logger class. */
Logger::~Logger() {}



/* Sets the output stream for this Logger. */
void Logger::set_output_stream(std::ostream& new_os) {
    // Get the lock for this Logger
    std::unique_lock<std::mutex> local_lock(this->lock);

    // Set the output stream
    this->stdos = &new_os;
}

/* Sets the error stream for this Logger. */
void Logger::set_error_stream(std::ostream& new_os) {
    // Get the lock for this Logger
    std::unique_lock<std::mutex> local_lock(this->lock);

    // Set the error stream
    this->erros = &new_os;
}

/* Sets the internal verbosity to the given value. */
void Logger::set_verbosity(Verbosity new_value) {
    // Get the lock for this Logger
    std::unique_lock<std::mutex> local_lock(this->lock);

    // Set the error stream
    this->verbosity = new_value;
}



/* Swap operator for the Logger class. */
void Tools::swap(Logger& l1, Logger& l2) {
    using std::swap;

    swap(l1.stdos, l2.stdos);
    swap(l1.erros, l2.erros);
    swap(l1.verbosity, l2.verbosity);
}
