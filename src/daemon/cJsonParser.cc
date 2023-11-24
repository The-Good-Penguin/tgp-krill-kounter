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

bool cJsonParser::getSerialNumber(std::string deviceRef, std::string* pValue)
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

    json_reader_read_member(pReader, deviceRef.c_str());
    json_reader_read_member(pReader, "serial_number");
    std::string output = (std::string)json_reader_get_string_value(pReader);

    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse 'serial_number': %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getTotalBytesWritten(std::string deviceRef, double* pValue)
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

    json_reader_read_member(pReader, deviceRef.c_str());
    json_reader_read_member(pReader, "total_bytes_written");
    double output = (double)json_reader_get_double_value(pReader);

    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to parse 'total_bytes_written': %s\n",
            pError->message);
        g_error_free(pError);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

bool cJsonParser::getStats(std::string deviceRef, struct sBlockStats* pStats)
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

    json_reader_read_member(pReader, deviceRef.c_str());
    json_reader_read_member(pReader, "previous_stats");

    int numErrors = 0;
    if (!getValueAsInt(pReader, "read_io", &pStats->readIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "read_merges", &pStats->readMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "read_sectors", &pStats->readSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "read_ticks", &pStats->readTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "write_io", &pStats->writeIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "write_merges", &pStats->writeMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "write_sectors", &pStats->writeSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "write_ticks", &pStats->writeTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "in_flight", &pStats->inFlight))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "io_ticks", &pStats->ioTicks))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "time_in_queue", &pStats->timeInQueue))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discard_io", &pStats->discardIo))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discard_merges", &pStats->discardMerges))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discard_sectors", &pStats->discardSectors))
    {
        numErrors++;
    }
    if (!getValueAsInt(pReader, "discard_ticks", &pStats->discardTicks))
    {
        numErrors++;
    }

    g_object_unref(pReader);
    return numErrors > 0 ? false : true;
}

bool cJsonParser::getPath(std::string deviceRef, std::string* pValue)
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

    json_reader_read_member(pReader, deviceRef.c_str());
    json_reader_read_member(pReader, "previous_path");
    std::string output = (std::string)json_reader_get_string_value(pReader);

    pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse 'previous_path': %s\n", pError->message);
        g_error_free(pError);
        return false; // failure
    }

    *pValue = output;

    g_object_unref(pReader);
    return true; // success
}

// private function

bool cJsonParser::getValueAsInt(
    JsonReader* pReader, std::string itemName, int* pValue)
{
    json_reader_read_member(pReader, itemName.c_str());
    int value = (int)json_reader_get_int_value(pReader);

    GError* pError = (GError*)json_reader_get_error(pReader);
    if (pError)
    {
        LOG_EVENT(
            LOG_ERR, "Unable to parse '%s': %s\n", itemName, pError->message);
        g_error_free(pError);
        return false; // failure
    }

    *pValue = value;

    json_reader_end_member(pReader);
    return true; // success
}
