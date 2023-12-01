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
        bool getSerialNumber(std::string deviceRef, std::string* pValue);
        bool getTotalBytesWritten(std::string deviceRef, double* pValue);
        bool getStats(std::string deviceRef, struct sBlockStats* pStats);
        bool getPath(std::string deviceRef, std::string* pValue);
        bool getDeviceRefs(std::vector<std::string>* pValue);

    private:
        bool getValueAsInt(
            JsonReader* pReader, std::string itemName, int* pValue);
        JsonParser* _pJsonParser;
        int _parserOpen = false;
};

#endif /* _CJSONPARSER_H */
