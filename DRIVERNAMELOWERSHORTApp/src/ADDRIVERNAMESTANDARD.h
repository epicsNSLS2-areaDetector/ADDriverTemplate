/*
 * Header file for the ADDRIVERNAMESTANDARD EPICS driver
 *
 * This file contains the definitions of PV params, and the definition of the ADDRIVERNAMESTANDARD class and functions.
 *
 * Author: 
 * Created:
 *
 * Copyright (c) :
 *
 */

// header guard
#ifndef ADDRIVERNAMEUPPER_H
#define ADDRIVERNAMEUPPER_H

// version numbers
#define ADDRIVERNAMESTANDARD_VERSION      0
#define ADDRIVERNAMESTANDARD_REVISION     0
#define ADDRIVERNAMESTANDARD_MODIFICATION 0


// Place PV string definitions here
// #define ADDRIVERNAMEUPPERSHORT_PVNameString          "PV_NAME"            //asynInt32


// Place any required inclues here

#include "ADDriver.h"


// ----------------------------------------
// DRIVERNAMESTANDARD Data Structures
//-----------------------------------------

// Place any in use Data structures here



/*
 * Class definition of the ADDRIVERNAMESTANDARD driver. It inherits from the base ADDriver class
 *
 * Includes constructor/destructor, PV params, function defs and variable defs
 *
 */
class ADDRIVERNAMESTANDARD : ADDriver{

    public:

        // Constructor - NOTE THERE IS A CHANCE THAT YOUR CAMERA DOESNT CONNECT WITH SERIAL # AND THIS MUST BE CHANGED
        ADDRIVERNAMESTANDARD(const char* portName, const char* serial, int maxBuffers, size_t maxMemory, int priority, int stackSize);


        // ADDriver overrides
        virtual asynStatus writeInt32(asynUser* pasynUser, epicsInt32 value);
        virtual asynStatus writeFloat64(asynUser* pasynUser, epicsFloat64 value);


        // destructor. Disconnects from camera, deletes the object
        ~ADDRIVERNAMESTANDARD();

    protected:

        
        #define ADDRIVERNAMEUPPER_FIRST_PARAM FIRST_PV_INDEX
        #define ADDRIVERNAMEUPPER_LAST_PARAM LAST_PV_INDEX

    private:

        // Some data variables
        epicsEventId startEventId;
        epicsEventId endEventId;
        

        // ----------------------------------------
        // DRIVERNAMESTANDARD Global Variables
        //-----------------------------------------

        processThreadRunning = 0;
        pthread_t imageCollectionThread;

        // ----------------------------------------
        // DRIVERNAMESTANDARD Functions - Logging/Reporting
        //-----------------------------------------

        //function used to report errors in DRIVERNAMELOWERSHORT operations
        // Note that vendor libraries usually have a status data structure, if not
        // it might be wise to make one
        void reportDRIVERNAMESTANDARDError(______ status, const char* functionName);

        // reports device and driver info into a log file
        void report(FILE* fp, int details);

        // writes to ADStatus PV
        void updateStatus(const char* status);

        // ----------------------------------------
        // UVC Functions - Connecting to camera
        //-----------------------------------------

        //function used for connecting to a DRIVERNAMESTANDARD device
        // NOTE - THIS MAY ALSO NEED TO CHANGE IF SERIAL # NOT USED
        asynStatus connectToDeviceDRIVERNAMESTANDARD(const char* serialNumber);

        //function used to disconnect from DRIVERNAMESTANDARD device
        asynStatus disconnectFromDeviceDRIVERNAMESTANDARD();

        // ----------------------------------------
        // DRIVERNAMESTANDARD Functions - Camera functions
        //-----------------------------------------

        //function that sets exposure time
        asynStatus setExposure(int exposureTime);
        asynStatus setGamma(int gamma);
        asynStatus setBacklightCompensation(int backlightCompensation);
        asynStatus setBrightness(int brightness);
        asynStatus setContrast(int contrast);
        asynStatus setGain(int gain);
        asynStatus setPowerLineFrequency(int powerLineFrequency);
        asynStatus setHue(int hue);
        asynStatus setSaturation(int saturation);
        asynStatus setSharpness(int sharpness);

        //function that begins image aquisition
        asynStatus acquireStart();

        //function that stops aquisition
        void acquireStop();

        //function that converts a DRIVERNAMESTANDARD frame into an NDArray
        asynStatus DRIVERNAMELOWERSHORTFrame2NDArray(______* frame, NDArray* pArray, NDDataType_t dataType, NDColorMode_t colorMode, size_t imBytes);

};

// Stores number of additional PV parameters are added by the driver
#define NUM_DRIVERNAMEUPPER_PARAMS ((int)(&ADDRIVERNAMEUPPER_LAST_PARAM - &ADDRIVERNAMEUPPER_FIRST_PARAM + 1))

#endif