/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JENNIC_EXTRA_H
#define JENNIC_EXTRA_H

#include <sys/types.h>

/** Abstracted firmware information common across bootloaders. */
typedef struct
{
    u_int32_t    u32ROMVersion;                  /**< ROM Version that the FW was built for */
    u_int32_t    u32TextSectionLoadAddress;      /**< Address to load .text section */
    u_int32_t    u32TextSectionLength;           /**< Length (bytes) of .text section */
    u_int32_t    u32BssSectionLoadAddress;       /**< Address of start of .bss section */
    u_int32_t    u32BssSectionLength;            /**< Length (bytes) of .bss section */
    u_int32_t    u32WakeUpEntryPoint;            /**< Address of wake up (warm start) entry point */
    u_int32_t    u32ResetEntryPoint;             /**< Address of rest (cold start) entry point */

    u_int8_t*    pu8ImageData;                   /**< Pointer to loaded image data for Flash */
    u_int32_t    u32ImageLength;                 /**< Length (bytes) of image for Flash */

    u_int32_t    u32MacAddressLocation;          /**< Offset in file of MAC address */
    u_int8_t*    pu8TextData;                    /**< Pointer to loaded .text section for RAM */
} stzb_firmware_t, *pstzb_firmware_t;

int jennic_pring_fw_info(u_int8_t *pfirmware, pstzb_firmware_t pfirm);

#endif // JENNIC_EXTRA_H
