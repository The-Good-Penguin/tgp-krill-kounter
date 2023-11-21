// cSDWriter.h
#ifndef _CSDWRITER_H
#define _CSDWRITER_H

#include <string>
#include <fstream>

class cSDWriter {
    public:
        void seqWrite(
            std::string devicePath,
            uint blockSize,
            uint blockCount,
            int randSeed
        );

    private:
        void openDevice(
            std::string devicePath,
            std::ofstream& file
        );
        void closeDevice(
            std::ofstream& file
        );
};

#endif /* _CSDWRITER_H */