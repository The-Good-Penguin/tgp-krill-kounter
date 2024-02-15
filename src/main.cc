#include <algorithm>
#include <filesystem>
#include <glib.h>
#include <iostream>
#include <json-glib/json-glib.h>
#include <map>
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

std::map<std::string, struct sDeviceEntry> targetDevices;
struct sJsonDevicesConfig targetConfig;

// converts the update rate to milliseconds
constexpr int   CONST_RATE_TO_MILLISECONDS   = 1000;
constexpr uint  CONST_SECTOR_SIZE            = 512;
constexpr std::string_view CONST_DEFAULT_CONFIG_PATH    = "/usr/share/KrillKounter/config.json";
constexpr std::string_view CONST_DEFAULT_STATS_PATH     = "/usr/share/KrillKounter/stats.json";

// glib variables
GError* pError           = nullptr;
GOptionContext* pContext = nullptr;
GMainLoop* pLoop         = nullptr;
guint timeoutId          = 0;

// cli values
gchar *cliStatsFilePath     = nullptr;
gchar *cliConfigFilePath    = nullptr;
gchar *cliDeviceName        = nullptr;
gchar *cliDevicePath        = nullptr;
uint   updateRate           = 3600; // seconds
bool   printBlockDevices    = false;
std::string configFilePath;

// cli arguments
GOptionEntry options[] = {
    { "config-file", 'c', 0, G_OPTION_ARG_FILENAME,
        &cliConfigFilePath, "JSON config file path" },
    { "stats-file", 's', 0, G_OPTION_ARG_FILENAME,
        &cliStatsFilePath, "JSON stats file path" },
    { "device-path", 'd', 0, G_OPTION_ARG_STRING,
        &cliDevicePath, "path of block device" },
    { "device-name", 'n', 0, G_OPTION_ARG_STRING,
        &cliDeviceName, "name of block device" },
    { "update-rate", 'r', 0, G_OPTION_ARG_INT,
        &updateRate, "update rate of checks (seconds)" },
    { "print-devices", 'p', 0, G_OPTION_ARG_NONE,
        &printBlockDevices, "print all available block devices" },
    { NULL }
};

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

void getSerialNumber(std::string devicePath)
{
    struct sDeviceSpecs specs;
    if (!targetDevices.contains(devicePath))
    {
        LOG_EVENT(LOG_ERR, "Unable to find [%s] in target devices\n", devicePath.c_str());
        exit(EXIT_FAILURE);
    }

    auto const deviceName = targetDevices[devicePath].deviceName;
    if (!reader.getSpecs(deviceName, &specs))
    {
        LOG_EVENT(LOG_ERR, "Unable to get device serial number\n");
        exit(EXIT_FAILURE);
    }
    targetDevices[devicePath].serialNumber = specs.serial.value;
}

gboolean parseConfigFile(void)
{
    gboolean ret = false; // failure
    if (!parser.openJson(configFilePath))
    {
        LOG_EVENT(LOG_ERR, "Unable to open config file\n");
        return false;
    }
    if (!parser.getConfig(&targetConfig))
    {
        LOG_EVENT(LOG_ERR, "Error processing config file\n");
        goto error;
    }

    if (targetConfig.devices.empty()) {
        LOG_EVENT(LOG_INFO, "No devices to monitor in config file\n");
        goto error;
    }

    for (const auto& devicePath : targetConfig.devices) {
        if (!std::filesystem::exists(devicePath)) {
            LOG_EVENT(LOG_ERR, "path [%s] does not exist\n", devicePath.c_str());
            exit(EXIT_FAILURE);
        }

        if (!std::filesystem::is_block_file(devicePath)) {
            LOG_EVENT(LOG_ERR, "[%s] is not a block device\n", devicePath.c_str());
            exit(EXIT_FAILURE);
        }

        targetDevices.insert({ devicePath,
            (struct sDeviceEntry) {
                .deviceName = devicePath.substr(devicePath.find_last_of("/") + 1),
                .devicePath = devicePath }
        });
    }
    ret = true; // success

error:
    if (!parser.closeJson())
    {
        LOG_EVENT(LOG_ERR, "Unable to close JSON reader\n");
        return false;
    }
    return ret;
}

void parseStatsFile(void)
{
    // Check if file exists
    const std::filesystem::path statsFile = targetConfig.statsFilePath;
    if (std::filesystem::exists(statsFile))
    {
        if (!parser.openJson(targetConfig.statsFilePath))
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

        for (auto &[devicePath, targetDevice] : targetDevices)
        {
            if (std::find(serialNumbers.begin(), serialNumbers.end(),
                    targetDevice.serialNumber)
                != serialNumbers.end())
            {
                // device exists in json already
                if (!parser.getStats(
                        targetDevice.serialNumber, &targetDevice.outputStats))
                {
                    LOG_EVENT(LOG_ERR, "Unable to read device stats\n");
                    exit(EXIT_FAILURE);
                }

                if (!reader.getStats(
                        targetDevice.deviceName, &targetDevice.stats))
                {
                    LOG_EVENT(LOG_ERR, "Unable to read device stats\n");
                    exit(EXIT_FAILURE);
                }

                if (!parser.getDiskSeq(
                        targetDevice.serialNumber, &targetDevice.diskSeq))
                {
                    LOG_EVENT(LOG_ERR, "Unable to read disk sequence\n");
                    exit(EXIT_FAILURE);
                }

                if (!parser.getTotalBytesWritten(targetDevice.serialNumber,
                        &targetDevice.totalBytesWritten))
                {
                    LOG_EVENT(LOG_ERR, "Unable to read totalBytesWritten\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

void updateStats(struct sDeviceEntry *targetDevice)
{
    LOG_EVENT(LOG_INFO, "Updating device stats for [%s]\n",
        targetDevice->serialNumber.c_str());

    auto previousDiskSeq = targetDevice->diskSeq;
    auto previousStats = targetDevice->stats;

    // get sequence
    if (!reader.getDiskSeq(targetDevice->deviceName, &targetDevice->diskSeq))
    {
        LOG_EVENT(LOG_ERR, "Unable to read device sequence\n");
        exit(EXIT_FAILURE);
    }

    // reset previous stats if disk sequence has changed
    if (targetDevice->diskSeq != previousDiskSeq)
        previousStats = {};

    // get new values
    if (!reader.getStats(targetDevice->deviceName, &targetDevice->stats))
    {
        LOG_EVENT(LOG_ERR, "Unable to read device stats\n");
        exit(EXIT_FAILURE);
    }

    // return if the stats haven't changed
    if (targetDevice->stats == previousStats)
        return;

    computer.updateStats(&previousStats,
        &targetDevice->stats, &targetDevice->outputStats);


    targetDevice->totalBytesWritten = computer.totalBytesWritten(CONST_SECTOR_SIZE,
        targetDevice->stats.writeSectors, previousStats.writeSectors,
        targetDevice->totalBytesWritten);

    if (!writer.writeJson(targetConfig.statsFilePath,
            targetConfig.statsFilePath, targetDevice->serialNumber,
            targetDevice->devicePath, &targetDevice->outputStats,
            targetDevice->diskSeq, targetDevice->totalBytesWritten))
    {
        LOG_EVENT(LOG_ERR, "Unable to write device stats to file\n");
        exit(EXIT_FAILURE);
    }
}

inline void updateAllDeviceStats(void)
{
    for (auto const & device : targetConfig.devices)
        updateStats(&targetDevices[device]);
}

gboolean timerCallback(gpointer data)
{
    updateAllDeviceStats();
    return true;
}

gboolean checkStatsFilePath()
{
    FILE *pFile;

    LOG_EVENT(LOG_INFO, "Checking stats path [%s]\n",
        targetConfig.statsFilePath.c_str());
    /* if the file is in the root directory or has no directory */
    auto ret = targetConfig.statsFilePath.find_last_of('/');
    if (ret == 0 || ret == std::string::npos)
        return true; // success


    auto statsDirectory = targetConfig.statsFilePath.substr(0, ret);
    if (!std::filesystem::exists(statsDirectory)) {
        std::string command = "mkdir -p " + statsDirectory;
        pFile = popen(command.c_str(), "r");
        if (nullptr == pFile)
        {
            LOG_EVENT(LOG_ERR, "Failed to open pFile, %s", strerror(errno));
            return false; // failure
        }

        if (pclose(pFile))
        {
            LOG_EVENT(LOG_ERR, "Failed to close pFile, %s", strerror(errno));
            return false; // failure
        }
    } else if (!std::filesystem::is_directory(statsDirectory)) {
        LOG_EVENT(LOG_ERR, "Error, [%s] exists and is not a directory\n", statsDirectory.c_str());
        return false; // failure
    }
    LOG_EVENT(LOG_INFO, "[%s] already exists and is a directory\n", statsDirectory.c_str());
    return true; // success
}

void onExit(void)
{
    if (timeoutId)
    {
        g_source_remove(timeoutId);
    }
    if (pLoop)
    {
        g_main_loop_unref(pLoop);
    }
    parser.closeJson();
    if (pError)
    {
        g_error_free(pError);
    }
    if (pContext)
    {
        g_option_context_free(pContext);
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

    configFilePath = cliConfigFilePath == nullptr
        ? CONST_DEFAULT_CONFIG_PATH : (std::string)cliConfigFilePath;

    targetConfig.updateRate = updateRate;
    targetConfig.statsFilePath = cliStatsFilePath == nullptr
        ? CONST_DEFAULT_STATS_PATH : (std::string)cliStatsFilePath;

    if (parseConfigFile() == false) {
        if (cliDevicePath == nullptr || cliDeviceName == nullptr) {
            LOG_EVENT(LOG_ERR, "deviceName and devicePath should be present "
                               "if config is missing/invalid");
            exit(EXIT_FAILURE);
        }

        targetDevices.insert({(std::string )cliDevicePath,
            (struct sDeviceEntry) {
                .deviceName = (std::string )cliDeviceName,
                .devicePath = (std::string )cliDevicePath,

            }});
    }

    if (checkStatsFilePath() == false)
        exit(EXIT_FAILURE);

    for (const auto& device : targetConfig.devices)
    {
        getSerialNumber(device);
    }

    // parse stats json file
    parseStatsFile();

    // loop & check
    pLoop = g_main_loop_new(nullptr, FALSE);
    if (pLoop == nullptr)
    {
        LOG_EVENT(LOG_ERR, "failed to create g_main_loop");
        return EXIT_FAILURE;
    }

    updateAllDeviceStats();

    timeoutId = g_timeout_add(updateRate * CONST_RATE_TO_MILLISECONDS, timerCallback, pLoop);
    g_main_loop_run(pLoop);

    return EXIT_SUCCESS;
}
