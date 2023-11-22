#include "cSDWriter.h"

// Public functions

void cSDWriter::randWrite(
    std::string devicePath,
    uint blockSize,
    uint blockCount,
    int randSeed)
{
    // open file
    std::ofstream device;
    openDevice(devicePath, device);

    // RNG seed
    std::srand(randSeed);

    for (int count = 0; count < blockCount; count++)
    {
        // generate RNG data
        uint8_t data[blockSize];
        for (uint byte = 0; byte < blockSize; byte++)
        {
            data[byte] = std::rand() % 256;
        }

        // write data
        device.write((char*)&data[0], blockSize);
    }

    closeDevice(device);
}

void cSDWriter::seqWrite(
    std::string devicePath,
    uint blockSize,
    uint blockCount)
{
    // open file
    std::ofstream device;
    openDevice(devicePath, device);

    // write data
    for (int count = 0; count < blockCount; count++)
    {
        // generate data
        uint8_t data[blockSize];
        for (uint byte = 0; byte < blockSize; byte++)
        {
            data[byte] = 0xff;
        }

        device.write((char*)&data[0], blockSize);
    }

    closeDevice(device);
}

// Private functions

void cSDWriter::openDevice(
    std::string devicePath,
    std::ofstream& file)
{
    int status = EXIT_SUCCESS;

    file.open(devicePath);

    // disbled buffering, use direct io
    file.rdbuf()->pubsetbuf(0, 0);
}

void cSDWriter::closeDevice(
    std::ofstream& file)
{
    file.close();
}