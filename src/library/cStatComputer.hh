// cStatComputer.hh
#ifndef _CSTATCOMPUTER_H
#define _CSTATCOMPUTER_H

#include "include/structs.hh"

#include <cstdint>
#include <string>

class cStatComputer
{
    public:
        uint getAverageWriteSize(
            struct sBlockStats* pDeviceStats, uint sectorSize);
        float totalBytesWritten(uint sectorSize, uintmax_t currentWriteSectors,
            uintmax_t previousWriteSectors, float previousTotal);
};

#endif /* _CSTATCOMPUTER_H */
