// structs.hh
#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <glib.h>
#include <string>

struct sBlockStatStub
{
        std::string value;
        bool enabled = false;
};

struct sBlockStats
{
        gint64 readIo;
        gint64 readMerges;
        gint64 readSectors;
        gint64 readTicks;
        gint64 writeIo;
        gint64 writeMerges;
        gint64 writeSectors;
        gint64 writeTicks;
        gint64 inFlight;
        gint64 ioTicks;
        gint64 timeInQueue;
        gint64 discardIo;
        gint64 discardMerges;
        gint64 discardSectors;
        gint64 discardTicks;

        inline bool operator == (struct sBlockStats a) const {
            return a.readIo == readIo && a.readMerges == readMerges &&
                a.readSectors == readSectors && a.readTicks == readTicks &&
                a.writeIo == writeIo && a.writeMerges == writeMerges &&
                a.writeSectors == writeSectors && a.writeTicks == writeTicks &&
                a.inFlight == inFlight && a.ioTicks == ioTicks &&
                a.timeInQueue == timeInQueue && a.discardIo == discardIo &&
                a.discardMerges == discardMerges && a.discardSectors == discardSectors &&
                a.discardTicks == discardTicks;
        }
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
        struct sBlockStats outputStats;
        gint64 totalBytesWritten;
        gint64 diskSeq;
};


struct jsonDeviceConfig
{
        std::string deviceName;
        std::string devicePath;
        gint64 updateRate;
        std::string statsFilePath;
};
#endif /* _STRUCTS_H */
