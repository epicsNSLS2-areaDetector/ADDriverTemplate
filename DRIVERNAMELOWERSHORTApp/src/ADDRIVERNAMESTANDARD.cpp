/**
 * Main source file for the ADDRIVERNAMESTANDARD EPICS driver 
 * 
 * This file contains functions for connecting and disconnectiong from the camera,
 * for starting and stopping image acquisition, and for controlling all camera functions through
 * EPICS.
 * 
 * Author: 
 * Created On: 
 * 
 * Copyright (c) : 
 * 
 */

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// EPICS includes
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsExit.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <iocsh.h>
#include <epicsExport.h>


// Area Detector include
#include "ADDRIVERNAMESTANDARD.h"


using namespace std;

// Add any additional namespaces here


const char* driverName = "ADDRIVERNAMESTANDARD";

// Add any driver constants here


// -----------------------------------------------------------------------
// ADDRIVERNAMESTANDARD Utility Functions (Reporting/Logging/ExternalC)
// -----------------------------------------------------------------------


/*
 * External configuration function for ADDRIVERNAMESTANDARD.
 * Envokes the constructor to create a new ADDRIVERNAMESTANDARD object
 * This is the function that initializes the driver, and is called in the IOC startup script
 * 
 * NOTE: When implementing a new driver with ADDriverTemplate, your camera may use a different connection method than
 * a const char* serialNumber. Just edit the param to fit your device, and make sure to make the same edit to the constructor below
 *
 * @params[in]: all passed into constructor
 * @return:     status
 */
extern "C" int ADDRIVERNAMESTANDARDConfig(const char* portName, const char* serialNumber, int maxBuffers, size_t maxMemory, int priority, int stackSize){
    new ADDRIVERNAMESTANDARD(portName, serialNumber, maxBuffers, maxMemory, priority, stackSize);
    return(asynSuccess);
}


/*
 * Callback function called when IOC is terminated.
 * Deletes created object
 *
 * @params[in]: pPvt -> pointer to the ADDRIVERNAMESTANDATD object created in ADDRIVERNAMESTANDARDConfig
 * @return:     void
 */
static void exitCallbackC(void* pPvt){
    ADDRIVERNAMESTANDARD* pDRIVERNAMEUPPER = (ADDRIVERNAMESTANDARD*) pPvt;
    delete(pDRIVERNAMEUPPER);
}


/**
 * Simple function that prints all information about a connected camera
 * 
 * @return: void
 */
void ADDRIVERNAMESTANDARD::printConnectedDeviceInfo(){
    printf("--------------------------------------\n");
    printf("Connected to DRIVERNAMESTANDARD device\n");
    printf("--------------------------------------\n");
    // Add any information you wish to print about the device here
    printf("--------------------------------------\n");
}


// -----------------------------------------------------------------------
// ADDRIVERNAMESTANDARD Connect/Disconnect Functions
// -----------------------------------------------------------------------


/**
 * Function that is used to initialize and connect to the device.
 * 
 * NOTE: Again, it is possible that for your camera, a different connection type is used (such as a product ID [int])
 * Make sure you use the same connection type as passed in the ADDRIVERNAMESTANDARDConfig function and in the constructor.
 * 
 * @params[in]: serialNumber    -> serial number of camera to connect to. Passed through IOC shell
 * @return:     status          -> success if connected, error if not connected
 */
asynStatus ADDRIVERNAMESTANDARD::connectToDevice(const char* serialNumber){
    const char* functionName = "connectToDevice";
    bool connected = false;


    // Implement connecting to the camera here
    // Usually the vendor provides examples of how to do this with the library/SDK


    if(connected) return asynSuccess;
    else{
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error: failed to connect to device\n", driverName, functionName);
        return asynError;
    }
}


/**
 * Function that disconnects from any connected device
 * First checks if is connected, then if it is, it frees the memory
 * for the info and the camera
 * 
 * @return: status  -> success if freed, error if never connected
 */
asynStatus ADDRIVERNAMESTANDARD::disconnectFromDevice(){
    const char* functionName = "disconnectFromDevice";

    // Free up any data allocated by driver here, and call the vendor libary to disconnect

    return asynSuccess;
}


/**
 * Function that updates PV values with camera information
 * 
 * @return: status
 */
asynStatus ADDRIVERNAMESTANDARD::collectCameraInformation(){
    const char* functionName = "collectCameraInformation";
    asynStatus status = asynSuccess;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s::%s Collecting camera information\n", driverName, functionName);

    // Use the vendor library to collect information about the connected camera here, and set the appropriate PVs
    // Make sure you check if camera is connected before calling on it for information

    //setStringParam(ADManufacturer,        _____________);
    //setStringParam(ADSerialNumber,        _____________);
    //setStringParam(ADFirmwareVersion,     _____________);
    //setStringParam(ADModel,               _____________);

    return status;
}



// -----------------------------------------------------------------------
// ADDRIVERNAMESTANDARD Acquisition Functions
// -----------------------------------------------------------------------


/*
#####################################################################################################################
#
# The next two functions can be used when a seperate image acquisition thread is required by the driver. 
# Some vendor software already creates its own acquisition thread for asynchronous use, but if not this
# must be used. By default, the acquireStart() function is written to not use these. If they are needed, 
# find the call to DRIVERNAMELOWERSHORTCallback in acquireStart(), and change it to startImageAcquisitionThread
#
#####################################################################################################################
*/



/**
 * Function that initializes the image acquisition thread for the camera
 * calls pthread_create to create the thread and sets the threadActive flag to true 
 *
 * return: status
 */
asynStatus ADDRIVERNAMESTANDARD::startImageAcquisitionThread(){
    const char* functionName = "startImageAcquisitionThread";
    asynStatus status;
    if(this->imageCollectionThreadActive == 0){
        if(pthread_create(&this->imageCollectionThread, NULL, DRIVERNAMELOWERSHORTCallbackWrapper, this)){
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error creating Image Acquisition thread\n", driverName, functionName);
            status = asynError;
        }
        else{
            this->imageCollectionThreadActive = 1;
            asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s Image Thread Created\n", driverName, functionName);
            int detached = pthread_detach(this->imageCollectionThread);
            if(detached) status = asynSuccess;
            else{
                asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unable to detach image acquisition thread\n", driverName, functionName);
                status = asynError;
            }
        }
    }
    else{
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Acquisition thread already active\n", driverName, functionName);
        status = asynError;
    }
    return status;
}


/**
 * Function that stops the image acquisition thread
 * Sets flag for active to false
 */
asynStatus ADDRIVERNAMESTANDARD::stopImageAcquisitionThread(){
    const char* functionName = "stopImageAcquisitionThread";
    asynStatus status;
    if(this->imageCollectionThreadActive == 0){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Image thread not active\n", driverName, functionName);
        status = asynError;
    }
    else{
        imageCollectionThreadActive = 0;
        asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s Stopping image acquisition thread\n", driverName, functionName);
        /*
        if(pthread_join(this->imageCollectionThread, NULL)){
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error joining image collection thread\n", driverName, functionName);
            status = asynError;
        }
        else{
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Joined Image Acquisition thread\n", driverName, functionName);
            status = asynSuccess;
        }
        */
    }
    return status;
}



/**
 * Function responsible for starting camera image acqusition. First, check if there is a
 * camera connected. Then, set camera values by reading from PVs. Then, we execute the 
 * Acquire Start command. if this command was successful, image acquisition started.
 * 
 * @return: status  -> error if no device, camera values not set, or execute command fails. Otherwise, success
 */
asynStatus ADDRIVERNAMESTANDARD::acquireStart(){
    const char* functionName = "acquireStart";
    asynStatus status;
    if(this->pcamera == NULL){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error: No camera connected\n", driverName, functionName);
        status = asynError;
    }
    else{

        // Here, either call the vendor acquisition function and pass the ADDRIVERNAMELOWERSHORTCallback as the callback function, or
        // call startImageAcquisitionThread()
        status = startImageAcquisitionThread();
        if(status != asynSuccess){
            setIntegerParam(ADAcquire, 0);
            setIntegerParam(ADStatus, ADStatusIdle);
            callParamCallbacks();
            status = asynError;
        }
        else{
            setIntegerParam(ADStatus, ADStatusAcquire);
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s::%s Image acquistion start\n", driverName, functionName);
            callParamCallbacks();
        }

    }
    return status;
}




/**
 * Function responsible for stopping camera image acquisition. First check if the camera is connected.
 * If it is, execute the 'AcquireStop' command. Then set the appropriate PV values, and callParamCallbacks
 * 
 * @return: status  -> error if no camera or command fails to execute, success otherwise
 */ 
asynStatus ADDRIVERNAMESTANDARD::acquireStop(){
    const char* functionName = "acquireStop";
    asynStatus status;
    if(this->pcamera == NULL){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error: No camera connected\n", driverName, functionName);
        status = asynError;
    }
    else{
        // Here, if not using acquisition thread, make sure to call any necessary stop collection functions
        status = stopImageAcquisitionThread();
    }
    setIntegerParam(ADStatus, ADStatusIdle);
    setIntegerParam(ADAcquire, 0);
    callParamCallbacks();
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s::%s Stopping Image Acquisition\n", driverName, functionName);
    return status;
}



/**
 * Function that is used to convert the Area Detector input data type and color mode into whatever format the camera
 * requires. To begin, you must find what data type the formats for the camera are stored in. This is often some form of
 * int, as an enum. Then, calculate which format equates to which data type and color mode combination.
 * 
 * @params[out]: DRIVERNAMELOWERSHORTDataType   -> pointer to an instance of the DRIVERNAMESTANDARD data type
 * @params[in]: dataType    -> NDArray Data type
 * @params[in]: colorMode   -> NDArray Color mode
 * @return: status
 */
asynStatus ADDRIVERNAMESTANDARD::getDRIVERNAMESTANDARDFrameFormat(____* DRIVERNAMELOWERSHORTDataType, NDDataType_t dataType, NDColorMode_t colorMode){
    const char* fuctionName = "getDRIVERNAMESTANDARDFrameFormat";
    asynStatus status = asynSuccess;

    // Note that only the most common data type + color mode combos are written out here. Follow the format set for these to add new ones.
    switch(colorMode){
        case NDColorModeMono:
            switch(dataType) {
                case NDUInt8:
                    *DRIVERNAMELOWERSHORTDataType = 0;
                    break;
                case NDUInt16:
                    *DRIVERNAMELOWERSHORTDataType = 1;
                    break;
                default:
                asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unsupported ColorMode + DataType combination\n", driverName, functionName);
                return asynError;
            }
            break;
        case NDColorModeRGB1:
            switch(dataType) {
                case NDUInt8:
                    *DRIVERNAMELOWERSHORTDataType = 2;
                    break;
                case NDUInt16:
                    *DRIVERNAMELOWERSHORTDataType = 3;
                    break;
                default:
                asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unsupported ColorMode + DataType combination\n", driverName, functionName);
                return asynError;
            }
            break;
        case NDColorModeRGB2:

        case NDColorModeRGB3:

        case NDColorModeBayer:

        case NDColorModeYUV444:

        case NDColorModeYUV422:

        case NDColorModeYUV421:

        default:
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unsupported ColorMode + DataType combination\n", driverName, functionName);
            return asynError;
    }
}



/**
 * Function that gets frame format information from a captured frame from the camera
 * 
 * @params[in]:     DRIVERNAMELOWERSHORTFrame  -> pointer to currently acquired frame
 * @params[out]:    dataType    -> pointer to output data type
 * @params[out]:    colorMode   -> pointer to output color mode
 * @return:         status       
 */
asynStatus ADDRIVERNAMESTANDARD::getFrameFormatND(______* DRIVERNAMELOWERSHORTFrame, NDDataType_t* dataType, NDColorMode_t* colorMode){
    const char* functionName = "getFrameFormatND";
    asynStatus status = asynSuccess;
    // here, retrieve the frame data type and color mode int whatever format they are stored in. Generally some form of int

    // For example:
    unsigned int* DRIVERNAMELOWERSHORTType = DRIVERNAMELOWERSHORTFrame->type; 
    switch(DRIVERNAMELOWERSHORTType){
        case TYPE_A:
            *dataType = NDUInt8;
            *coloMode = NDColorModeMono;
            break;
        default:
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unsupported Frame format\n", driverName, functionName);
            status = asynError;
            break;
    }
    return status;
}


asynStatus ADDRIVERNAMESTANDARD::DRIVERNAMELOWERSHORTFrame2NDArray(______* DRIVERNAMELOWERSHORTFrame, NDArray* pArray){
    
}
