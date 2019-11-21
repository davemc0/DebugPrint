// A debug log message that uses stream-style printing

#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

// Considerations:
// Compiler seeing all the code
// Not executing all the code if not printing (more performant) vs. executing it (better for side-effects)
// A hidden if() has hidden syntax pitfalls
// A static stream may not be thread-safe and is inelegant
// Having to put a terminator would be annoying
// Need to package the user-provided output into one string for C-style printing by the bigger system

#ifdef _DEBUG
#define DBGBOOL true
#else
#define DBGBOOL false
#endif

//////////////////////////////////////////////////////////////////////////////////////
// Interface

// Use like DBGS(10) << "Warning: " << x << " is bad." << DEND;
#define DBGS(severity_) DebugPrint::getSingleton()->streamPrint(DBGBOOL, severity_, __FILE__, __LINE__)
#define DEND DebugPrint::streamEnd

// Use like DPRINT(50, "Warning: %d is bad.\n");
#define DPRINT(severity_, fmt, ...) DebugPrint::getSingleton()->print(DBGBOOL, severity_, __FILE__, __LINE__, fmt, __VA_ARGS__);

#define DBGSETVERBOSITY(v) DebugPrint::getSingleton()->setVerbosity(v);

// An error checker macro for OpenCL calls
#define CLERRCK(op)                                                         \
    {                                                                       \
        int res = op;                                                       \
        if (op != 0) { printf("error: '%s'\n", std::string(#op).c_str()); } \
    }

//////////////////////////////////////////////////////////////////////////////////////
// Implementation

class DebugPrint
{
public:
    DebugPrint();
    DebugPrint(const DebugPrint&) = delete;
    DebugPrint& operator=(const DebugPrint) = delete;

    // Return a stream that debug info can be streamed to.
    // Streams the fname and line number into that stream.
    // Condition must be true.
    // After the last item to add to the stream, you must stream streamEnd
    std::ostream& streamPrint(bool cond, int severity, const char* fname, const int lineno);

    // Grabs the accumulated string and print it (also adds EOL).
    static std::ostream& streamEnd(std::ostream& os);

    void print(bool cond, int severity, const char* fname, const int lineno, char const* const fmt, ...);

    static DebugPrint* getSingleton() { return s_onlyDebugPrint; }

    void setVerbosity(const int v) { m_verbosity = v; }
    int getVerbosity() const { return m_verbosity; }

private:
    // Print any message with severity <= m_verbosity (smaller numbers are more important).
    int m_verbosity = 30;

    std::stringstream m_dbgstrstr;

    // A pointer to the singleton instance of the DebugPrint class
    static DebugPrint* s_onlyDebugPrint;
};
