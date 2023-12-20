#include <algorithm>
#include <filesystem>
#include <glib.h>
#include <iostream>
#include <json-glib/json-glib.h>
#include <string>

#include "daemon/cJsonParser.hh"
#include "daemon/cJsonWriter.hh"

#include "library/cStatComputer.hh"
#include "library/cStatReader.hh"
#include "library/include/structs.hh"

#include "utils/log-event.hh"

// global variables
cJsonParser parser;
cJsonWriter writer;
cStatReader reader;
cStatComputer computer;
struct jsonDeviceEntry targetDevice;

// glib variables
GError* pError           = nullptr;
GOptionContext* pContext = nullptr;
GMainLoop* pLoop         = nullptr;
guint timeoutId          = 0;

// cli values
std::string statsFilePath = "~/.KrillKounter/stats.json";
std::string deviceName;
std::string devicePath;
uint updateRate        = 3600; // seconds
uint sectorSize        = 512;
bool printBlockDevices = false;

// cli arguements
GOptionEntry options[] = { { "stats-file", 's', 0, G_OPTION_ARG_STRING,
                               &statsFilePath, "JSON stats file path" },
    { "device-path", 'd', 0, G_OPTION_ARG_STRING, &devicePath,
        "path of block device" },
    { "device-name", 'n', 0, G_OPTION_ARG_STRING, &deviceName,
        "name of block device" },
    { "update-rate", 'r', 0, G_OPTION_ARG_INT, &updateRate,
        "update rate of checks (seconds)" },
    { "sector-size", 'b', 0, G_OPTION_ARG_INT, &sectorSize,
        "sector size of block device" },
    { "print-devices", 'p', 0, G_OPTION_ARG_NONE, &printBlockDevices,
        "print all available block devices" },
    { NULL } };

void printAllBlockDevices(void)
{
    std::vector<std::string> devices = reader.findDevices();
    std::cout << "Block devices:\n";
    for (uint i = 0; i < devices.size(); i++)
    {
        std::cout << "- " << devices[i] << "\n";
    }
    exit(EXIT_SUCCESS);
}

void getSerialNumber(void)
{
    struct sDeviceSpecs specs;
    if (!reader.getSpecs(deviceName, &specs))
    {
        LOG_EVENT(LOG_ERR, "Unable to get device serial number\n");
        exit(EXIT_FAILURE);
    }
    targetDevice.serialNumber = specs.serial.value;
}

void parseFile(void)
{
    // Check if file exists
    const std::filesystem::path statsFile = (statsFilePath);
    if (std::filesystem::exists(statsFile))
    {
        if (!parser.openJson(statsFilePath))
        {
            LOG_EVENT(LOG_ERR, "Unable to open stats file\n");
            exit(EXIT_FAILURE);
        }

        // does entry for device already exist?
        std::vector<std::string> serialNumbers;
        if (!parser.getSerialNumbers(&serialNumbers))
        {
            LOG_EVENT(
                LOG_ERR, "Unable to retrieve serial numbers from json file\n");
            exit(EXIT_FAILURE);
        }
        if (std::find(serialNumbers.begin(), serialNumbers.end(),
                targetDevice.serialNumber)
            != serialNumbers.end())
        {
            // device exists in json already
            if (!parser.getPath(
                    targetDevice.serialNumber, &targetDevice.previousPath))
            {
                LOG_EVENT(LOG_ERR, "Unable to read previousPath\n");
                exit(EXIT_FAILURE);
            }
            if (!parser.getStats(
                    targetDevice.serialNumber, &targetDevice.stats))
            {
                LOG_EVENT(LOG_ERR, "Unable to read device stats\n");
                exit(EXIT_FAILURE);
            }
            if (!parser.getTotalBytesWritten(
                    targetDevice.serialNumber, &targetDevice.totalBytesWritten))
            {
                LOG_EVENT(LOG_ERR, "Unable to read totalBytesWritten\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // no existing device data in json
            targetDevice.previousPath = devicePath;
        }
    }
}

gboolean updateStats(gpointer data)
{
    LOG_EVENT(LOG_INFO, "Updating device stats for [%s]\n",
        targetDevice.serialNumber);

    int previousWriteSectors = targetDevice.stats.writeSectors;

    // get new values
    if (!reader.getStats(deviceName, &targetDevice.stats))
    {
        LOG_EVENT(LOG_ERR, "Unable to read device stats\n");
        exit(EXIT_FAILURE);
    }

    targetDevice.totalBytesWritten = computer.totalBytesWritten(sectorSize,
        targetDevice.stats.writeSectors, previousWriteSectors,
        targetDevice.totalBytesWritten);

    if (!writer.writeJson(statsFilePath, statsFilePath,
            targetDevice.serialNumber, devicePath, &targetDevice.stats,
            targetDevice.totalBytesWritten))
    {
        LOG_EVENT(LOG_ERR, "Unable to write device stats to file\n");
        exit(EXIT_FAILURE);
    }

    return TRUE;
}

void onExit(void)
{
    g_source_remove(timeoutId);
    if (pLoop)
    {
        g_main_loop_unref(pLoop);
    }
    parser.closeJson();
    if (pError)
    {
        g_object_unref(pError);
    }
    if (pContext)
    {
        g_object_unref(pContext);
    }
    LogEventDeinit();
}

int main(int argc, char* argv[])
{
    LogEventInit(basename(argv[0]), 6);
    LOG_EVENT(LOG_INFO, "Starting app.");

    if (atexit(onExit))
    {
        LOG_EVENT(LOG_ERR, "failed to register onExit()");
        return EXIT_FAILURE;
    }

    pContext = g_option_context_new(
        "- track read/write stats for SD storage devices");
    g_option_context_add_main_entries(pContext, options, NULL);
    if (!g_option_context_parse(pContext, &argc, &argv, &pError))
    {
        LOG_EVENT(LOG_ERR, "option parsing failed: %s\n", pError->message);
        return EXIT_FAILURE;
    }
    if (printBlockDevices)
    {
        printAllBlockDevices();
    }
    // convert cstring to std::string
    statsFilePath = (std::string)statsFilePath.c_str();
    deviceName    = (std::string)deviceName.c_str();
    devicePath    = (std::string)devicePath.c_str();

    getSerialNumber();

    // parse stats json file
    parseFile();

    // loop & check
    pLoop = g_main_loop_new(nullptr, FALSE);
    if (pLoop == nullptr)
    {
        LOG_EVENT(LOG_ERR, "failed to create g_main_loop");
        return EXIT_FAILURE;
    }

    timeoutId = g_timeout_add(updateRate, updateStats, pLoop);
    g_main_loop_run(pLoop);

    return EXIT_SUCCESS;
}
