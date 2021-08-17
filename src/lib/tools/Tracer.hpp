/* TRACER.hpp
 *   by Lut99
 *
 * Created:
 *   17/08/2021, 16:59:21
 * Last edited:
 *   17/08/2021, 16:59:21
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a debug tracer for debugging purposes. Is only enabled when
 *   NDEBUG and NTRACE are not defined. If TRACE_LOG is defined, then the
 *   class prints changes to the callstack to stdout.
**/

#ifndef TOOLS_TRACER_HPP
#define TOOLS_TRACER_HPP

#include <limits>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <exception>


/***** COMMON MACROS *****/
/* Call to stringify given macro. */
#define STR(S) STR_2(S)
/* Helper macro for STR. */
#define STR_2(S) #S





#if !(defined(NDEBUG) && defined(NTRACE))
/***** TRACE ENABLED MACROS *****/

/* Defines the current thread in the tracer by giving it a name. */
#define TSTART(NAME) \
    tracer.register_thread((NAME));
/* Undefines the current thread in the tracer by giving it a name. */
#define TSTOP \
    tracer.unregister_thread();

/* Puts the current function onto the callstack. Since it isn't called but rather entered, line information will be missing. */
#define TENTER(FUNC_NAME) \
    tracer.push_frame((FUNC_NAME), (__FILE__), std::numeric_limits<size_t>::max());
/* Removes the current function from the callstack again. */
#define TLEAVE \
    tracer.pop_frame();
/* Removes the current function from the callstack again and returns the function. Can be used as a normal return statement. */
#define TRETURN \
    tracer.pop_frame(); return

/* Calls the given function without a return value, putting it on the stack with its call location. */
#define TCALL(FUNC_NAME, FUNC_ARGS) \
    tracer.push_frame((#FUNC_NAME), (__FILE__), (__LINE__)); (FUNC_NAME)FUNC_ARGS; tracer.pop_frame();
/* Calls the given function with a return value, putting it on the stack with its call location. Cannot returns function values, though. */
#define TCALLR(VAR, FUNC_NAME, FUNC_ARGS) \
    tracer.push_frame((#FUNC_NAME), (__FILE__), (__LINE__)); (VAR) = (FUNC_NAME)FUNC_ARGS; tracer.pop_frame();





/***** IMPLEMENTATION *****/
namespace Tools {
    /* The Tracer class, which manages the callstack. */
    class Tracer {
    public:
        /* The initial size of each stack. */
        static constexpr const size_t initial_stack_size = 16;
        /* The width of the prefix message for errors. Includes extra space. */
        static constexpr const size_t prefix_width = 8;

    private:
        /* A single frame on the callstack. */
        struct Frame {
            /* The name of the function called. */
            const char* name;
            /* The filename of the call. */
            const char* file;
            /* The call location as a line number. */
            size_t line;
        };

    private:
        /* The callstack. We have one per thread. */
        std::unordered_map<std::thread::id, std::vector<Frame>> stack;
        /* Mapping from thread ids to names. */
        std::unordered_map<std::thread::id, const char*> thread_names;

        /* Mutex lock used for synchronization. */
        std::mutex lock;

        /* Red colour string. */
        const char* sred;
        /* Bold colour string. */
        const char* sbold;
        /* Clear colour string. */
        const char* sreset;
    
    public:
        /* Default constructor for the Tracer class. */
        Tracer();

        /* Registers the current thread in the tracer by giving it a name. */
        void register_thread(const char* thread_name);
        /* Unregisters the current thread. */
        void unregister_thread();

        /* Enter the function with the given name that is called from the given file at the given line number. Use std::numeric_limits<size_t>::max() to indicate it wasn't called. */
        void push_frame(const char* func_name, const char* file_name, size_t line);
        /* Pop the last function off the stack again. */
        void pop_frame();

        /* Prints the given text as fatal error to the given stream, printing all traces. */
        void throw_error(const char* message) const;

    };

    /* Tell the compiler that there is an global debugger instance. */
    extern Tracer tracer;
}





#else
/***** TRACE DISABLED MACROS *****/

/* Defines the current thread in the tracer by giving it a name. */
#define TSTART(NAME)
/* Undefines the current thread in the tracer by giving it a name. */
#define TSTOP

/* Puts the current function onto the callstack. Since it isn't called but rather entered, line information will be missing. */
#define TENTER(NAME)
/* Removes the current function from the callstack again. */
#define TLEAVE
/* Removes the current function from the callstack again and returns the function. Can be used as a normal return statement. */
#define TRETURN \
    return

/* Calls the given function without a return value, putting it on the stack with its call location. */
#define TCALL(FUNC_NAME, FUNC_ARGS) \
    (FUNC_NAME)FUNC_ARGS;
/* Calls the given function with a return value, putting it on the stack with its call location. Cannot returns function values, though. */
#define TCALLR(VAR, FUNC_NAME, FUNC_ARGS) \
    (VAR) = (FUNC_NAME)FUNC_ARGS;

#endif

#endif
