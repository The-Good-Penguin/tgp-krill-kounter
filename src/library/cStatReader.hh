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
        std::vector<std::string> findDevices(void);
        bool getSpaceInfo(std::string deviceName, uintmax_t* pValue);
        bool getStats(std::string deviceName, struct sBlockStats* pStats);
        bool getDiskSeq(std::string deviceName, gint64* pSeq);
        bool getSpecs(std::string deviceName, struct sDeviceSpecs* pSpecs);

    private:
        int _sectorSize = 512;
        bool getSpecsEmmc(std::string deviceName, struct sDeviceSpecs* pSpecs);
        bool getSerialNumberFallback(
            std::string deviceName, struct sDeviceSpecs* pSpecs);
};

#endif /* _CSTATREADER_H */