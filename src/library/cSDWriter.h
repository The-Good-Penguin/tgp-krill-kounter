// cSDWriter.h
#ifndef _CSDWRITER_H
#define _CSDWRITER_H

#include <fstream>
#include <string>

class cSDWriter
{
    public:
        void randWrite(
            std::string devicePath,
            uint blockSize,
            uint blockCount,
            int randSeed);
        void seqWrite(
            std::string devicePath,
            uint blockSize,
            uint blockCount);

    private:
        void openDevice(
            std::string devicePath,
            std::ofstream& file);
        void closeDevice(
            std::ofstream& file);
};

#endif /* _CSDWRITER_H */