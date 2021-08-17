/* LOGGER.cpp
 *   by Lut99
 *
 * Created:
 *   25/07/2021, 14:11:06
 * Last edited:
 *   17/08/2021, 16:13:33
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
/* Constructor for the Logger class, which takes an output stream to write its non-messages to, an output stream to write error messages to and a verbosity level. */
Logger::Logger(std::ostream& stdos, std::ostream& erros, Verbosity verbosity) :
    stdos(stdos),
    erros(erros),
    verbosity(verbosity)
{}



/* Defines a thread-channel mapping for the given thread. Allows log, warning, error and fatal to be called without channel parameter. Set to an empty string to remove the channel again. */
void Logger::set_thread_channel(const std::thread::id& tid, const std::string& channel) {
    // If the channel is empty, remove the map instead
    if (channel.empty()) {
        std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_channel_map.find(tid);
        if (iter != this->thread_channel_map.end()) {
            this->thread_channel_map.erase(iter);
        }
        return;
    }

    // Either create or update the value by calling operator[] in our map
    this->thread_channel_map[tid] = channel;

    // DOne
    return;
}
