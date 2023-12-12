#include "cStatReader.hh"
#include "../utils/log-event.hh"

#include <errno.h>
#include <filesystem>
#include <fstream>
#include <string.h>

// public functions

std::vector<std::string> cStatReader::findDevices(void)
{
    // Return paths of all block devices

    std::vector<std::string> paths;
    for (const auto& entry : std::filesystem::directory_iterator("/dev/"))
    {
        if (entry.is_block_file())
        {
            paths.push_back(entry.path());
        }
    }
    return paths;
}

bool cStatReader::getSpaceInfo(std::string deviceName, uintmax_t* pValue)
{
    // Check path is valid
    const std::filesystem::path device = ("/dev/" + deviceName);
    if (std::filesystem::exists(device) == false)
    {
        LOG_EVENT(LOG_ERR, "Device does not exist");
        return false; // failure
    }
    if (std::filesystem::is_block_file(device) == false)
    {
        LOG_EVENT(LOG_ERR, "Path is not a block device");
        return false; // failure
    }

    // get size
    auto ifs = std::ifstream("/sys/block/" + deviceName + "/size");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device size");
        return false; // failure
    }
    std::string response;
    std::getline(ifs, response);
    *pValue = (uintmax_t)std::stoi(response);

    return true; // success
}

bool cStatReader::getStats(std::string deviceName, struct sBlockStats* pStats)
{
    // get /sys/block/<dev>/stat
    auto ifs = std::ifstream("/sys/block/" + deviceName + "/stat");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device stats");
        return false; // failure
    }
    std::string line;
    std::getline(ifs, line);

    if (line.empty())
    {
        LOG_EVENT(LOG_ERR, "Device does not exist");
        return false; // failure
    }

    // carve up data into struct
    std::istringstream strm(line);
    std::vector<std::string> vec;
    std::string s;

    while (strm >> s)
        // loop for each string and add to the vector
        vec.push_back(s);

    pStats->readIo         = std::stoi(vec[0]);
    pStats->readMerges     = std::stoi(vec[1]);
    pStats->readSectors    = std::stoi(vec[2]);
    pStats->readTicks      = std::stoi(vec[3]);
    pStats->writeIo        = std::stoi(vec[4]);
    pStats->writeMerges    = std::stoi(vec[5]);
    pStats->writeSectors   = std::stoi(vec[6]);
    pStats->writeTicks     = std::stoi(vec[7]);
    pStats->inFlight       = std::stoi(vec[8]);
    pStats->ioTicks        = std::stoi(vec[9]);
    pStats->timeInQueue    = std::stoi(vec[10]);
    pStats->discardIo      = std::stoi(vec[11]);
    pStats->discardMerges  = std::stoi(vec[12]);
    pStats->discardSectors = std::stoi(vec[13]);
    pStats->discardTicks   = std::stoi(vec[14]);

    return true; // success
}

bool cStatReader::getSpecs(std::string deviceName, struct sDeviceSpecs* pSpecs)
{

    // attempt to access CID
    if (getSpecsEmmc(deviceName, pSpecs))
    {
        return true; // success
    }

    // clearup failed CID read
    pSpecs->manfid.enabled = false;
    pSpecs->oemid.enabled  = false;
    pSpecs->name.enabled   = false;
    pSpecs->hwrev.enabled  = false;
    pSpecs->fwrev.enabled  = false;
    pSpecs->serial.enabled = false;
    pSpecs->mdt.enabled    = false;

    // attempt fallback method
    if (getSerialNumberFallback(deviceName, pSpecs))
    {
        return true; // success
    }

    return false; // failure
}

// private functions

bool cStatReader::getSpecsEmmc(
    std::string deviceName, struct sDeviceSpecs* pSpecs)
{
    /*
    Likely to fail when reading from SD Cards connected via a USB SD Card
    reader, as these often don't allow access to the CID register of
    inserted cards. A workaround for this is to use a built-in SD Card
    reader (e.g. integrated into a laptop), as these usually do allow
    CID access.

    https://www.cameramemoryspeed.com/sd-memory-card-faq/reading-sd-card-cid-serial-psn-internal-numbers/
    */

    std::string devicePath = "/sys/block/" + deviceName + "/device/block";

    // Check path is valid
    if (std::filesystem::exists(devicePath) == false)
    {
        LOG_EVENT(LOG_ERR, "Failed to open devicePath, doesn't exist");
        return false; // failure
    }

    // Manufacturer ID
    auto ifs = std::ifstream(devicePath + "/manfid");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Manufacturer ID");
        return false; // failure
    }
    std::getline(ifs, pSpecs->manfid.value);
    pSpecs->manfid.enabled = true;

    // OEM ID
    ifs = std::ifstream(devicePath + "/oemid");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get OEM ID");
        return false; // failure
    }
    std::getline(ifs, pSpecs->oemid.value);
    pSpecs->oemid.enabled = true;

    // Name
    ifs = std::ifstream(devicePath + "/name");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Device Name");
        return false; // failure
    }
    std::getline(ifs, pSpecs->name.value);
    pSpecs->name.enabled = true;

    // Hardware Revision
    ifs = std::ifstream(devicePath + "/hwrev");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Hardware Revision");
        return false; // failure
    }
    std::getline(ifs, pSpecs->hwrev.value);
    pSpecs->hwrev.enabled = true;

    // Firmware Revision
    ifs = std::ifstream(devicePath + "/fwrev");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Firmware Revision");
        return false; // failure
    }
    std::getline(ifs, pSpecs->fwrev.value);
    pSpecs->fwrev.enabled = true;

    // Serial number
    ifs = std::ifstream(devicePath + "/serial");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device Serial Number");
        return false; // failure
    }
    std::getline(ifs, pSpecs->serial.value);
    pSpecs->serial.enabled = true;

    // Manufacturing Date
    ifs = std::ifstream(devicePath + "/date");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device Manufacturing Date");
        return false; // failure
    }
    std::getline(ifs, pSpecs->mdt.value);
    pSpecs->mdt.enabled = true;

    ifs.close();
    return true; // success
}

bool cStatReader::getSerialNumberFallback(
    std::string deviceName, struct sDeviceSpecs* pSpecs)
{
    FILE* pFile;
    char output = 0;

    std::string command = "lsblk --raw -n -o serial /dev/" + deviceName + " -a";

    pFile = (FILE*)popen(command.c_str(), "r");
    if (0 == pFile)
    {
        LOG_EVENT(LOG_ERR, "Failed to open pFile, ", strerror(errno));
        return false; // failure
    }

    while (fread(&output, sizeof output, 1, pFile))
    {
        // only first line of lsblk output is relevant
        if (output == '\n')
        {
            break;
        }
        pSpecs->serial.value.push_back(output);
    }
    pSpecs->serial.enabled = true;

    if (pclose(pFile))
    {
        LOG_EVENT(LOG_ERR, "Failed to close pFile, ", strerror(errno));
        return false; // failure
    }

    return true; // success
}
