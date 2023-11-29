// structs.hh
#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <string>

struct sBlockStats
{
        int readIo;
        int readMerges;
        int readSectors;
        int readTicks;
        int writeIo;
        int writeMerges;
        int writeSectors;
        int writeTicks;
        int inFlight;
        int ioTicks;
        int timeInQueue;
        int discardIo;
        int discardMerges;
        int discardSectors;
        int discardTicks;
};

struct sDeviceSpecs
{
        std::string manfid;
        std::string oemid;
        std::string name;
        std::string hwrev;
        std::string fwrev;
        std::string serial;
        std::string mdt;
};

#endif /* _STRUCTS_H */