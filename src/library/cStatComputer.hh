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
        gint64 totalBytesWritten(uint sectorSize, gint64 currentWriteSectors,
            gint64 previousWriteSectors, gint64 previousTotal);
};

#endif /* _CSTATCOMPUTER_H */
