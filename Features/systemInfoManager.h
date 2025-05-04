#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class SystemInfoManager {
public:
    static void showSystemInfo() {
        cout << "System Information:" << endl;
        cout << "-------------------" << endl;

        showUptime();
        showCpuInfo();
        showMemInfo();
    }

private:
    static void showUptime() {
        ifstream uptimeFile("/proc/uptime");
        string uptime;
        if (uptimeFile) {
            getline(uptimeFile, uptime);
            cout << "Uptime: " << uptime << endl;
        } else {
            cout << "Unable to read uptime." << endl;
        }
    }

    static void showCpuInfo() {
        ifstream cpuInfoFile("/proc/cpuinfo");
        string line;
        cout << "CPU Information: " << endl;
        if (cpuInfoFile) {
            while (getline(cpuInfoFile, line)) {
                if (line.find("model name") != string::npos) {
                    cout << line << endl;
                }
            }
        } else {
            cout << "Unable to read CPU info." << endl;
        }
    }

    static void showMemInfo() {
        ifstream memInfoFile("/proc/meminfo");
        string line;
        cout << "Memory Information: " << endl;
        if (memInfoFile) {
            while (getline(memInfoFile, line)) {
                if (line.find("MemFree") != string::npos) {
                    cout << line << endl;
                }
            }
        } else {
            cout << "Unable to read memory info." << endl;
        }
    }
};
