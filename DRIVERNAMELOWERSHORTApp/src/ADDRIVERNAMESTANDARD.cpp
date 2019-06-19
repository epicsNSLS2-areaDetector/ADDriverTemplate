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
asynStatus ADDRIVERNAMESTANDARD::connectToDeviceDRIVERNAMESTANDARD(const char* serialNumber){
    const char* functionName = "connectToDeviceDRIVERNAMESTANDARD";
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
asynStatus ADDRIVERNAMESTANDARD::disconnectFromDeviceDRIVERNAMESTANDARD(){
    const char* functionName = "disconnectFromDeviceDRIVERNAMESTANDARD";

    // Free up any data allocated by driver here, and call the vendor libary to disconnect

    return asynSuccess;
}


/**
 * Function that updates PV values with camera information
 * 
 * @return: status
 */
asynStatus ADDRIVERNAMESTANDARD::getDeviceInformation(){
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
    if(this->processThreadRunning == 0){
        if(pthread_create(&this->imageCollectionThread, NULL, newFrameCallbackWrapper, this)){
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Error creating Image Acquisition thread\n", driverName, functionName);
            status = asynError;
        }
        else{
            this->processThreadRunning = 1;
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
    if(this->processThreadRunning == 0){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Image thread not active\n", driverName, functionName);
        status = asynError;
    }
    else{
        this->processThreadRunning = 0;
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
                    //*DRIVERNAMELOWERSHORTDataType = 0;
                    break;
                case NDUInt16:
                    //*DRIVERNAMELOWERSHORTDataType = 1;
                    break;
                default:
                asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unsupported ColorMode + DataType combination\n", driverName, functionName);
                return asynError;
            }
            break;
        case NDColorModeRGB1:
            switch(dataType) {
                case NDUInt8:
                    //*DRIVERNAMELOWERSHORTDataType = 2;
                    break;
                case NDUInt16:
                    //*DRIVERNAMELOWERSHORTDataType = 3;
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


/**
 * Function that converts DRIVERNAMESTANDARD frames into NDArrays for use with areaDetector
 * 
 * @params[in]:     DRIVERNAMELOWERSHORTFrame  -> pointer to currently acquired frame
 * @params[out]:    dataType    -> pointer to output data type
 * @params[out]:    colorMode   -> pointer to output color mode
 * @return:         status
 */
asynStatus ADDRIVERNAMESTANDARD::DRIVERNAMELOWERSHORTFrame2NDArray(______* DRIVERNAMELOWERSHORTFrame){
    const char* functionName = "DRIVERNAMELOWERSHORTFrame2NDArray";
    NDColorMode_t colorMode;
    NDDataType_t dataType;
    asynStatus status = asynSuccess;
    status = getFrameFormatND(DRIVERNAMELOWERSHORTFrame, &dataType, &colorMode);
    if(status != asynError){
        NDArrayInfo arrayInfo;
        NDArray* pArray;
        size_t ndims;
    
        if(colorMode == NDColorModeMono) ndims = 2;
        else ndims = 3;
        size_t dims[ndims];
        if(ndims == 2){
            dims[0] = frame->width;
            dims[1] = frame->height;
        }
        else{
            dims[0] = 3;
            dims[1] = frame->width;
            dims[2] = frame->height;
        }
    
        this->pArrays[0] = pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        if(this->pArrays[0]!=NULL){ 
            pArray = this->pArrays[0];   
        }
        else{
            this->pArrays[0]->release();
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Unable to allocate array\n", driverName, functionName);
            // FREE DRIVERNAMELOWERSHORTFrame HERE!
            return;
        }

        pArray->getInfo(&arrayInfo);
        setIntegerParam(NDArraySize, (int)arrayInfo.totalBytes);
        setIntegerParam(NDArraySizeX, arrayInfo.xSize);
        setIntegerParam(NDArraySizeY, arrayInfo.ySize);

        // MAKE SURE YOU COPY THE IMAGE DATA FROM THE VENDOR LIB FRAME DATA STRUCTURE
        memcpy(pArray->pData, DRIVERNAMELOWERSHORTFrame->data, (int) arrayInfo.totalBytes);
    
        updateTimeStamp(&pArray->epicsTS);
    
        pArray->pAttributeList->add("DataType", "Data Type", NDAttrInt32, &dataType);
        pArray->pAttributeList->add("ColorMode", "Color Mode", NDAttrInt32, &colorMode);
        //increment the array counter
        int arrayCounter;
        getIntegerParam(NDArrayCounter, &arrayCounter);
        arrayCounter++;
        setIntegerParam(NDArrayCounter, arrayCounter);
        //refresh PVs
        callParamCallbacks();
        //Sends image to the ArrayDataPV
        getAttributes(pArray->pAttributeList);
        doCallbacksGenericPointer(pArray, NDArrayData, 0);
        //frees up memory
    }
    
    // Always free array whether successful or not
    // FREE DRIVERNAMELOWERSHORTFrame HERE!
    pArray->release();
    return status;
}


/*
 * Function used as a wrapper function for the callback.
 * In some cases thi is necessary threads take static function as parameters
 * 
 * @params[out]: ptr     -> 'this' cast as a void pointer. It is cast back to ADUVC object and then new frame callback is called
 * @return: void
 */
static void newFrameCallbackWrapper(void* ptr){
    ADDRIVERNAMESTANDARD* pPvt = ((ADDRIVERNAMESTANDARD*) ptr);
    pPvt->newFrameCallback();
}


/*
 * Function that performs the callbacks onto new frames generated by the camera.
 * The way this function needs to be implemented depends on the vendor library SDK 
 * being used. There are two cases:
 * 
 * Case 1: Vendor Lib generates callback thread. 
 * In this case, you will likely need to edit this function to accept some kind of frame
 * as a parameter, and you also may need to make a static function wrapper around it. There
 * will be no need for a loop inside this function itself, as that will be handled within the
 * vendor library. An example of this case is the ADUVC driver.
 * 
 * Case 2: You create your own image handling thread
 * In this case, you will need to create your own image processing thread. In this case, you will need
 * to make a global parameter along the lines of 'processThreadRunning' which is toggled on/off in the
 * acquireStart/Stop functions. The loop should be while(processThreadRunning). This callback is then
 * passed as the parameter to the created image handler thread. An example of this is the ADEmergentVision driver
 *
 * @return: void
 */
void ADDRIVERNAMESTANDARD::newFrameCallBack() {
    // In this function, you must 
    const char* functionName = "newFrameCallBack";
    // NOTE THAT THIS LOOP ASSUMES THAT YOU ARE CREATING YOUR OWN PROCESS THREAD. LOOK AT THE COMMENT ABOVE FOR DETAILS
    while(processThreadRunning){
        ______* DRIVERNAMELOWERSHORTFrame;
        ____* DRIVERNAMELOWERSHORTDataType;
        int dataType;
        int colorMode;
        int operatingMode;
        getIntegerParam(ADImageMode, &operatingMode);
        getIntegerParam(NDDataType, &dataType);
        getIntegerParam(NDColorMode, &colorMode);

        getDRIVERNAMESTANDARDFrameFormat(DRIVERNAMELOWERSHORTDataType, (NDDataType_t) dataType, (NDColorMode_t) colorMode);


        DRIVERNAMELOWERSHORTFrame2NDArray(DRIVERNAMELOWERSHORTFrame);
        int numImages;
        getIntegerParam(ADNumImagesCounter, &numImages);
        numImages++;
        setIntegerParam(ADNumImagesCounter, numImages);

        if(operatingMode == ADImageSingle){
            acquireStop();
        }

        // block shot mode stops once numImages reaches the number of desired images
        else if(operatingMode == ADImageMultiple){
            int desiredImages;
            getIntegerParam(ADNumImages, &desiredImages);

            if(numImages>=desiredImages){
                acquireStop();
            }
        }
    }
}


//---------------------------------------------------------
// Base DRIVERNAMESTANDARD Camera functionality
//---------------------------------------------------------

/**
 * Function that sets exposure time in seconds
 * 
 * @params[in]: exposureTime -> the exposure time in seconds
 * @return: status
 */
asynStatus ADDRIVERNAMESTANDARD::setExposure(int exposureTime){
    const char* functionName = "setExposure";
    asynStatus status = asynSuccess;
    updateStatus("Set Exposure");
    // HERE CALL VENDOR LIBRARY TO SET VALUE. IT SHOULD RETURN SOME STATUS WHICH IS USED NEXT FOR ERROR CHECK
    if(deviceStatus < 0){
        reportDRIVERNAMESTANDARDError(deviceStatus, functionName);
        status = asynError;
    }
    return status;
}


/**
 * Function that sets camera gain value
 * 
 * @params[in]: gain -> value for gain
 * @return: status
 */
asynStatus ADDRIVERNAMESTANDARD::setGain(int gain){
    const char* functionName = "setGain";
    asynStatus status = asynSuccess;
    updateStatus("Set Gain");
    // HERE CALL VENDOR LIBRARY TO SET VALUE. IT SHOULD RETURN SOME STATUS WHICH IS USED NEXT FOR ERROR CHECK
    if(deviceStatus < 0){
        reportDRIVERNAMESTANDARDError(deviceStatus, functionName);
        status = asynError;
    }
    return status;
}


// ADD ANY OTHER SETTER CAMERA FUNCTIONS HERE, ADD CALL THEM IN WRITE INT32/FLOAT64


//-------------------------------------------------------------------------
// ADDriver function overwrites
//-------------------------------------------------------------------------



/*
 * Function overwriting ADDriver base function.
 * Takes in a function (PV) changes, and a value it is changing to, and processes the input
 *
 * @params[in]: pasynUser       -> asyn client who requests a write
 * @params[in]: value           -> int32 value to write
 * @return: asynStatus      -> success if write was successful, else failure
 */
asynStatus ADDRIVERNAMESTANDARD::writeInt32(asynUser* pasynUser, epicsInt32 value){
    int function = pasynUser->reason;
    int acquiring;
    int status = asynSuccess;
    static const char* functionName = "writeInt32";
    getIntegerParam(ADAcquire, &acquiring);

    status = setIntegerParam(function, value);
    // start/stop acquisition
    if(function == ADAcquire){
        if(value && !acquiring){
            //asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Entering aquire\n", driverName, functionName);
            deviceStatus = acquireStart();
            if(deviceStatus < 0){
                reportDRIVERNAMESTANDARDError(deviceStatus, functionName);
                return asynError;
            }
        }
        if(!value && acquiring){
            acquireStop();
        }
    }

    else if(function == ADImageMode){
        if(acquiring == 1) acquireStop();
        if(value == ADImageSingle) setIntegerParam(ADNumImages, 1);
        else if(value == ADImageMultiple) setIntegerParam(ADNumImages, 300);
        else if(value == ADImageContinuous) setIntegerParam(ADNumImages, 3000);
        else{
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s ERROR: Unsupported camera operating mode\n", driverName, functionName);
            return asynError;
        }
    }

    else{
        if (function < ADDRIVERNAMEUPPER_FIRST_PARAM) {
            status = ADDriver::writeInt32(pasynUser, value);
        }
    }
    callParamCallbacks();

    if(status){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s ERROR status=%d, function=%d, value=%d\n", driverName, functionName, status, function, value);
        return asynError;
    }
    else asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s function=%d value=%d\n", driverName, functionName, function, value);
    return asynSuccess;
}


/*
 * Function overwriting ADDriver base function.
 * Takes in a function (PV) changes, and a value it is changing to, and processes the input
 * This is the same functionality as writeInt32, but for processing doubles.
 *
 * @params[in]: pasynUser       -> asyn client who requests a write
 * @params[in]: value           -> int32 value to write
 * @return: asynStatus      -> success if write was successful, else failure
 */
asynStatus ADDRIVERNAMESTANDARD::writeFloat64(asynUser* pasynUser, epicsFloat64 value){
    int function = pasynUser->reason;
    int acquiring;
    int status = asynSuccess;
    static const char* functionName = "writeFloat64";
    getIntegerParam(ADAcquire, &acquiring);

    status = setDoubleParam(function, value);

    if(function == ADAcquireTime){
        if(acquiring) acquireStop();
    }
    else if(function == ADGain) setGain((int) value);
    else{
        if(function < ADDRIVERNAMESTANDARD_FIRST_PARAM){
            status = ADDriver::writeFloat64(pasynUser, value);
        }
    }
    callParamCallbacks();

    if(status){
        asynPrint(this-> pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s ERROR status = %d, function =%d, value = %f\n", driverName, functionName, status, function, value);
        return asynError;
    }
    else asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER, "%s::%s function=%d value=%f\n", driverName, functionName, function, value);
    return asynSuccess;
}



/*
 * Function used for reporting ADUVC device and library information to a external
 * log file. The function first prints all libuvc specific information to the file,
 * then continues on to the base ADDriver 'report' function
 * 
 * @params[in]: fp      -> pointer to log file
 * @params[in]: details -> number of details to write to the file
 * @return: void
 */
void ADDRIVERNAMESTANDARD::report(FILE* fp, int details){
    const char* functionName = "report";
    int height;
    int width;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s reporting to external log file\n",driverName, functionName);
    if(details > 0){
        fprintf(fp, " Vendor SDK Version        ->      %d.%d.%d\n", VENDOR_VERSION, LIBUVC_VERSION_MINOR, LIBUVC_VERSION_PATCH);
        fprintf(fp, " -------------------------------------------------------------------\n");
        if(!connected){
            fprintf(fp, " No connected devices\n");
            ADDriver::report(fp, details);
            return;
        }
        fprintf(fp, " Connected Device Information\n");
        // GET CAMERA INFORMATION HERE AND PRINT IT TO fp
        getIntegerParam(ADSizeX, &width);
        getIntegerParam(ADSizeY, &height);
        fprintf(fp, " Image Width           ->      %d\n", width);
        fprintf(fp, " Image Height          ->      %d\n", height);
        fprintf(fp, " -------------------------------------------------------------------\n");
        fprintf(fp, "\n");
        
        ADDriver::report(fp, details);
    }
}




//----------------------------------------------------------------------------
// ADDRIVERNAMESTANDARD Constructor/Destructor
//----------------------------------------------------------------------------


ADDRIVERNAMESTANDARD::ADDRIVERNAMESTANDARD(const char* portName, const char* serial, int maxbuffers, size_t maxMemory, int priority, int stackSize )
    : ADDriver(portName, 1, (int)NUM_DRIVERNAMEUPPER_PARAMS, maxBuffers, maxMemory, asynEnumMask, asynEnumMask, ASYN_CANBLOCK, 1, priority, stackSize){
    static const char* functionName = "ADDRIVERNAMESTANDARD";

    // Call createParam here
    // ex. createParam(ADUVC_UVCComplianceLevelString, asynParamInt32, &ADUVC_UVCComplianceLevel);

    //sets driver version
    char versionString[25];
    epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d", ADDRIVERNAMEUPPER_VERSION, ADDRIVERNAMEUPPER_REVISION, ADDRIVERNAMEUPPER_MODIFICATION);
    setStringParam(NDDriverVersion, versionString);

    if(strlen(serial) < 0){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s Connection failed, abort\n", driverName, functionName);
    }
    else{
        connected = connectToDeviceDRIVERNAMESTANDARD(serial);
        if(connected){
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s::%s Acquiring device information\n", driverName, functionName);
            getDeviceInformation();
        }
    }

     // when epics is exited, delete the instance of this class
    epicsAtExit(exitCallbackC, this);
}


ADDRIVERNAMESTANDARD::~ADDRIVERNAMESTANDARD(){
    const char* functionName = "~ADDRIVERNAMESTANDARD";
    disconnectFromDeviceDRIVERNAMESTANDARD();
    asynPrint(this->pasynUserSelf, ASYN_TRACEIO_DRIVER,"%s::%s ADUVC driver exiting\n", driverName, functionName);
    disconnect(this->pasynUserSelf);
}


//-------------------------------------------------------------
// ADDRIVERNAMESTANDARD ioc shell registration
//-------------------------------------------------------------

/* DRIVERNAMESTANDARDConfig -> These are the args passed to the constructor in the epics config function */
static const iocshArg DRIVERNAMESTANDARDConfigArg0 = { "Port name",        iocshArgString };
static const iocshArg DRIVERNAMESTANDARDConfigArg1 = { "Serial number",    iocshArgString };
static const iocshArg DRIVERNAMESTANDARDConfigArg2 = { "maxBuffers",       iocshArgInt };
static const iocshArg DRIVERNAMESTANDARDConfigArg3 = { "maxMemory",        iocshArgInt };
static const iocshArg DRIVERNAMESTANDARDConfigArg4 = { "priority",         iocshArgInt };
static const iocshArg DRIVERNAMESTANDARDConfigArg5 = { "stackSize",        iocshArgInt };


/* Array of config args */
static const iocshArg * const DRIVERNAMESTANDARDConfigArgs[] =
        { &DRIVERNAMESTANDARDConfigArg0, &DRIVERNAMESTANDARDConfigArg1, &DRIVERNAMESTANDARDConfigArg2,
        &DRIVERNAMESTANDARDConfigArg3, &DRIVERNAMESTANDARDConfigArg4, &DRIVERNAMESTANDARDConfigArg5 };


/* what function to call at config */
static void configDRIVERNAMESTANDARDCallFunc(const iocshArgBuf *args) {
    ADDRIVERNAMESTANDARDConfig(args[0].sval, args[1].sval, args[2].ival, args[3].ival, args[4].ival, args[5].ival);
}


/* information about the configuration function */
static const iocshFuncDef configUVC = { "ADDRIVERNAMESTANDARDConfig", 5, DRIVERNAMESTANDARDConfigArgs };


/* IOC register function */
static void DRIVERNAMESTANDARDRegister(void) {
    iocshRegister(&configDRIVERNAMESTANDARD, configDRIVERNAMESTANDARDCallFunc);
}


/* external function for IOC register */
extern "C" {
    epicsExportRegistrar(DRIVERNAMESTANDARDRegister);
}