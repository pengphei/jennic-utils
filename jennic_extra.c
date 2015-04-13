#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "jennic_extra.h"


typedef struct
{
    u_int8_t     u8ConfigByte0;
    u_int8_t     u8ConfigByte1;
    u_int16_t    u16SpiScrambleIndex;
    u_int32_t    u32TextStartAddress;
    u_int32_t    u32TextLength;
    u_int32_t    u32ROMVersion;
    u_int32_t    u32Unused1;
    u_int32_t    u32BssStartAddress;
    u_int32_t    u32BssLength;
    u_int32_t    u32WakeUpEntryPoint;
    u_int32_t    u32ResetEntryPoint;
    u_int8_t     au8OadData[12];
    u_int8_t     u8TextDataStart;
} _stheader_v1_t, *_pstheader_v1_t;


typedef struct
{
    u_int32_t    u32ROMVersion;
    u_int32_t    au32BootImageRecord[4];
    u_int64_t    u64MacAddress;
    u_int32_t    au32EncryptionInitialisationVector[4];
    u_int32_t    u32DataSectionInfo;
    u_int32_t    u32BssSectionInfo;
    u_int32_t    u32WakeUpEntryPoint;
    u_int32_t    u32ResetEntryPoint;
    u_int8_t     u8TextDataStart;
} _stheader_v2_t, *_pstheader_v2_t;

int jennic_pring_fw_info(u_int8_t *pfirmware, pstzb_firmware_t pfirm)
{
    _pstheader_v2_t ph2 = (_pstheader_v2_t)pfirmware;

    // JN5148-J01 onwards uses multiimage bootloader - check for it's magic number.
    if ((ntohl(ph2->au32BootImageRecord[0]) == 0x12345678) &&
        (ntohl(ph2->au32BootImageRecord[1]) == 0x11223344) &&
        (ntohl(ph2->au32BootImageRecord[2]) == 0x55667788))
    {
        printf("Module uses Bootloader v2 Header\n");

        pfirm->u32ROMVersion                = ntohl(ph2->u32ROMVersion);

        pfirm->u32TextSectionLoadAddress    = 0x04000000 + (((ntohl(ph2->u32DataSectionInfo)) >> 16) * 4);
        pfirm->u32TextSectionLength         = (((ntohl(ph2->u32DataSectionInfo)) & 0x0000FFFF) * 4);
        pfirm->u32BssSectionLoadAddress     = 0x04000000 + (((ntohl(ph2->u32BssSectionInfo)) >> 16) * 4);
        pfirm->u32BssSectionLength          = (((ntohl(ph2->u32BssSectionInfo)) & 0x0000FFFF) * 4);

        pfirm->u32ResetEntryPoint           = ph2->u32ResetEntryPoint;
        pfirm->u32WakeUpEntryPoint          = ph2->u32WakeUpEntryPoint;

        /* Pointer to and length of image for flash */
        pfirm->pu8ImageData                 = (u_int8_t*)&(ph2->au32BootImageRecord[0]);

        pfirm->u32MacAddressLocation        = 0x10;

        /* Pointer to text section in image for RAM */
        pfirm->pu8TextData                  = &(ph2->u8TextDataStart);
    }
    else
    {
        _pstheader_v1_t ph1              = (_pstheader_v1_t)pfirmware;

        printf("Module uses Bootloader v1 Header\n");

        pfirm->u32ROMVersion                = ntohl(ph1->u32ROMVersion);

        pfirm->u32TextSectionLoadAddress    = ntohl(ph1->u32TextStartAddress);
        pfirm->u32TextSectionLength         = ntohl(ph1->u32TextLength);
        pfirm->u32BssSectionLoadAddress     = ntohl(ph1->u32BssStartAddress);
        pfirm->u32BssSectionLength          = ntohl(ph1->u32BssLength);

        pfirm->u32ResetEntryPoint           = ph1->u32ResetEntryPoint;
        pfirm->u32WakeUpEntryPoint          = ph1->u32WakeUpEntryPoint;

        /* Pointer to and length of image for flash */
        pfirm->pu8ImageData                 = &(ph1->u8ConfigByte0);
        //pfirm->u32ImageLength               = sizeof(_stheader_v1_t) + pfirm->u32TextSectionLength;

        pfirm->u32MacAddressLocation        = 0x30;

        /* Pointer to text section in image for RAM */
        pfirm->pu8TextData                  = &(ph1->u8TextDataStart);

    }

    printf("u32ROMVersion:         0x%08x\n", pfirm->u32ROMVersion);
    printf("u32DataSectionInfo:    0x%08x\n", ntohl(ph2->u32DataSectionInfo));
    printf("u32TextSectionLength:  0x%08x\n", (((ntohl(ph2->u32DataSectionInfo)) & 0x0000FFFF) * 4));

    return 0;
}
