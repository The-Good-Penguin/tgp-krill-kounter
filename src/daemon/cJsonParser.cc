#include "cJsonParser.hh"
#include "../utils/log-event.hh"

#include <iostream>

// public functions

bool cJsonParser::openJson(std::string jsonPath)
{
    if (_parserOpen)
    {
        LOG_EVENT(LOG_ERR, "JSON already open");
        return false; // failure
    };

    GError* pError = nullptr;
    _pJsonParser   = json_parser_new();

    json_parser_load_from_file(_pJsonParser, jsonPath.c_str(), &pError);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        g_object_unref(_pJsonParser);
        return false; // failure
    }

    _parserOpen = true;
    return true; // success
}

bool cJsonParser::closeJson()
{
    if (!_parserOpen)
    {
        LOG_EVENT(LOG_ERR, "No JSON open");
        return false; // failure
    }

    g_object_unref(_pJsonParser);

    _parserOpen = false;
    return true; // success
}

#if JSON_CHECK_VERSION(1, 8, 0)
bool cJsonParser::getDevicesArray(JsonReader* pReader, struct sJsonDevicesConfig* pConfig)
{
    auto node = json_reader_get_current_node(pReader);
    if (json_node_get_node_type(node) != JSON_NODE_ARRAY)
    {
        LOG_EVENT(
            LOG_ERR, "Error parsing 'devices': devices is not an array\n");
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    auto devicePathArray = json_node_get_array(node);
    if (devicePathArray == nullptr)
    {
        LOG_EVENT(
            LOG_ERR, "Error parsing 'devices': error getting array\n");
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_array_foreach_element(
        devicePathArray,
        [](JsonArray* array, guint index_, JsonNode* element_node,
            gpointer user_data)
        {
            auto config     = (struct sJsonDevicesConfig*)user_data;
            auto devicePath = json_node_get_string(element_node);
            if (devicePath != nullptr)
                config->devices.emplace_back((std::string)devicePath);
        },
        pConfig);
    return true; // success
}
#else
bool cJsonParser::getDevicesArray(JsonReader* pReader, struct sJsonDevicesConfig* pConfig)
{
    GError* pError      = nullptr;
    auto elementCount = json_reader_count_elements(pReader);
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'devices': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    for (int i = 0; i < elementCount; ++i)
    {
        json_reader_read_element(pReader, i);
        pError = (GError*)json_reader_get_error(pReader);
        if (pError)
        {
            LOG_EVENT(LOG_ERR, "Error parsing 'devices': %s\n",
                pError->message);
            json_reader_end_member(pReader);
            g_object_unref(pReader);
            return false; // failure
        }

        auto devicePath = json_reader_get_string_value(pReader);
        if (devicePath != nullptr)
            pConfig->devices.emplace_back((std::string)devicePath);
        json_reader_end_element(pReader);
    }
    return true; // success
}
#endif

bool cJsonParser::getConfig(struct sJsonDevicesConfig* pConfig)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, "devices");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'devices': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    if(!getDevicesArray(pReader, pConfig))
    {
        return false; // failure
    }

    json_reader_end_member(pReader);

    // Optional members
    getValueAsInt(pReader, "updateRate", &pConfig->updateRate);
    getValueAsString(pReader, "statsFilePath", &pConfig->statsFilePath);

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getTotalBytesWritten(std::string serialNumber, gint64 *pValue)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, serialNumber.c_str());
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'serialNumber': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_reader_read_member(pReader, "totalBytesWritten");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'totalBytesWritten': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    auto output = json_reader_get_int_value(pReader);
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse 'totalBytesWritten': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getDiskSeq(std::string serialNumber, gint64 *pValue)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, serialNumber.c_str());
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'serialNumber': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_reader_read_member(pReader, "diskSeq");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'diskSeq': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    auto output = json_reader_get_int_value(pReader);
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'diskSeq': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getStats(std::string serialNumber, struct sBlockStats* pStats)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, serialNumber.c_str());
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'serialNumber': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_reader_read_member(pReader, "previousStats");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'previousStats': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    int numErrors = 0;
    if (!getValueAsInt(pReader, "readIo", &pStats->readIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "readMerges", &pStats->readMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "readSectors", &pStats->readSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "readTicks", &pStats->readTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "writeIo", &pStats->writeIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "writeMerges", &pStats->writeMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "writeSectors", &pStats->writeSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "writeTicks", &pStats->writeTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "inFlight", &pStats->inFlight))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "ioTicks", &pStats->ioTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "timeInQueue", &pStats->timeInQueue))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discardIo", &pStats->discardIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discardMerges", &pStats->discardMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discardSectors", &pStats->discardSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discardTicks", &pStats->discardTicks))
    {
        numErrors++;
    }

    g_object_unref(pReader);
    return numErrors > 0 ? false : true;
}

bool cJsonParser::getPath(std::string serialNumber, std::string* pValue)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, serialNumber.c_str());
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'serialNumber': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_reader_read_member(pReader, "previousPath");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'previousPath': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    std::string output = (std::string)json_reader_get_string_value(pReader);
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse 'previous_path': %s\n", pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getFirstSightingDate(std::string serialNumber, std::string* pValue)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    json_reader_read_member(pReader, serialNumber.c_str());
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'serialNumber': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    json_reader_read_member(pReader, "firstSightingDate");
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Error parsing 'firstSightingDate': %s\n",
            pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    std::string output = (std::string)json_reader_get_string_value(pReader);
    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse 'firstSightingDate': %s\n", pError->message);
        json_reader_end_member(pReader);
        g_object_unref(pReader);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getSerialNumbers(std::vector<std::string>* pValue)
{
    GError* pError      = nullptr;
    JsonReader* pReader = json_reader_new(json_parser_get_root(_pJsonParser));
    pError              = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse file: %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    for (uint i = 0; i < (uint)json_reader_count_members(pReader); i++)
    {
        pValue->push_back((std::string)json_reader_list_members(pReader)[i]);
    }

    g_object_unref(pReader);
    return true; // success
}

// private function

bool cJsonParser::getValueAsInt(
    JsonReader* pReader, std::string itemName, gint64* pValue)
{
    json_reader_read_member(pReader, itemName.c_str());
    int value = (int)json_reader_get_int_value(pReader);

    GError* pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse '%s': %s\n", itemName.c_str(), pError->message);
        json_reader_end_member(pReader);
        return false; // failure
    }

    *pValue = value;

    json_reader_end_member(pReader);
    return true; // success
}

bool cJsonParser::getValueAsString(
    JsonReader* pReader, std::string itemName, std::string *pValue)
{
    json_reader_read_member(pReader, itemName.c_str());
    auto value = json_reader_get_string_value(pReader);

    auto pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse '%s': %s\n", itemName.c_str(), pError->message);
        json_reader_end_member(pReader);
        return false; // failure
    }

    *pValue = (std::string) value;

    json_reader_end_member(pReader);
    return true; // success
}