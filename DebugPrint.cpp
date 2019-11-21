// A debug log message that uses stream-style printing

#include "DebugPrint.h"

#include "asprintf.h"

#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

// Storage for the static instance
static DebugPrint s_thisIsTheOnlyDebugPrint;

// Static pointer to the static instance
DebugPrint* DebugPrint::s_onlyDebugPrint = nullptr;

int getDbgStrStrPtr()
{
    static int i = std::ios_base::xalloc();
    return i;
}

DebugPrint::DebugPrint()
{
    s_onlyDebugPrint = this;
}

std::ostream& DebugPrint::streamPrint(bool cond, int severity, const char* fname, const int lineno)
{
    m_dbgstrstr.clear();            // clear any bits set
    m_dbgstrstr.str(std::string()); // remove contents

    if (m_verbosity >= severity && cond)
        m_dbgstrstr.pword(getDbgStrStrPtr()) = &m_dbgstrstr; // Store a pointer to the stringstream inside its ostream base class for later access.
    else
        m_dbgstrstr.pword(getDbgStrStrPtr()) = nullptr;
    m_dbgstrstr << fname << '(' << lineno << "): "; // Should it instead store these values in iwords to be streamed at the end?

    return m_dbgstrstr;
}

// Grab the accumulated string and print it (also add EOL).
std::ostream& DebugPrint::streamEnd(std::ostream& os)
{
    os << std::endl;
    std::stringstream* pstrstr = reinterpret_cast<std::stringstream*>(os.pword(getDbgStrStrPtr()));

    if (pstrstr) {
        OutputDebugString(pstrstr->str().c_str());
        fprintf(stderr, "[TIMESTAMP]: %s", pstrstr->str().c_str());
        // Could also print to log file here.
    }

    return os;
}

void DebugPrint::print(bool cond, int severity, const char* fname, const int lineno, char const* const fmt, ...)
{
    va_list ap;
    
    if (!(m_verbosity >= severity && cond)) return;

    // Print the user's fmt and args into a C string allocated by vasprintf
    char* usrStr = nullptr;
    va_start(ap, fmt);
    int lenu = vasprintf(&usrStr, fmt, ap);
    va_end(ap);

    // Print the user's final string with our annotations into a C string allocated by asprintf
    char* annStr = nullptr;
    int lena = asprintf(&annStr, "%s(%d): %s", fname, lineno, usrStr);

    // Final printing of the log message to all destinations
    OutputDebugString(annStr);
    fprintf(stderr, "[TIMESTAMP]: %s", annStr);
    // Could also print annStr to log file here.

    if (lenu == 0) free(usrStr);
    if (lena == 0) free(annStr);
}
