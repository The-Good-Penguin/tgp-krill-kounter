#include "cJsonWriter.hh"

#include "../utils/log-event.hh"
#include "cJsonParser.hh"
#include <fstream>

// constructor

cJsonWriter::cJsonWriter() { _pJsonBuilder = json_builder_new(); }

// public functions

bool cJsonWriter::writeJson(std::string jsonPathInput,
    std::string jsonPathOutput, std::string serialNumber,
    std::string firstSightingDate, std::string previousPath,
    struct sBlockStats* pStats, gint64 diskSeq, gint64 totalBytesWritten)
{
    json_builder_begin_object(_pJsonBuilder);

    // does file already exist
    std::ifstream f(jsonPathInput.c_str());
    if (f.good())
    {
        // load existing data
        std::vector<struct sJsonDeviceEntry> devices;
        if (!readExistingJson(jsonPathInput, &devices))
        {
            LOG_EVENT(LOG_ERR, "Unable to read existing json file: %s\n",
                jsonPathInput.c_str());
            return false; // failure
        }

        // build old json data
        for (uint i = 0; i < devices.size(); i++)
        {
            addEntryToBuilder(devices[i].serialNumber,
                devices[i].firstSightingDate, devices[i].previousPath,
                &(devices[i].stats), devices[i].diskSeq, devices[i].totalBytesWritten);
        }

        f.close();
    }

    /*
    build new json data
    -----
    If an entry with a matching serialNumber has been loaded into the builder
    via addEntryToBuilder() above, then the values for that entry will be
    overwritten here before any json is generated or written back to disk.
    */
    addEntryToBuilder(serialNumber, firstSightingDate, previousPath, pStats,
        diskSeq, totalBytesWritten);

    json_builder_end_object(_pJsonBuilder);

    // generate json string
    JsonGenerator* pGen = json_generator_new();
    json_generator_set_pretty(pGen, true);
    json_generator_set_indent(pGen, _indentLevel);
    JsonNode* pRoot = json_builder_get_root(_pJsonBuilder);
    if (pRoot == nullptr)
    {
        LOG_EVENT(LOG_ERR, "Unable to get root of _pJsonBuilder");
        return false; // failure
    }
    json_generator_set_root(pGen, pRoot);

    // write json string to file
    GError* pError = nullptr;
    json_generator_to_file(pGen, jsonPathOutput.c_str(), &pError);
    if (pError)
    {
        LOG_EVENT(LOG_ERR, "Unable to write json to file [%s]: %s\n",
            jsonPathOutput.c_str(), pError->message);
        g_error_free(pError);
        g_object_unref(pGen);
        json_builder_reset(_pJsonBuilder);
        return false; // failure
    }

    // tidy up
    g_object_unref(pGen);
    json_builder_reset(_pJsonBuilder);

    return true; // success
}

// private functions

bool cJsonWriter::readExistingJson(
    std::string jsonPath, std::vector<struct sJsonDeviceEntry>* pDevices)
{
    // open file
    cJsonParser parser;
    if (!parser.openJson(jsonPath))
    {
        LOG_EVENT(LOG_ERR, "Unable to open json file: %s\n", jsonPath.c_str());
        return false; // failure
    }

    // get all device references
    std::vector<std::string> serialNumbers;
    if (!parser.getSerialNumbers(&serialNumbers))
    {
        LOG_EVENT(LOG_ERR, "Unable to parse device references from file");
        return false; // failure
    }

    // build sJsonDeviceEntry for each device in json file, add to pDevices
    for (uint i = 0; i < serialNumbers.size(); i++)
    {
        struct sJsonDeviceEntry device;
        bool error = false;

        device.serialNumber = serialNumbers[i];
        error |= !parser.getFirstSightingDate(device.serialNumber, &device.firstSightingDate);
        error |= !parser.getPath(device.serialNumber, &device.previousPath);
        error |= !parser.getStats(device.serialNumber, &device.stats);
        error |= !parser.getDiskSeq(device.serialNumber, &device.diskSeq);
        error |= !parser.getTotalBytesWritten(
            device.serialNumber, &device.totalBytesWritten);

        if (error)
        {
            LOG_EVENT(LOG_ERR, "Unable to parse serial number: %s\n",
                serialNumbers[i].c_str());
            break;
        }
        pDevices->push_back(device);
    }

    parser.closeJson();

    return true; // success
}

void cJsonWriter::addEntryToBuilder(std::string serialNumber,
    std::string firstSightingDate, std::string previousPath,
    struct sBlockStats* pStats, gint64 diskSeq, gint64 totalBytesWritten)
{
    // serial number
    json_builder_set_member_name(_pJsonBuilder, serialNumber.c_str());
    json_builder_begin_object(_pJsonBuilder);
    // - first sighting date
    json_builder_set_member_name(_pJsonBuilder, "firstSightingDate");
    json_builder_add_string_value(_pJsonBuilder, firstSightingDate.c_str());
    // - previous path
    json_builder_set_member_name(_pJsonBuilder, "previousPath");
    json_builder_add_string_value(_pJsonBuilder, previousPath.c_str());
    // - begin previous stats
    json_builder_set_member_name(_pJsonBuilder, "previousStats");
    json_builder_begin_object(_pJsonBuilder);
    // -- readIo
    json_builder_set_member_name(_pJsonBuilder, "readIo");
    json_builder_add_int_value(_pJsonBuilder, pStats->readIo);
    // -- readMerges
    json_builder_set_member_name(_pJsonBuilder, "readMerges");
    json_builder_add_int_value(_pJsonBuilder, pStats->readMerges);
    // -- readSectors
    json_builder_set_member_name(_pJsonBuilder, "readSectors");
    json_builder_add_int_value(_pJsonBuilder, pStats->readSectors);
    // -- readTicks
    json_builder_set_member_name(_pJsonBuilder, "readTicks");
    json_builder_add_int_value(_pJsonBuilder, pStats->readTicks);
    // -- writeIo
    json_builder_set_member_name(_pJsonBuilder, "writeIo");
    json_builder_add_int_value(_pJsonBuilder, pStats->writeIo);
    // -- writeMerges
    json_builder_set_member_name(_pJsonBuilder, "writeMerges");
    json_builder_add_int_value(_pJsonBuilder, pStats->writeMerges);
    // -- writeSectors
    json_builder_set_member_name(_pJsonBuilder, "writeSectors");
    json_builder_add_int_value(_pJsonBuilder, pStats->writeSectors);
    // -- writeTicks
    json_builder_set_member_name(_pJsonBuilder, "writeTicks");
    json_builder_add_int_value(_pJsonBuilder, pStats->writeTicks);
    // -- inFlight
    json_builder_set_member_name(_pJsonBuilder, "inFlight");
    json_builder_add_int_value(_pJsonBuilder, pStats->inFlight);
    // -- ioTicks
    json_builder_set_member_name(_pJsonBuilder, "ioTicks");
    json_builder_add_int_value(_pJsonBuilder, pStats->ioTicks);
    // -- timeInQueue
    json_builder_set_member_name(_pJsonBuilder, "timeInQueue");
    json_builder_add_int_value(_pJsonBuilder, pStats->timeInQueue);
    // -- discardIo
    json_builder_set_member_name(_pJsonBuilder, "discardIo");
    json_builder_add_int_value(_pJsonBuilder, pStats->discardIo);
    // -- discardMerges
    json_builder_set_member_name(_pJsonBuilder, "discardMerges");
    json_builder_add_int_value(_pJsonBuilder, pStats->discardMerges);
    // -- discardSectors
    json_builder_set_member_name(_pJsonBuilder, "discardSectors");
    json_builder_add_int_value(_pJsonBuilder, pStats->discardSectors);
    // -- discardTicks
    json_builder_set_member_name(_pJsonBuilder, "discardTicks");
    json_builder_add_int_value(_pJsonBuilder, pStats->discardTicks);
    // - totalBytesWritten
    json_builder_end_object(_pJsonBuilder);
    json_builder_set_member_name(_pJsonBuilder, "diskSeq");
    json_builder_add_int_value(_pJsonBuilder, diskSeq);
    json_builder_set_member_name(_pJsonBuilder, "totalBytesWritten");
    json_builder_add_int_value(_pJsonBuilder, totalBytesWritten);
    // close
    json_builder_end_object(_pJsonBuilder);
}
