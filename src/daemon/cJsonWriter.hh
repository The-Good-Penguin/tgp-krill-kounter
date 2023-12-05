// cJsonWriter.hh
#ifndef _CJSONWRITER_H
#define _CJSONWRITER_H

#include "../library/include/structs.hh"
#include <json-glib/json-glib.h>
#include <stdint.h>
#include <string>
#include <vector>

class cJsonWriter
{
    public:
        cJsonWriter();
        bool writeJson(std::string jsonPathInput, std::string jsonPathOutput,
            std::string deviceRef, std::string serialNumber,
            std::string previousPath, struct sBlockStats* pStats,
            uintmax_t totalBytesWritten);

    private:
        uint _indentLevel = 4;
        JsonBuilder* _pJsonBuilder;
        bool readExistingJson(std::string jsonPath,
            std::vector<struct jsonDeviceEntry>* pDevices);
        void addEntryToBuilder(std::string deviceRef, std::string serialNumber,
            std::string previousPath, struct sBlockStats* pStats,
            uintmax_t totalBytesWritten);
};

#endif /* _CJSONWRITER_H */