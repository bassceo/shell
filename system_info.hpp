
#ifndef SYSTEM_INFO_HPP
#define SYSTEM_INFO_HPP

#include <thread>
#include <unistd.h>

struct SystemInfo {
    static int getNumCPUs() {
        return std::thread::hardware_concurrency();
    }
    
    static long getPageSize() {
        return sysconf(_SC_PAGESIZE);
    }
    
    static long getPhysicalMemory() {
        return sysconf(_SC_PHYS_PAGES) * getPageSize();
    }
};

#endif