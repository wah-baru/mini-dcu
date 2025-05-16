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
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>
#include "MQTTAsync.h"

// #define DLMS_IGNORE_MALLOC

#if !defined(_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#if defined(_WRS_KERNEL)
#include <OsWrapper.h>
#endif

#define USEMQTT
#if defined(USEMQTT)
#define ADDRESS "tcp://127.0.0.1"
#define CLIENTID "ExampleClientSub"
#define USERN "mgi"
#define PASSW "admin@mgi"
#define TOPIC_METER1 "1/#"
#define topic_instant_profile_meter1 "1/data/instant_profile"
#define TOPIC_METER2 "2/#"
#define topic_instant_profile_meter2 "2/data/instant_profile"
#define TOPIC_METER3 "3/#"
#define topic_instant_profile_meter3 "3/data/instant_profile"
#define TOPIC_METER4 "4/#"
#define topic_instant_profile_meter4 "4/data/instant_profile"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L
#endif

#if defined(_WIN32) || defined(_WIN64) // Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/getopt.h"
#include <tchar.h>
#include <conio.h>
#include <Winsock2.h> //Add support for sockets
#include <time.h>
#include <process.h> //Add support for threads
#else                // Linux includes.
#define closesocket close
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#endif

#include "../include/exampleserver.h"
#include "../include/connection.h"
#include "../../development/include/cosem.h"
#include "../../development/include/gxaes.h"

// SERIAL
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()
// SERIAL

unsigned char snframeBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
unsigned char snpduBuff[PDU_BUFFER_SIZE];
unsigned char lnframeBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
unsigned char lnpduBuff[PDU_BUFFER_SIZE];
unsigned char sn47frameBuff[WRAPPER_BUFFER_SIZE];
unsigned char sn47pduBuff[PDU_BUFFER_SIZE];
unsigned char ln47frameBuff[WRAPPER_BUFFER_SIZE];
unsigned char ln47pduBuff[PDU_BUFFER_SIZE];

char DATAFILE[FILENAME_MAX];
char IMAGEFILE[FILENAME_MAX];
char TRACEFILE[FILENAME_MAX];

// MQTT
int disc_finished = 0;
int subscribed = 0;
int finished = 0;

void onConnect(void *context, MQTTAsync_successData *response);
void onConnectFailure(void *context, MQTTAsync_failureData *response);
void connlost(void *context, char *cause);

#if defined(USEMQTT)
void connlost(void *context, char *cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    printf("\nConnection lost\n");
    if (cause)
        printf("     cause: %s\n", cause);

    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char *)message->payload);

    if (strcmp(topicName, topic_instant_profile_meter1) == 0)
    {
        handleInstantProfileFromMeter(1, (char *)message->payload);
        printf("handleInstantProfileFromMeter 1\n");
    }
    else if (strcmp(topicName, topic_instant_profile_meter2) == 0)
    {
        handleInstantProfileFromMeter(2, (char *)message->payload);
        printf("handleInstantProfileFromMeter 2\n");
    }
    else if (strcmp(topicName, topic_instant_profile_meter3) == 0)
    {
        handleInstantProfileFromMeter(3, (char *)message->payload);
        printf("handleInstantProfileFromMeter 3\n");
    }
    else if (strcmp(topicName, topic_instant_profile_meter4) == 0)
    {
        handleInstantProfileFromMeter(4, (char *)message->payload);
        printf("handleInstantProfileFromMeter 4\n");
    }

    ////////////////////// JSON PARSE /////////////////////////
    // char *payload = message->payload;
    // json_object *root, *temp;

    // // Parse JSON
    // struct json_object *parsed_json;
    // struct json_object *array_obj;
    // struct json_object *obis, *type, *value, *scaler, *unit;

    // // Parse JSON string
    // parsed_json = json_tokener_parse((char *)message->payload);
    // if (!parsed_json)
    // {
    //     printf("Error parsing JSON data!\n");
    //     return 1;
    // }

    // int array_length = json_object_array_length(parsed_json);
    // printf("Total Records: %d\n", array_length);

    // // Loop through JSON array
    // char buf[20];
    // float bufVal;
    // for (int i = 0; i < array_length; i++)
    // {
    //     array_obj = json_object_array_get_idx(parsed_json, i);

    //     // Extract fields
    //     if (json_object_object_get_ex(array_obj, "obis", &obis))
    //         printf("OBIS: %s\n", json_object_get_string(obis));

    //     if (json_object_object_get_ex(array_obj, "type", &type))
    //         printf("Type: %s\n", json_object_get_string(type));

    //     if (json_object_object_get_ex(array_obj, "value", &value))
    //     {

    //         if (json_object_is_type(value, json_type_string))
    //         {
    //             printf("Value: %s\n", json_object_get_string(value));
    //             strcpy(buf, json_object_get_string(value));
    //         }
    //         else
    //         {
    //             printf("Value: %lf\n", json_object_get_double(value));
    //             bufVal = json_object_get_double(value);
    //         }
    //     }

    //     if (json_object_object_get_ex(array_obj, "scaler", &scaler))
    //         printf("Scaler: %lf\n", json_object_get_double(scaler));

    //     if (json_object_object_get_ex(array_obj, "unit", &unit))
    //     {
    //         if (json_object_is_type(unit, json_type_string))
    //         {
    //             printf("Unit: %s\n", json_object_get_string(unit));
    //         }
    //         else
    //         {
    //             printf("Unit: %d\n", json_object_get_int(unit)); // Some unit values are integers
    //         }
    //     }

    //     printf("------------------------------------\n");
    //     UpdateLoadProfileData(&lnHdlc.settings, buf, bufVal);
    // }
    // // Free JSON object memory
    // json_object_put(parsed_json);

    ////////////////////// JSON PARSE /////////////////////////

    // UpdateLoadProfileData(&lnHdlc.settings, buf, bufVal);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void onDisconnectFailure(void *context, MQTTAsync_failureData *response)
{
    printf("Disconnect failed, rc %d\n", response->code);
    disc_finished = 1;
}

void onDisconnect(void *context, MQTTAsync_successData *response)
{
    printf("Successful disconnection\n");
    disc_finished = 1;
}

void onSubscribe(void *context, MQTTAsync_successData *response)
{
    printf("Subscribe succeeded\n");
    subscribed = 1;
}

void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    printf("Subscribe failed, rc %d\n", response->code);
    finished = 1;
}

void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    printf("Connect failed, rc %d\n", response->code);
    finished = 1;
}

void onConnect(void *context, MQTTAsync_successData *response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Successful connection\n");

    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    if ((rc = MQTTAsync_subscribe(client, TOPIC_METER1, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
    if ((rc = MQTTAsync_subscribe(client, TOPIC_METER2, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
    if ((rc = MQTTAsync_subscribe(client, TOPIC_METER3, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
    if ((rc = MQTTAsync_subscribe(client, TOPIC_METER4, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
}
MQTTAsync client;
// MQTT
// MQTT
#endif
// MQTT
connection snHdlc, lnHdlc, snWrapper, lnWrapper, lniec;
int startServers(int port, int trace)
{
    int ret;

    // Initialize DLMS settings.
    svr_init(&snHdlc.settings, 0, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, snframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, snpduBuff, PDU_BUFFER_SIZE);
    // Set master key (KEK) to 1111111111111111.
    unsigned char KEK[16] = {0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};
    BB_ATTACH(snHdlc.settings.base.kek, KEK, sizeof(KEK));
    svr_InitObjects(&snHdlc.settings);
    // Start server
    if ((ret = svr_start(&snHdlc, port)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server in port %d.\n", port);
    printf("Example connection settings:\n");
    printf("Gurux.DLMS.Client.Example.Net -r sn -h localhost -p %d\n", port);
    printf("----------------------------------------------------------\n");
    // Initialize DLMS settings.
    svr_init(&lnHdlc.settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, lnframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
    // We have several server that are using same objects. Just copy them.
    oa_copy(&lnHdlc.settings.base.objects, &snHdlc.settings.base.objects);
    // Start server
    if ((ret = svr_start(&lnHdlc, port + 2)) != 0)
    {
        return ret;
    }
    printf("Logical Name DLMS Server in port %d.\n", port + 2);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port + 2);
    printf("----------------------------------------------------------\n");
    // Initialize DLMS settings.
    svr_init(&snWrapper.settings, 0, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, sn47frameBuff, WRAPPER_BUFFER_SIZE, sn47pduBuff, PDU_BUFFER_SIZE);
    // We have several server that are using same objects. Just copy them.
    oa_copy(&snWrapper.settings.base.objects, &snHdlc.settings.base.objects);
    // Start server
    if ((ret = svr_start(&snWrapper, port + 3)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server with IEC 62056-47 in port %d.\n", port + 3);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -r sn -h localhost -p %d -i WRAPPER\n", port + 3);
    printf("----------------------------------------------------------\n");
    // Initialize DLMS settings.
    svr_init(&lnWrapper.settings, 1, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, ln47frameBuff, WRAPPER_BUFFER_SIZE, ln47pduBuff, PDU_BUFFER_SIZE);
    // We have several server that are using same objects. Just copy them.
    oa_copy(&lnWrapper.settings.base.objects, &snHdlc.settings.base.objects);
    // Start server
    if ((ret = svr_start(&lnWrapper, port + 4)) != 0)
    {
        return ret;
    }
    snHdlc.trace = lnHdlc.trace = snWrapper.trace = lnWrapper.trace = trace;
    printf("Logical Name DLMS Server with IEC 62056-47 in port %d.\n", port + 4);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -i WRAPPER\n", port + 4);

    // Initialize DLMS settings.
    svr_init(&lniec.settings, 1, DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, lnframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
    // Set flag id.
    memcpy(lniec.settings.flagId, "MEL", 3);
    // We have several server that are using same objects. Just copy them.
    oa_copy(&lniec.settings.base.objects, &snHdlc.settings.base.objects);
    // Start server
    if ((ret = svr_start(&lniec, port + 5)) != 0)
    {
        return ret;
    }
    printf("Hdlc With Mode E Logical Name DLMS Server in port %d.\n", port + 5);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -i HdlcWithModeE\n", port + 5);
    printf("----------------------------------------------------------\n");

    printf("----------------------------------------------------------\n");
    printf("Authentication levels:\n");
    printf("None: Client address 16 (0x10)\n");
    printf("Low: Client address 17 (0x11)\n");
    printf("High GMac: Client address 1 (0x1)\n");
    printf("High Pre-established: Client address 2 (0x2)\n");
    printf("High ECDSA: Client address 3 (0x3)\n");
    printf("----------------------------------------------------------\n");
    println("System Title", &snHdlc.settings.base.cipher.systemTitle);
    println("Authentication key", &snHdlc.settings.base.cipher.authenticationKey);
    println("Block cipher key", &snHdlc.settings.base.cipher.blockCipherKey);
    println("Client System title", snHdlc.settings.base.preEstablishedSystemTitle);
    println("Master key (KEK)", &snHdlc.settings.base.kek);
    printf("----------------------------------------------------------\n");
    printf("Press Enter to close application.\r\n");
    uint32_t lastMonitor = 0;
    uint32_t executeTime = 0;
    while (1)
    {
        // Monitor values only once/second.
        // Tick_blockProfile(&lnHdlc.settings);
        summaryInstantValueFromMeter(client);
        if (time_current() - lastMonitor > 1)
        {
            lastMonitor = time_current();
            // if ((ret = svr_limiterAll(&snHdlc.settings, lastMonitor)) != 0)
            // {
            //     printf("snHdlc limiter failed.\r\n");
            // }
            // if ((ret = svr_limiterAll(&lnHdlc.settings, lastMonitor)) != 0)
            // {
            //     printf("lnHdlc limiter failed.\r\n");
            // }
            // if ((ret = svr_limiterAll(&snWrapper.settings, lastMonitor)) != 0)
            // {
            //     printf("snWrapper limiter failed.\r\n");
            // }
            // if ((ret = svr_limiterAll(&lnWrapper.settings, lastMonitor)) != 0)
            // {
            //     printf("lnWrapper limiter failed.\r\n");
            // }
            // if ((ret = svr_monitorAll(&snHdlc.settings)) != 0)
            // {
            //     printf("snHdlc monitor failed.\r\n");
            // }
            // if ((ret = svr_monitorAll(&lnHdlc.settings)) != 0)
            // {
            //     printf("lnHdlc monitor failed.\r\n");
            // }
            // if ((ret = svr_monitorAll(&snWrapper.settings)) != 0)
            // {
            //     printf("snWrapper monitor failed.\r\n");
            // }
            // if ((ret = svr_monitorAll(&lnWrapper.settings)) != 0)
            // {
            //     printf("lnWrapper monitor failed.\r\n");
            // }
            if (executeTime <= lastMonitor)
            {
                svr_run(&lnHdlc.settings, lastMonitor, &executeTime);
                if (executeTime != -1)
                {
                    time_t tmp = lastMonitor;
                    printf("%s", ctime(&tmp));
                    tmp = executeTime;
                    printf("%lu seconds before next invoke %s", executeTime - lastMonitor, ctime(&tmp));
                }
            }
        }
        usleep(1000);
    }
    con_close(&snHdlc);
    con_close(&snWrapper);
    con_close(&lnWrapper);
    return 0;
}

void showHelp()
{
    printf("Gurux DLMS example Server implements four DLMS/COSEM devices.\r\n");
    printf(" -t [Error, Warning, Info, Verbose] Trace messages.\r\n");
    printf(" -p Start port number. Default is 4060.\r\n");
}

///////////////// SERIAL //////////////////
int set_interface_attribs(int fd, int speed, int parity)
{
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("error from tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0;        // no signaling chars, no echo,
                            // no canonical processing
    tty.c_oflag = 0;        // no remapping, no delays
    tty.c_cc[VMIN] = 0;     // read doesn't block
    tty.c_cc[VTIME] = 5;    // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    // tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("error from tcsetattr");
        return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("error from tggetattr");
        return;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        perror("error setting term attributes");
}
///////////////// SERIAL //////////////////

#if defined(_WIN32) || defined(_WIN64) // Windows includes
int _tmain(int argc, _TCHAR *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    ////////////// SERIAL /////////////////
    // char *portname = "/dev/serial0";
    // int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    // if (fd < 0)
    // {
    //     printf("error %d opening %s: %s", errno, portname, strerror(errno));
    //     return;
    // }
    // set_interface_attribs(fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
    // set_blocking(fd, 0);
    // write(fd, "n0.val=99\n", 11);
    // write(fd, 0xff, 2);
    // write(fd, 0xff, 2);
    // write(fd, 0xff, 2);
    // int c_fd = close(fd);
    ////////////// SERIAL ////////////////

#if defined(USEMQTT)
    // MQTT
    // MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.username = USERN;
    conn_opts.password = PASSW;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    int rc;
    int ch;

    const char *uri = (argc > 1) ? argv[1] : ADDRESS;
    printf("Using server at %s\n", uri);

    if ((rc = MQTTAsync_create(&client, uri, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    else
    {
        printf("MQTT Connected to Client\n");
    }

    // publishToMQTT(client, "TeST", 12);
    // MQTT
#endif

    int opt, port = 4060;
    int trace = 0;
#if defined(_WIN32) || defined(_WIN64) // Windows includes
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif

    strcpy(DATAFILE, argv[0]);
#if defined(_WIN32) || defined(_WIN64) // Windows includes
    char *p = strrchr(DATAFILE, '\\');
    *p = '\0';
    strcpy(IMAGEFILE, DATAFILE);
    strcpy(TRACEFILE, DATAFILE);
    // Add empty file name. This is removed when data is updated.
    strcat(IMAGEFILE, "\\empty.bin");
    strcat(DATAFILE, "\\data.csv");
    strcat(TRACEFILE, "\\trace.txt");
#else
    char *p = strrchr(DATAFILE, '/');
    *p = '\0';
    strcpy(IMAGEFILE, DATAFILE);
    strcpy(TRACEFILE, DATAFILE);
    // Add empty file name.
    strcat(IMAGEFILE, "/empty.bin");
    strcat(DATAFILE, "/data.csv");
    strcat(TRACEFILE, "/trace.txt");
#endif
    // Clear trace file
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE *f = NULL;
    fopen_s(&f, TRACEFILE, "w");
#else
    FILE *f = fopen(TRACEFILE, "w");
#endif
    fclose(f);
#endif // defined(_WIN32) || defined(_WIN64) || defined(__linux__)

    while ((opt = getopt(argc, argv, "t:p:")) != -1)
    {
        switch (opt)
        {
        case 't':
            // Trace.
            if (strcmp("Error", optarg) == 0)
                trace = GX_TRACE_LEVEL_ERROR;
            else if (strcmp("Warning", optarg) == 0)
                trace = GX_TRACE_LEVEL_WARNING;
            else if (strcmp("Info", optarg) == 0)
                trace = GX_TRACE_LEVEL_INFO;
            else if (strcmp("Verbose", optarg) == 0)
                trace = GX_TRACE_LEVEL_VERBOSE;
            else if (strcmp("Off", optarg) == 0)
                trace = GX_TRACE_LEVEL_OFF;
            else
            {
                printf("Invalid trace option '%s'. (Error, Warning, Info, Verbose, Off)", optarg);
                return 1;
            }
            break;
        case 'p':
            // Port.
            port = atoi(optarg);
            break;
        case '?':
        {
            if (optarg[0] == 'p')
            {
                printf("Missing mandatory port option.\n");
            }
            else if (optarg[0] == 't')
            {
                printf("Missing mandatory trace option.\n");
            }
            else
            {
                showHelp();
                return 1;
            }
        }
        break;
        default:
            showHelp();
            return 1;
        }
    }

    // MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    // MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;

    // int64_t t = 1413431;
    // char buf[256];
    // int n = snprintf(buf, sizeof(buf), "%lld", (long long)t);
    // printf("%s\n", buf);
    // pubmsg.payload = buf;
    // pubmsg.payloadlen = n;
    // pubmsg.qos = QOS;
    // pubmsg.retained = 0;
    // if ((rc = MQTTAsync_sendMessage(client, "TEST", &pubmsg, &pub_opts)) != MQTTASYNC_SUCCESS)
    // {
    //     printf("Failed to start sendMessage, return code %d\n", rc);
    //     exit(EXIT_FAILURE);
    // }

    startServers(port, trace);

#if defined(USEMQTT)
destroy_exit:
    if (client == NULL) {
        fprintf(stderr, "MQTTClient is NULL\n");
        exit(1);  // Or handle the error appropriately
    }
    MQTTAsync_destroy(&client);
#endif

#if defined(_WIN32) || defined(_WIN64) // Windows

    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif
exit:
    return 0;
}
