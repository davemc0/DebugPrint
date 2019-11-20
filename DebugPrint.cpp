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

// Print any message with severity <= g_verbosity (smaller numbers are more important).
const int g_verbosity = 30;

#ifdef _DEBUG
#define DBGBOOL true
#else
#define DBGBOOL false
#endif

int getDbgStrStrPtr()
{
    static int i = std::ios_base::xalloc();
    return i;
}

// Grab the accumulated string and print it (also add EOL).
std::ostream& dbgStreamEnd(std::ostream& os)
{
    os << std::endl;
    std::stringstream* pstrstr = reinterpret_cast<std::stringstream*>(os.pword(getDbgStrStrPtr()));
    if (pstrstr) {
        fprintf(stderr, "[TIMESTAMP]: %s", pstrstr->str().c_str());
        OutputDebugString(pstrstr->str().c_str());
    }

    return os;
}

#define DBGS(severity_) debugStrPrintCond(g_verbosity >= severity_ && DBGBOOL, __FILE__, __LINE__)
#define DEND dbgStreamEnd

std::stringstream s_dbgstrstr;

std::ostream& debugStrPrintCond(bool cond, const char* fname, const int lineno)
{
    s_dbgstrstr.clear();            // clear any bits set
    s_dbgstrstr.str(std::string()); // remove contents

    if (cond)
        s_dbgstrstr.pword(getDbgStrStrPtr()) = &s_dbgstrstr; // Store a pointer to the stringstream inside its ostream baseclass for later access.
    else
        s_dbgstrstr.pword(getDbgStrStrPtr()) = nullptr;
    s_dbgstrstr << fname << '(' << lineno << "): "; // Should it instead store these values in iwords to be streamed at the end?

    return s_dbgstrstr;
}

struct hardToPrint_t
{
    int x;
    hardToPrint_t(int x_) : x(x_) {}
};

std::ostream& operator<<(std::ostream& stream, const hardToPrint_t& rhs)
{
    stream << "[" << rhs.x << "]";
    return stream;
}

void tryDebugPrint()
{
    for (int i = 0; i < 10; i++) {
        DBGS(10) << "Warning: " << hardToPrint_t(1) << " I just thought you ought to know." << DEND;
        DBGS(50) << "FYI: " << hardToPrint_t(2) << DEND;
    }
}

// An error checker macro for OpenCL calls
#define CLERRCK(op)                                                         \
    {                                                                       \
        int res = op;                                                       \
        if (op != 0) { printf("error: '%s'\n", std::string(#op).c_str()); } \
    }
