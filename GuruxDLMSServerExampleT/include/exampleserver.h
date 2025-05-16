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

#pragma once

#if defined(_WIN32) || defined(_WIN64) // Windows includes
#include <Winsock2.h>                  //Add support for sockets
#endif

#include "../../development/include/server.h"
#define HDLC_HEADER_SIZE 17
#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 512
#define WRAPPER_BUFFER_SIZE 8 + PDU_BUFFER_SIZE

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include "connection.h"

void println(char *desc, gxByteBuffer *data);

int svr_start(
    connection *con,
    unsigned short port);

#endif // defined(_WIN32) | defined(_WIN64) || defined(__linux__)

int svr_InitObjects(
    dlmsServerSettings *settings);

/**
 * Check is data sent to this server.
 *
 * @param serverAddress
 *            Server address.
 * @param clientAddress
 *            Client address.
 * @return True, if data is sent to this server.
 */
unsigned char svr_isTarget(
    dlmsSettings *settings,
    uint32_t serverAddress,
    uint32_t clientAddress);

/**
 * Get attribute access level.
 */
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index);

/**
 * Get method access level.
 */
extern DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index);

/**
 * called when client makes connection to the server.
 */
int svr_connected(
    dlmsServerSettings *settings);

/**
 * Client has try to made invalid connection. Password is incorrect.
 *
 * @param connectionInfo
 *            Connection information.
 */
int svr_invalidConnection(dlmsServerSettings *settings);

/**
 * called when client clses connection to the server.
 */
int svr_disconnected(
    dlmsServerSettings *settings);

/**
 * Read selected item(s).
 *
 * @param args
 *            Handled read requests.
 */
void svr_preRead(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Write selected item(s).
 *
 * @param args
 *            Handled write requests.
 */
void svr_preWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Action is occurred.
 *
 * @param args
 *            Handled action requests.
 */
void svr_preAction(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Read selected item(s).
 *
 * @param args
 *            Handled read requests.
 */
void svr_postRead(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Write selected item(s).
 *
 * @param args
 *            Handled write requests.
 */
void svr_postWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Action is occurred.
 *
 * @param args
 *            Handled action requests.
 */
void svr_postAction(
    dlmsSettings *settings,
    gxValueEventCollection *args);

/**
 * Check whether the authentication and password are correct.
 *
 * @param authentication
 *            Authentication level.
 * @param password
 *            Password.
 * @return Source diagnostic.
 */
DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings *settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer *password);

/**
 * Find object.
 *
 * @param objectType
 *            Object type.
 * @param sn
 *            Short Name. In Logical name referencing this is not used.
 * @param ln
 *            Logical Name. In Short Name referencing this is not used.
 * @return Found object or NULL if object is not found.
 */
int svr_findObject(
    dlmsSettings *settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char *ln,
    gxValueEventArg *e);

void svr_preGet(dlmsSettings *settings,
                gxValueEventCollection *args);

void svr_postGet(dlmsSettings *settings,
                 gxValueEventCollection *args);

/**
 * This is reserved for future use.
 *
 * @param args
 *            Handled data type requests.
 */
void svr_getDataType(
    dlmsSettings *settings,
    gxValueEventCollection *args);

int captureProfileGeneric(dlmsSettings *settings,
                          gxProfileGeneric *pg);

void Tick_blockProfile(dlmsSettings *settings);

void UpdateLoadProfileData(dlmsSettings *settings, char *cosem, int value);

void handleInstantProfileFromMeter(int id_meter, char *payload);
void updateInstantValue(char *input, int value);
// void updateInstantValueStruct(char *input, int value, struct instant_value_profile *instant_meter);
void publishToMQTT(MQTTAsync handle, char *meterTopic, char *topics, double value);
void summaryInstantValueFromMeter(MQTTAsync handle);