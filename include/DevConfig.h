/*
 * DevConfig.h
 *
 *  Created on: Aug 23, 2024
 *      Author: ep14231
 */

#ifndef INCLUDE_DEVCONFIG_H_
#define INCLUDE_DEVCONFIG_H_

#include "SilabsConfig.h"

namespace chip {
namespace DeviceLayer {
namespace Internal {

class DevConfig : private SilabsConfig
{
public:    
    static void VerifyAndConfigure(void);
};

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip

#endif /* INCLUDE_DEVCONFIG_H_ */
