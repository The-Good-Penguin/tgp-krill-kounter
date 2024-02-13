// cJsonParser.hh
#ifndef _CJSONPARSER_H
#define _CJSONPARSER_H

#include "../library/include/structs.hh"
#include <json-glib/json-glib.h>
#include <string>
#include <vector>

class cJsonParser
{
    public:
        bool openJson(std::string jsonPath);
        bool closeJson();
        bool getConfig(jsonDeviceConfig* pConfig);
        bool getTotalBytesWritten(std::string serialNumber, gint64* pValue);
        bool getDiskSeq(std::string serialNumber, gint64* pValue);
        bool getStats(std::string serialNumber, struct sBlockStats* pStats);
        bool getPath(std::string serialNumber, std::string* pValue);
        bool getSerialNumbers(std::vector<std::string>* pValue);

    private:
        bool getValueAsInt(
            JsonReader* pReader, std::string itemName, gint64 * pValue);
        bool getValueAsString(
            JsonReader* pReader, std::string itemName, std::string* pValue);
        JsonParser* _pJsonParser;
        int _parserOpen = false;
};

#endif /* _CJSONPARSER_H */
