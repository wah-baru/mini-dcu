//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL:  $
//
// Version:         $Revision:  $,
//                  $Date:  $
//                  $Author: $
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#if defined(_WIN32) || defined(_WIN64) // Windows includes
#include <conio.h>
#include "../include/getopt.h"
#include <process.h> //Add support for threads
#if _MSC_VER > 1000
#include <crtdbg.h>
#endif
// Windows doesn't implement strcasecmp. It uses strcmpi.
#define strcasecmp _strcmpi
#else
#include <string.h> /* memset */
#include <unistd.h>
#include <strings.h>
#endif

#include "../include/communication.h"
#include "../../development/include/gxserializer.h"
unsigned char testMode = 1;
// Client don't need this.
void svr_preGet(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
void svr_postGet(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
void svr_preRead(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
void svr_preWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
void svr_preAction(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
extern void svr_postRead(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
extern void svr_postWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
extern void svr_postAction(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}

// Client don't need this.
unsigned char svr_isTarget(
    dlmsSettings *settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    return 0;
}

// Client don't need this.
int svr_connected(
    dlmsServerSettings *settings)
{
    return 0;
}

// Client don't need this.
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ_WRITE;
}

// Client don't need this.
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index)
{
    return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char *str, const char *data)
{
    // Send trace to the serial port in test mode.
    if (testMode)
    {
        if (data == NULL)
        {
            printf("%s\r\n", str);
        }
        else
        {
            printf("%s %s\r\n", str, data);
        }
    }
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_INT(const char *str, int32_t value)
{
    char data[10];
    sprintf(data, " %ld", value);
    GXTRACE(str, data);
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_LN(const char *str, uint16_t type, unsigned char *ln)
{
    char buff[30];
    sprintf(buff, "%d %d.%d.%d.%d.%d.%d", type, ln[0], ln[1], ln[2], ln[3], ln[4], ln[5]);
    GXTRACE(str, buff);
}

/**
 * Updates clock and reads it.
 */
int com_updateClock(connection *connection)
{
    int ret;
    gxClock clock;
    unsigned char ln[] = {0, 0, 1, 0, 0, 255};
    INIT_OBJECT(clock, DLMS_OBJECT_TYPE_CLOCK, ln);
    // Initialize connection.
    ret = com_initializeConnection(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    // Write clock.
    time_init(&clock.time, 2000, 1, 1, 0, 0, 0, 0, 0x8000);
    ret = com_write(connection, &clock.base, 2);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }

    // Read clock.
    ret = com_read(connection, &clock.base, 2);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    // Print clock.
    ret = time_print(NULL, &clock.time);
    return ret;
}

/**
 * Calls disconnect method.
 */
int disconnect(connection *connection)
{
    int ret;
    gxDisconnectControl dc;
    unsigned char ln[] = {0, 0, 96, 3, 10, 255};
    INIT_OBJECT(dc, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln);
    // Call Disconnect action.
    dlmsVARIANT param;
    GX_INT8(param) = 0;
    ret = com_method(connection, &dc.base, 1, &param);
    return ret;
}

/**
 * Calls capture of the profile generic object..
 */
int CaptureProfileGeneric(connection *connection)
{
    printf("CaptureProfileGeneric - Start\n");
    int ret;
    gxProfileGeneric pg;
    unsigned char ln[] = {1, 0, 98, 1, 0, 255};
    INIT_OBJECT(pg, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln);
    // Invokes capture action.
    dlmsVARIANT param;
    GX_INT8(param) = 0;
    ret = com_method(connection, BASE(pg), 2, &param);
    printf("CaptureProfileGeneric - Stop\n");
    return ret;
}

/**
* Activates and strengthens the security policy
  for the security setup object
*/
int SecurityActivate(connection *connection,
                     DLMS_SECURITY_POLICY policy)
{
    int ret;
    gxSecuritySetup ss;
    unsigned char ln[] = {0, 0, 43, 0, 0, 255};
    INIT_OBJECT(ss, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln);
    // Invokes capture action.
    dlmsVARIANT param;
    var_setEnum(&param, policy);
    GX_ENUM(param) = policy;
    ret = com_method(connection, BASE(ss), 1, &param);
    return ret;
}

// Show PDU before it's encrypted or
// after it's decrypted when DLMS_TRACE_PDU
// is defined.
void cip_tracePdu(
    unsigned char encrypt,
    gxByteBuffer *pdu)
{
    const char *direction = encrypt ? "TX" : "RX";
    char *str = bb_toHexString(pdu);
    printf("\r\n%s PDU: %s\r\n", direction, str);
    free(str);
}

/**
 * Executes selected script.
 * params:
 * ln: Logical name of the script table object.
 * scriptId: Script Id.
 */
int execute(connection *connection, const unsigned char *ln, uint16_t scriptId)
{
    int ret;
    gxScriptTable st;
    INIT_OBJECT(st, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln);
    // Call script id.
    dlmsVARIANT param;
    GX_UINT16(param) = scriptId;
    ret = com_method(connection, &st.base, 1, &param);
    return ret;
}

/**
* Update firmware of the meter.
*
* In image update following steps are made:
1. Image_transfer_enabled is read.
2. Image block size is read.
3. image_transferred_blocks_status is read to check is image try to update before.
4. image_transfer_initiate
5. image_transfer_status is read.
6. image_block_transfer
7. image_transfer_status is read.
8. image_transfer_status is read.
9. image_verify is called.
10. image_transfer_status is read.
11. image_activate is called.
*/
int imageUpdate(connection *connection, const unsigned char *identification, uint16_t identificationSize, unsigned char *image, uint32_t imageSize)
{
    int ret;
    gxByteBuffer bb;
    bb_init(&bb);
    dlmsVARIANT param;
    gxImageTransfer im;
    unsigned char ln[] = {0, 0, 44, 0, 0, 255};
    INIT_OBJECT(im, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln);

    // 1. Image_transfer_enabled is read.
    if ((ret = com_read(connection, BASE(im), 5)) == 0 &&
        // 2. Image block size is read.
        (ret = com_read(connection, BASE(im), 2)) == 0 &&
        // 3. image_transferred_blocks_status is read to check is image try to update before.
        (ret = com_read(connection, BASE(im), 3)) == 0 &&
        // 4. image_transfer_initiate
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_STRUCTURE)) == 0 &&
        (ret = bb_setInt8(&bb, 2)) == 0 &&
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_OCTET_STRING)) == 0 &&
        (ret = hlp_setObjectCount(identificationSize, &bb)) == 0 &&
        (ret = bb_set(&bb, identification, identificationSize)) == 0 &&
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_UINT32)) == 0 &&
        (ret = bb_setInt32(&bb, imageSize)) == 0 &&
        (ret = com_method3(connection, BASE(im), 1, &bb)) == 0)
    {
        // 5. image_transfer_status is read.
        if ((ret = com_read(connection, BASE(im), 6)) == 0)
        {
            // 6. image_block_transfer
            uint32_t count = im.imageBlockSize;
            uint32_t blockNumber = 0;
            while (imageSize != 0)
            {
                if (imageSize < im.imageBlockSize)
                {
                    count = imageSize;
                }
                bb_clear(&bb);
                if ((ret = bb_setInt8(&bb, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setInt8(&bb, 2)) != 0 ||
                    (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_UINT32)) != 0 ||
                    (ret = bb_setInt32(&bb, blockNumber)) != 0 ||
                    (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(count, &bb)) != 0 ||
                    (ret = bb_set(&bb, image, count)) != 0 ||
                    (ret = com_method3(connection, BASE(im), 2, &bb)) != 0)
                {
                    break;
                }
                imageSize -= count;
                ++blockNumber;
            }
            if (ret == 0)
            {
                // 7. image_transfer_status is read.
                ret = com_read(connection, BASE(im), 6);
                if (ret == 0)
                {
                    // 9. image_verify is called.
                    GX_INT8(param) = 0;
                    if ((ret = com_method(connection, BASE(im), 3, &param)) == 0 ||
                        ret == DLMS_ERROR_CODE_TEMPORARY_FAILURE)
                    {
                        while (1)
                        {
                            // 10. image_transfer_status is read.
                            ret = com_read(connection, BASE(im), 6);
                            if (im.imageTransferStatus == DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL)
                            {
                                break;
                            }
                            if (im.imageTransferStatus == DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED)
                            {
                                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                                break;
                            }

                            // Wait until image is activated.
#if defined(_WIN32) || defined(_WIN64) // Windows
                            Sleep(10000);
#else
                            usleep(1000000);
#endif // defined(_WIN32) || defined(_WIN64)
                        }
                        if (ret == 0)
                        {
                            ret = com_read(connection, BASE(im), 6);
                            // 11. image_activate is called.
                            ret = com_method(connection, BASE(im), 4, &param);
                        }
                    }
                }
            }
        }
    }
    bb_clear(&bb);
    return ret;
}

/*
 * This method can be used to update firmware from the hex file.
 */
int imageUpdateFromFile(
    connection *connection,
    const char *identifier,
    const char *fileName)
{
#if _MSC_VER > 1400
    FILE *f = NULL;
    fopen_s(&f, fileName, "r");
#else
    FILE *f = fopen(fileName, "r");
#endif
    int ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    if (f != NULL)
    {
        size_t imagesize;
        char image[101];
        gxByteBuffer bb;
        bb_init(&bb);
        while (feof(f) == 0)
        {
            imagesize = fread(image, sizeof(char), sizeof(image) - 1, f);
            image[imagesize] = 0;
            bb_addHexString(&bb, image);
        }
        fclose(f);
        ret = imageUpdate(connection, identifier, (uint16_t)strlen(identifier), bb.data, bb.size);
        bb_clear(&bb);
    }
    return ret;
}

/*Read DLMS meter using TCP/IP connection.*/
int readTcpIpConnection(
    connection *connection,
    const char *address,
    const int port,
    char *readObjects,
    const char *invocationCounter,
    const char *outputFile)
{
    int ret;
#if defined(_WIN32) || defined(_WIN64) // Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("Connecting to %s:%d\n", address, port);
    }
    // Make connection to the meter.
    ret = com_makeConnect(connection, address, port, 5000);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        con_close(connection);
        com_close(connection);
        return ret;
    }
    if (readObjects != NULL)
    {
        if ((ret = com_updateInvocationCounter(connection, invocationCounter)) == 0 &&
            (ret = com_initializeConnection(connection)) == 0)
        // &&            (ret = com_getAssociationView(connection, outputFile)) == 0)
        {
            int index;
            unsigned char buff[200];
            gxObject *obj = NULL;
            char *p2, *p = readObjects;
            do
            {
                if (p != readObjects)
                {
                    ++p;
                }

                p2 = strchr(p, ':');
                *p2 = '\0';
                ++p2;
#if defined(_WIN32) || defined(_WIN64) // Windows
                sscanf_s(p2, "%d", &index);
#else
                sscanf(p2, "%d", &index);
#endif
                hlp_setLogicalName(buff, p);
                if ((ret = oa_findByLN(&connection->settings.objects, DLMS_OBJECT_TYPE_NONE, buff, &obj)) == 0)
                {
                    if (obj == NULL)
                    {
                        printf("Object '%s' not found from the association view.\n", p);
                        break;
                    }
                    // Capture objects are read first if the buffer of the profile generic is read.
                    if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC && index == 2)
                    {
                        if ((ret = com_readValue(connection, obj, 3)) != 0)
                        {
                            break;
                        }
                    }
                    ret = com_readValue(connection, obj, index);
                }
            } while ((p = strchr(p2, ',')) != NULL);
        }
    }
    else
    {
        // Initialize connection.
        if ((ret = com_updateInvocationCounter(connection, invocationCounter)) != 0 ||
            (ret = com_initializeConnection(connection)) != 0)
        //  ||            (ret = com_readAllObjects(connection, outputFile)) != 0)
        // Read all objects from the meter.
        {
            // Error code is returned at the end of the function.
        }
    }
    // Close connection.
    com_close(connection);
    con_close(connection);
    return ret;
}

/*Read DLMS meter using serial port connection.*/
int readSerialPort(
    connection *connection,
    const char *port,
    char *readObjects,
    const char *invocationCounter,
    const char *outputFile)
{
    int ret;
    ret = com_open(connection, port);
    if (ret == 0 && readObjects != NULL)
    {
        if ((ret = com_initializeOpticalHead(connection)) == 0 &&
            (ret = com_updateInvocationCounter(connection, invocationCounter)) == 0 &&
            (ret = com_initializeOpticalHead(connection)) == 0 &&
            (ret = com_initializeConnection(connection)) == 0 &&
            (ret = com_getAssociationView(connection, outputFile)) == 0)
        {
            int index;
            unsigned char buff[200];
            gxObject *obj = NULL;
            char *p2, *p = readObjects;
            do
            {
                printf(__LINE__);
                if (p != readObjects)
                {
                    ++p;
                }

                p2 = strchr(p, ':');
                *p2 = '\0';
                ++p2;
#if defined(_WIN32) || defined(_WIN64) // Windows
                sscanf_s(p2, "%d", &index);
#else
                sscanf(p2, "%d", &index);
#endif
                hlp_setLogicalName(buff, p);
                oa_findByLN(&connection->settings.objects, DLMS_OBJECT_TYPE_NONE, buff, &obj);
                if (obj == NULL)
                {
                    printf("Object '%s' not found from the association view.\n", p);
                    break;
                }
                // Capture objects are read first if the buffer of the profile generic is read.
                if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC && index == 2)
                {
                    if ((ret = com_readValue(connection, obj, 3)) != 0)
                    {
                        break;
                    }
                }
                ret = com_readValue(connection, obj, index);
            } while ((p = strchr(p2, ',')) != NULL);
        }
    }
    else if (ret == 0)
    {
        // Initialize connection.
        if ((ret = com_initializeOpticalHead(connection)) != 0 ||
            (ret = com_updateInvocationCounter(connection, invocationCounter)) != 0 ||
            (ret = com_initializeOpticalHead(connection)) != 0 ||
            (ret = com_initializeConnection(connection)) != 0)
        // (ret = com_readAllObjects(connection, outputFile)) != 0)
        // Read all objects from the meter.
        {
            // Error code is returned at the end of the function.
        }
    }
    // Close connection.
    //  com_close(connection);
    //  con_close(connection);
    return ret;
}

static void ShowHelp()
{
    printf("GuruxDlmsSample reads data from the DLMS/COSEM device.\n");
    printf("GuruxDlmsSample -h [Meter IP Address] -p [Meter Port No] -c 16 -s 1 -r SN\n");
    printf(" -h \t host name or IP address.\n");
    printf(" -p \t port number or name (Example: 1000).\n");
    printf(" -u \t UDP is used.");
    printf(" -S \t serial port.\n");
    printf(" -a \t Authentication (None, Low, High, HighMd5, HighSha1, HighGmac, HighSha256).\n");
    printf(" -P \t Password for authentication.\n");
    printf(" -c \t Client address. (Default: 16)\n");
    printf(" -s \t Server address. (Default: 1)\n");
    printf(" -n \t Server address as serial number.\n");
    printf(" -r \t [sn, sn]\t Short name or Logican Name (default) referencing is used.\n");
    printf(" -w \t WRAPPER profile is used. HDLC is default.\n");
    printf(" -t \t Trace messages.\n");
    printf(" -g \t Get selected object(s) with given attribute index. Ex -g \"0.0.1.0.0.255:1; 0.0.1.0.0.255:2\" \n");
    printf(" -C \t Security Level. (None, Authentication, Encrypted, AuthenticationEncryption)\n");
    printf(" -v \t Invocation counter data object Logical Name. Ex. 0.0.43.1.1.255\n");
    printf(" -I \t Auto increase invoke ID\n");
    printf(" -o \t Cache association view to make reading faster. Ex. -o C:\\device.bin");
    printf(" -T \t System title that is used with chiphering. Ex -T 4775727578313233\n");
    printf(" -A \t Authentication key that is used with chiphering. Ex -A D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF\n");
    printf(" -B \t Block cipher key that is used with chiphering. Ex -B 000102030405060708090A0B0C0D0E0F\n");
    printf(" -b \t Broadcast Block cipher key that is used with chiphering. Ex -b 000102030405060708090A0B0C0D0E0F");
    printf(" -D \t Dedicated key that is used with chiphering. Ex -D 00112233445566778899AABBCCDDEEFF\n");
    printf(" -i \t Used communication interface. Ex. -i WRAPPER.");
    printf(" -m \t Used PLC MAC address. Ex. -m 1.");
    printf(" -R \t Data is send as a broadcast (UnConfirmed, Confirmed).");
    printf("Example:\n");
    printf("Read LG device using TCP/IP connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -h [Meter IP Address] -p [Meter Port No]\n");
    printf("Read LG device using serial port connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -sp COM1 -i\n");
    printf("Read Indian device using serial port connection.\n");
    printf("GuruxDlmsSample -S COM1 -c 16 -s 1 -a Low -P [password]\n");
}

int readProfile(connection *connection)
{
    printf("readProfile()\n");
    char *data = NULL;
    int ret;
    gxProfileGeneric pg;
    char lns[] = "1.0.98.1.0.255";
    INIT_OBJECT(pg, DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);
    cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);

    com_read(connection, BASE(pg), 3);

    // com_readRowsByEntry(connection, &pg, 0, 0);
    gxtime startTime, endTime;
    // startTime.skip = DATETIME_SKIPS_STATUS;
    // endTime.skip = DATETIME_SKIPS_STATUS;
    startTime.skip = DATETIME_SKIPS_DEVITATION;
    endTime.skip = DATETIME_SKIPS_DEVITATION;

    // time_init(&startTime, 2024, 1, 1, 00, 00, 00, 0000, -420);
    // time_init(&endTime, 2024, 1, 2, 00, 00, 00, 0000, -420);

    time_now(&startTime);
    startTime.skip = DATETIME_SKIPS_DEVITATION;
    endTime.skip = DATETIME_SKIPS_DEVITATION;
    startTime.millisecond = 0;
    endTime = startTime;
    // time_clearTime(&startTime);
    ret = com_readRowsByRange2(connection, &pg, &startTime, &endTime);
    ret = com_readRowsByEntry(connection, &pg, 0, 0);
    obj_toString(BASE(pg), &data);

    printf("readProfile()\n");
    GXTRACE(("svr_preRead: "), data);
    // GXTRACE(PSTR("Load profile"), data);
}

int readLoadProfile(connection *connection)
{
    printf("readLoadProfile()\n");
    int ret;
    gxProfileGeneric pg;
    char lns[] = "1.0.99.1.0.255";
    INIT_OBJECT(pg, DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);
    cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);

    com_read(connection, BASE(pg), 3);
    // com_read(connection, BASE(pg), 2);
    com_readRowsByEntry(connection, &pg, 1, 2);
    // obj_toString(BASE(pg), &data);
    printf("readLoadProfile()\n");

    // GXTRACE(PSTR("Load profile"), data);
}

int com_readProfileGenericsss(connection *connection)
{
    gxtime startTime, endTime;
    int ret, pos;
    char str[50];
    char ln[25];
    char *data = NULL;
    gxByteBuffer ba;
    objectArray objects;
    // gxProfileGeneric *pg;
    gxProfileGeneric pg;
    char lns[] = "1.0.98.1.0.255";
    INIT_OBJECT(pg, DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);
    cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, lns);

    ret = com_read(connection, &pg, 7);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 7);
        // Do not clear objects list because it will free also objects from association view list.
        oa_empty(&objects);
        return ret;
    }
    // Read entries.
    ret = com_read(connection, &pg, 8);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
        // Do not clear objects list because it will free also objects from association view list.
        oa_empty(&objects);
        return ret;
    }
    printf("Entries: %ld/%ld\r\n", &pg.entriesInUse, &pg.profileEntries);
    // If there are no columns or rows.

    // Read first row from Profile Generic.
    // ret = com_readRowsByEntry(connection, &pg, 0, 1);
    ret = com_read(connection, &pg, 2);
    // Read last day from Profile Generic.
    // time_now(&startTime);
    // endTime = startTime;
    // time_clearTime(&startTime);
    // ret = com_readRowsByRange(connection, &pg, &startTime, &endTime);

    // Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

int connectMeter(int argc, char *argv[])
{
    connection con;
    gxByteBuffer item;
    bb_init(&item);
    con_init(&con, GX_TRACE_LEVEL_INFO);
    // Initialize settings using Logical Name referencing and HDLC.
    cl_init(&con.settings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
    int ret, opt = 0;
    int port = 0;
    char *address = NULL;
    char *serialPort = NULL;
    char *p, *readObjects = NULL, *outputFile = NULL;
    int index, a, b, c, d, e, f;
    char *invocationCounter = NULL;

    printf("%d\n", __LINE__);
    // SETTING
    char systemTitles[] = "4142434445464748";
    char authKey[] = "46464646464646464646464646464646";
    char blockKey[] = "46464646464646464646464646464646";
    int clintAddress = 4;
    int serverAddress = 35;
    char auth[] = "HighGmac";
    char serialPorts[] = "/dev/ttyUSB0";
    // SETTING

    // TRACE LEVEL
    con.trace = GX_TRACE_LEVEL_VERBOSE;
    // if (strcasecmp("Error", optarg) == 0)
    //     con.trace = GX_TRACE_LEVEL_ERROR;
    // else if (strcasecmp("Warning", optarg) == 0)
    //     con.trace = GX_TRACE_LEVEL_WARNING;
    // else if (strcasecmp("Info", optarg) == 0)
    //     con.trace = GX_TRACE_LEVEL_INFO;
    // else if (strcasecmp("Verbose", optarg) == 0)
    //     con.trace = GX_TRACE_LEVEL_VERBOSE;
    // else if (strcasecmp("Off", optarg) == 0)
    //     con.trace = GX_TRACE_LEVEL_OFF;
    // TRACE LEVEL

    // case 'P': // if using low level
    // bb_init(&con.settings.password);
    // bb_addString(&con.settings.password, optarg);

    // TYPE INTERFACE
    // case 'i': // if using hdlc of wrapper
    // Interface
    // if (strcasecmp("HDLC", optarg) == 0)
    con.settings.interfaceType = DLMS_INTERFACE_TYPE_HDLC;
    // else  if (strcasecmp("WRAPPER", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_WRAPPER;
    // else  if (strcasecmp("HdlcModeE", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E;
    // else  if (strcasecmp("Plc", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC;
    // else if (strcasecmp("PlcHdlc", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC_HDLC;
    // else if (strcasecmp("PlcPrime", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC_PRIME;
    // else if (strcasecmp("Pdu", optarg) == 0)
    //     con.settings.interfaceType = DLMS_INTERFACE_TYPE_PDU;
    // TYPE INTERFACE

    // TYPE SECURITY
    // case 'C':
    // if (strcasecmp("None", optarg) == 0)
    // {
    //     con.settings.cipher.security = DLMS_SECURITY_NONE;
    // }
    // else if (strcasecmp("Authentication", optarg) == 0)
    // {
    //     con.settings.cipher.security = DLMS_SECURITY_AUTHENTICATION;
    // }
    // else if (strcasecmp("Encryption", optarg) == 0)
    // {
    //     con.settings.cipher.security = DLMS_SECURITY_ENCRYPTION;
    // }
    // else if (strcasecmp("AuthenticationEncryption", optarg) == 0)
    // {
    //     con.settings.cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
    // }
    con.settings.cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
    // TYPE SECURITY

    // INVOCATION COUNTER LN
    // case 'F':
    con.settings.cipher.invocationCounter = "0.0.40.0.255";
    // INVOCATION COUNTER LN

    // SYSTEM TITLE
    bb_clear(&con.settings.cipher.systemTitle);
    bb_addHexString(&con.settings.cipher.systemTitle, systemTitles);
    // SYSTEM TITLE

    // PRE-SYSTEM TITLE
    // case 'M':
    // if (con.settings.preEstablishedSystemTitle == NULL)
    // {
    //     con.settings.preEstablishedSystemTitle = malloc(sizeof(gxByteBuffer));
    //     bb_init(con.settings.preEstablishedSystemTitle);
    // }
    // bb_clear(con.settings.preEstablishedSystemTitle);
    // bb_addHexString(con.settings.preEstablishedSystemTitle, optarg);
    // PRE-SYSTEM TITLE

    // AUTH KEY
    // case 'A':
    bb_clear(&con.settings.cipher.authenticationKey);
    bb_addHexString(&con.settings.cipher.authenticationKey, authKey);
    // AUTH KEY

    // BLOCK KEY
    // case 'B':
    bb_clear(&con.settings.cipher.blockCipherKey);
    bb_addHexString(&con.settings.cipher.blockCipherKey, blockKey);
    // BLOCK KEY

    // BROADCAST BLOCK KEY
    // case 'b':
    // bb_clear(&con.settings.cipher.broadcastBlockCipherKey);
    // bb_addHexString(&con.settings.cipher.broadcastBlockCipherKey, optarg);
    // BROADCAST BLOCK KEY

    // DEDICATED KEY
    // con.settings.cipher.dedicatedKey = (gxByteBuffer *)malloc(sizeof(gxByteBuffer));
    // bb_init(con.settings.cipher.dedicatedKey);
    // bb_addHexString(con.settings.cipher.dedicatedKey, optarg);
    // DEDICATED KEY

    // SERIAL PORT
    serialPort = serialPorts;
    // SERIAL PORT

    // TIPE AUTENTIKASI
    if (strcasecmp("None", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_NONE;
    }
    else if (strcasecmp("Low", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_LOW;
    }
    else if (strcasecmp("High", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_HIGH;
    }
    else if (strcasecmp("HighMd5", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_HIGH_MD5;
    }
    else if (strcasecmp("HighSha1", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_HIGH_SHA1;
    }
    else if (strcasecmp("HighGmac", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_HIGH_GMAC;
    }
    else if (strcasecmp("HighSha256", auth) == 0)
    {
        con.settings.authentication = DLMS_AUTHENTICATION_HIGH_SHA256;
    }

    // CLIENT ADDRESS
    con.settings.clientAddress = clintAddress;
    // CLIENT ADDRESS

    // SERVER ADDRESS
    con.settings.serverAddress = 163;
    // SERVER ADDRESS

    // CUSTOM SERVER ADDRESS
    // con.settings.serverAddress = cl_getServerAddress(1, (unsigned short)con.settings.serverAddress, 2);
    printf("auth: %d\n", con.settings.authentication);
    printf("clientAddress: %d\n", con.settings.clientAddress);
    printf("serverAddress: %d\n", con.settings.serverAddress);
    printf("systemTitle: %c\n", con.settings.cipher.systemTitle);
    printf("authKey: %c\n", con.settings.cipher.authenticationKey);
    printf("blockKey: %c\n", con.settings.cipher.blockCipherKey);
    // CUSTOM SERVER ADDRESS

    // IF TCP
    // address = optarg;
    // port = atoi(optarg);
    // ret = readTcpIpConnection(&con, address, port, readObjects, invocationCounter, outputFile);
    // IF TCP

    // IF SERIAL
    ret = readSerialPort(&con, serialPort, readObjects, invocationCounter, outputFile);
    // IF SERIAL

    // readLoadProfile(&con);
    readProfile(&con);

    com_close(&con);
    con_close(&con);
    cl_clear(&con.settings);
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = connectMeter(argc, argv);
    if (ret != 0)
    {
        ShowHelp();
    }
#if defined(_WIN32) || defined(_WIN64) // Windows
    WSACleanup();
#if _MSC_VER > 1000
    // Show memory leaks.
    _CrtDumpMemoryLeaks();
#endif //_MSC_VER
#endif
    return ret;
}
