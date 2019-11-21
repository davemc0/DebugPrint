#include "DebugPrint.h"

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

int main(int argc, char** argv)
{
    // Do a few debug prints
    for (int i = 0; i < 10; i++) {
        DBGS(10) << "Warning: There are " << hardToPrint_t(i) << " trolls in the dungeon." << DEND;
        DBGS(50) << "FYI: " << hardToPrint_t(2*2) << DEND;
        DPRINT(30, "0x%llx is a hex number.\n", ~(unsigned long long)0x5a5a);
    }

    DBGSETVERBOSITY(70);

    for (int i = 0; i < 10; i++) {
        DBGS(50) << "FYI: The number of thy counting shall be " << i << '.' << DEND;
    }
}
