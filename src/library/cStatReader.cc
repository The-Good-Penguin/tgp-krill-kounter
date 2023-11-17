#include "cStatReader.hh"
#include "../utils/log-event.hh"

#include <filesystem>
#include <fstream>

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

uintmax_t cStatReader::getSpaceInfo(std::string deviceName)
{
    // Check path is valid
    const std::filesystem::path device = ("/dev/" + deviceName);
    if (std::filesystem::exists(device) == false)
    {
        LOG_EVENT(LOG_ERR, "Device does not exist");
        exit(EXIT_FAILURE);
    }
    if (std::filesystem::is_block_file(device) == false)
    {
        LOG_EVENT(LOG_ERR, "Path is not a block device");
        exit(EXIT_FAILURE);
    }

    // get size
    auto ifs = std::ifstream("/sys/block/" + deviceName + "/size");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device size");
        exit(EXIT_FAILURE);
    }
    std::string response;
    std::getline(ifs, response);
    return (uintmax_t)std::stoi(response);
}

void cStatReader::getStats(std::string deviceName, struct sBlockStats* pStats)
{
    // get /sys/block/<dev>/stat
    auto ifs = std::ifstream("/sys/block/" + deviceName + "/stat");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device stats");
        exit(EXIT_FAILURE);
    }
    std::string line;
    std::getline(ifs, line);

    if (line.empty())
    {
        LOG_EVENT(LOG_ERR, "Device does not exist");
        exit(EXIT_FAILURE);
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
}

void cStatReader::getSpecs(std::string devicePath, struct sDeviceSpecs* pSpecs)
{
    /*
    Likely to fail when reading from SD Cards connected via a USB SD Card
    reader, as these often don't allow access to the CID register of
    inserted cards. A workaround for this is to use a built-in SD Card
    reader (e.g. integrated into a laptop), as these usually do allow
    CID access.

    https://www.cameramemoryspeed.com/sd-memory-card-faq/reading-sd-card-cid-serial-psn-internal-numbers/
    */

    // Check path is valid
    const std::filesystem::path device = (devicePath);
    if (std::filesystem::exists(device) == false)
    {
        LOG_EVENT(LOG_ERR, "Failed to open devicePath, doesn't exist");
        exit(EXIT_FAILURE);
    }

    // Manufacturer ID
    auto ifs = std::ifstream(devicePath + "/manfid");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Manufacturer ID");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->manfid);

    // OEM ID
    ifs = std::ifstream(devicePath + "/oemid");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get OEM ID");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->oemid);

    // Name
    ifs = std::ifstream(devicePath + "/name");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Device Name");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->name);

    // Hardware Revision
    ifs = std::ifstream(devicePath + "/hwrev");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Hardware Revision");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->hwrev);

    // Firmware Revision
    ifs = std::ifstream(devicePath + "/fwrev");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get Firmware Revision");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->fwrev);

    // Serial number
    ifs = std::ifstream(devicePath + "/serial");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device Serial Number");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->serial);

    // Manufacturing Date
    ifs = std::ifstream(devicePath + "/date");
    if (ifs.is_open() != true)
    {
        LOG_EVENT(LOG_ERR, "Failed to get device Manufacturing Date");
        exit(EXIT_FAILURE);
    }
    std::getline(ifs, pSpecs->mdt);

    ifs.close();
}
