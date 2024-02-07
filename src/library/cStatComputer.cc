#include "cStatComputer.hh"
#include <limits.h>

uint cStatComputer::getAverageWriteSize(
    struct sBlockStats* pDeviceStats, uint sectorSize)
{
    return (sectorSize * pDeviceStats->writeSectors) / pDeviceStats->writeIo;
}

gint64 cStatComputer::totalBytesWritten(uint sectorSize,
    gint64 currentWriteSectors, gint64 previousWriteSectors,
    gint64 previousTotal)
{
    gint64 newTotal = previousTotal;
    if (currentWriteSectors < previousWriteSectors)
    {
        /* overflow occured, calculate difference:

        ULONG_MAX from limits.h is used to get the max value for the
        writeSector field. This can either be unsigned int (32 bit)
        or unsigned long (32-bit or 64-bit), depending on the machine.

        https://www.kernel.org/doc/html/v6.1/admin-guide/iostats.html
        */
        newTotal += (ULONG_MAX + 1 - previousWriteSectors) * sectorSize;
        newTotal += currentWriteSectors * sectorSize;
    }
    else
    {
        newTotal += (currentWriteSectors - previousWriteSectors) * sectorSize;
    }

    return newTotal;
}
