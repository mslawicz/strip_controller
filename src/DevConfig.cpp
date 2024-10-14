/*
 * DevConfig.cpp
 *
 *  Created on: Aug 23, 2024
 *      Author: ep14231
 */

#include "DevConfig.h"
#include <string.h>

namespace chip {
namespace DeviceLayer {
namespace Internal {

void DevConfig::VerifyAndConfigure (void)
{
    const char* VendorName = "MS Controllers";
    const char* ProductName = "Strip Controller";
    constexpr size_t MaxLen = 32;
    char strBuf[MaxLen];
    size_t len;
    CHIP_ERROR err;

    err = ReadConfigValueStr(kConfigKey_VendorName, strBuf, MaxLen, len);
    if((CHIP_DEVICE_ERROR_CONFIG_NOT_FOUND == err) ||
        (strlen(VendorName) != len) ||
        (strcmp(VendorName, strBuf) != 0))
    {
        // set vendor name in NV memory
        WriteConfigValueStr(kConfigKey_VendorName, VendorName, strlen(VendorName));
    }

    err = ReadConfigValueStr(kConfigKey_ProductName, strBuf, MaxLen, len);
    if((CHIP_DEVICE_ERROR_CONFIG_NOT_FOUND == err) ||
        (strlen(ProductName) != len) ||
        (strcmp(ProductName, strBuf) != 0))
    {
        // replace product name in NV memory
        WriteConfigValueStr(kConfigKey_ProductName, ProductName, strlen(ProductName));
    }    
}

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip
