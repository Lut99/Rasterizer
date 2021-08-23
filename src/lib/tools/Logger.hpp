/* LOGGER.hpp
 *   by Lut99
 *
 * Created:
 *   25/07/2021, 14:11:20
 * Last edited:
 *   23/08/2021, 14:52:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Logger class, which logs different things based on the
 *   severity level.
**/

#ifndef TOOLS_LOGGER_HPP
#define TOOLS_LOGGER_HPP

#include <ostream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <exception>
#include "date.h"

namespace Tools {
    /* Namespace encapsulating the verbosity enum. */
    namespace VerbosityValues {
        /* The values of the Verbosity enum. */
        enum values {
            /* No verbosity; ignore all messages except for the most fatal of errors. */
            none = 0,
            /* A little verbosity: shows some major plotpoints, such as tokenizing and parsing, but leave most out. */
            important = 1,
            /* Some more verbosity: shows most of the steps the parser goes through, but leaves out some more debug information such as parameter values. */
            details = 2,
            /* Shows everything, including debug information of parameters and junk. */
            debug = 3
        };
    }
    /* The Verbosity enum, which is directly mapped to verbosity integers. */
    using Verbosity = VerbosityValues::values;



    /* The Logger class, which is used to dynamically log stuff. */
    class Logger {
    public:
        /* Static string that is used in case an empty sting is needed. */
        inline static const std::string empty_string = "";

        /* Data that can be used to initialize the Logger instead of calling it with separate arguments. */
        struct InitData {
            /* The output stream to which we write. */
            std::ostream& stdos;
            /* The error stream to which we write. */
            std::ostream& erros;
            /* The verbosity level of the Logger. */
            Verbosity verbosity;
        };

        /* Exception that is thrown on fatal() calls. */
        class Fatal: public std::exception {
        public:
            /* The message passed to the fatal error message. */
            std::string message;

            /* Constructor for the Fatal exception, which takes a message describing what happened. */
            Fatal(const std::string& message);

            /* Overload of the what() function, which is used to pretty-print the message. */
            inline const char* what() const noexcept { return this->message.c_str(); }
        };


        /* The output stream to log non-error messages to. */
        std::ostream& stdos;
        /* The output stream to log error messages to. */
        std::ostream& erros;

    private:
        /* The verbosity level of logging. */
        Verbosity verbosity;
        /* Keeps track of the thread/channel maps made. */
        std::string channel;


        /* Internal helper function that populates a given stringstream with all given types, converted to strings. */
        template <class T>
        static inline void _add_args(std::ostream& os, const T& arg) { os << arg; }
        /* Internal helper function that populates a given stringstream with all given types, converted to strings. */
        template <class T, class... Ts>
        static void _add_args(std::ostream& os, const T& arg, const Ts&... rest) {
            os << arg;
            Logger::_add_args(os, rest...);
        }
  
    public:
        /* Constructor for the Logger class, which takes an output stream to write its non-messages to, an output stream to write error messages to and a verbosity level. Optionally takes a default channel name. */
        Logger(std::ostream& stdos, std::ostream& erros, Verbosity verbosity, const std::string& channel = "");
        /* Constructor for the Logger class, which takes its setup data as an InitData struct. Optionally takes a default channel name. */
        Logger(const InitData& init_data, const std::string& channel = "");
        /* Copy constructor for the Logger class. */
        Logger(const Logger& other);
        /* Move constructor for the Logger class. */
        Logger(Logger&& other);
        /* Destructor for the Logger class. */
        ~Logger();

        /* Defines a channel to standardly use for this thread, allowing log, warning, error and fatal to be called without channel parameter and still display a channel. Set to an empty string to remove the channel again. */
        inline void set_channel(const std::string& channel) { this->channel = channel; }
        /* Returns the channel set for this logger. If no such channel name is defined, returns an empty string. */
        inline const std::string& get_channel() const { return this->channel; }

        /* Writes a message to the internal standard output stream. The given verbosity determines if the message is printed or not. The arguments are appended (in order) and without spaces in between. */
        template <class... Ts>
        inline void log(Verbosity verbosity, const Ts&... message) const { this->logc(verbosity, this->get_channel(), message...); }
        /* Writes a message to the internal standard output stream. The given verbosity determines if the message is printed or not, and the channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. */
        template<class... Ts>
        void logc(Verbosity verbosity, const std::string& channel, Ts... args) const {
            using namespace date;

            // Check if we should print
            if (this->verbosity < verbosity) { return; }

            // Otherwise, start constructing the stringstream
            this->stdos << '[' << std::chrono::system_clock::now() << ']';
            this->stdos << "[INFO]";
            if (!channel.empty()) { this->stdos << '[' << channel << ']'; }
            this->stdos << ' ';
            this->_add_args(this->stdos, args...);
            this->stdos << std::endl;

            // Done
            return;
        }
        /* Writes a warning message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's a warning, its verbosity is fixed to 2 (details). */
        template <class... Ts>
        inline void warning(const Ts&... message) const { this->warningc(this->get_channel(), message...); }
        /* Writes a warning message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's a warning, its verbosity is fixed to 2 (details). */
        template<class... Ts>
        void warningc(const std::string& channel, Ts... args) const {
            using namespace date;

            // Check if we should print
            if (this->verbosity < Verbosity::details) { return; }

            // Otherwise, start constructing the stringstream
            this->erros << '[' << std::chrono::system_clock::now() << ']';
            this->erros << "[WARNING]";
            if (!channel.empty()) { this->erros << '[' << channel << ']'; }
            this->erros << ' ';
            this->_add_args(this->erros, args...);
            this->erros << std::endl;

            // Done
            return;
        }
        /* Writes an error message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's an, its verbosity is fixed to 1 (important). */
        template <class... Ts>
        inline void error(const Ts&... message) const { this->errorc(this->get_channel(), message...); }
        /* Writes an error message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's an, its verbosity is fixed to 1 (important). */
        template<class... Ts>
        void errorc(const std::string& channel, Ts... args) const {
            using namespace date;

            // Check if we should print
            if (this->verbosity < Verbosity::important) { return; }

            // Otherwise, start constructing the stringstream
            this->erros << '[' << std::chrono::system_clock::now() << ']';
            this->erros << "[ERROR]";
            if (!channel.empty()) { this->erros << '[' << channel << ']'; }
            this->erros << ' ';
            this->_add_args(this->erros, args...);
            this->erros << std::endl;

            // Done
            return;
        }
        /* Writes an error message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's an, its verbosity is fixed to 1 (important). */
        template <class... Ts>
        [[ noreturn ]] inline void fatal(const Ts&... message) const { this->fatalc(this->get_channel(), message...); }
        /* Writes an error message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's an, its verbosity is fixed to 1 (important). */
        template<class... Ts>
        [[ noreturn ]] void fatalc(const std::string& channel, Ts... args) const {
            using namespace date;

            // We first construct the message separately
            std::stringstream sstr;
            this->_add_args((std::ostream&) sstr, args...);

            // Always start constructing the stringstream
            this->erros << '[' << std::chrono::system_clock::now() << ']';
            this->erros << "[FATAL]";
            if (!channel.empty()) { this->erros << '[' << channel << ']'; }
            this->erros << ' ' << sstr.str() << std::endl;

            // Instead of returning, hit 'em with the exception
            throw Logger::Fatal(sstr.str());
        }

        /* Sets the internal verbosity to the given value. */
        inline void set_verbosity(Verbosity new_value) { this->verbosity = new_value; }
        /* Returns the internal verbosity. */
        inline Verbosity get_verbosity() const { return this->verbosity; }

        /* Explicitly returns an InitData struct so another Logger may be created with the same settings. */
        inline InitData get_init_data() const { return InitData({ this->stdos, this->erros, this->verbosity }); }
        /* Implicitly returns an InitData struct so another Logger may be created with the same settings. */
        inline operator InitData() const { return this->get_init_data(); }

        /* Copy assignment operator for the Logger class. */
        inline Logger& operator=(const Logger& other) { return *this = Logger(other); }
        /* Move assignment operator for the Logger class. */
        inline Logger& operator=(Logger&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Logger class. */
        friend void swap(Logger& l1, Logger& l2);

    };
    
    /* Swap operator for the Logger class. */
    void swap(Logger& l1, Logger& l2);

}

#endif