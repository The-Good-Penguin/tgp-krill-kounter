// cStatReader.hh
#ifndef _CSTATREADER_H
#define _CSTATREADER_H

#include "include/structs.hh"
#include <cstdint>
#include <string>
#include <vector>

class cStatReader
{
    public:
        // General
        std::vector<std::string> findDevices(void);

        // Storage Specs
        uintmax_t getSpaceInfo(std::string deviceName);
        void getStats(std::string deviceName, struct sBlockStats* pStats);
        void getSpecs(std::string devicePath, struct sDeviceSpecs* pSpecs);

    private:
        int _sectorSize = 512;
};

#endif /* _CSTATREADER_H */