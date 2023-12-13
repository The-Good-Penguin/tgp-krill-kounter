// structs.hh
#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <string>

struct sBlockStatStub
{
        std::string value;
        bool enabled = false;
};

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
        struct sBlockStatStub manfid;
        struct sBlockStatStub oemid;
        struct sBlockStatStub name;
        struct sBlockStatStub hwrev;
        struct sBlockStatStub fwrev;
        struct sBlockStatStub serial;
        struct sBlockStatStub mdt;
};

struct jsonDeviceEntry
{
        std::string serialNumber;
        std::string previousPath;
        struct sBlockStats stats;
        double totalBytesWritten;
};

#endif /* _STRUCTS_H */
