#include <windows.h>
#include <setupapi.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "setupapi.lib")

bool findDevice(std::wstring deviceToFind) {
    // Check result
    bool found = false;

    // Get a handle to the device information set
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return found;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // Enumerate devices in the device information set
    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        DWORD dataType, bufferSize = 0;
        std::wstring deviceName;

        // Get the size of the buffer required
        SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_DEVICEDESC, // Get the device description
            &dataType,
            NULL,
            0,
            &bufferSize
        );

        if (bufferSize > 0) {
            std::vector<BYTE> buffer(bufferSize);
            if (SetupDiGetDeviceRegistryPropertyW(
                deviceInfoSet,
                &deviceInfoData,
                SPDRP_DEVICEDESC,
                &dataType,
                buffer.data(),
                bufferSize,
                NULL)) {
                // Convert buffer to a wide string
                deviceName = reinterpret_cast<wchar_t*>(buffer.data());
                size_t pos = deviceName.find(deviceToFind);
                if (pos != std::wstring::npos) {
                    std::wcout << deviceToFind << " device string matched, position is " << pos << std::endl;
                    found = true;
                    break;
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return found;
}

int main() {
    bool arcFound = findDevice(L"Intel(R) Arc(TM)");
    std::cout << "arcFound: " << arcFound << std::endl;

    bool intelNPUFound = findDevice(L"Intel(R) AI Boost");
    std::cout << "intelNPUFound: " << intelNPUFound << std::endl;

    bool testDeviceFound = findDevice(L"test device");
    std::cout << "testDeviceFound: " << testDeviceFound << std::endl;

    std::cin.get();
    return 0;
}