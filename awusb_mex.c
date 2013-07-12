/*
awusb_mex.c

Compile in MATLAB with mex awusb_mex.c [-v]
For description see awusb_mex.m

Copyright (C) 2012 Andreas Widmann, University of Leipzig, widmann@uni-leipzig.de

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <mex.h>
#include <sys/types.h>
#include "awusb.c"

#define AWUSB_MAX_DEVS 256

typedef void (*MexFunctionPtr)(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static awusb_device *device_table[256];

void atExit(void)
{

    uint64_T i;

    /* Clear out device table */
    for (i = 0; i < AWUSB_MAX_DEVS; i++) {
        if (device_table[i] != NULL) {
            if (device_table[i]->claimed) {
                awusb_release(device_table[i]);
            }
            mxFree(device_table[i]);
        }
        device_table[i] = NULL;
    }

}


void AWUSBCloseAll(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    uint64_T i;

    /* Release device */
    for (i = 0; i < AWUSB_MAX_DEVS; i++) {
        if (device_table[i] != NULL) {
            if (device_table[i]->claimed) {
                if (awusb_release(device_table[i]) != AWUSB_OK) {
                    mexErrMsgTxt("Could not close ActiveWire device.");
                }
            }
        }
    }

}

void AWUSBOpen(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    uint64_T dev;

    /* The device number must be noncomplex scalar double */
    if (nrhs != 1 || !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || !mxIsScalar(prhs[0])) {
        mexErrMsgTxt("Device must be noncomplex scalar double.");
    }

    /* Assign pointers to each input */
    dev = *mxGetPr(prhs[0]);
    if (dev < 1 || dev > AWUSB_MAX_DEVS) {
        mexErrMsgTxt("Device number out of range.");
    }
    dev--;

    /* Claim device */
    if (device_table[dev] == NULL || device_table[dev]->claimed || awusb_claim(device_table[dev]) != AWUSB_OK) {
        mexErrMsgTxt("Could not open ActiveWire device.");
    }

}

void AWUSBClose(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    uint64_T dev;

    /* The device number must be noncomplex scalar double */
    if (nrhs != 1 || !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || !mxIsScalar(prhs[0])) {
        mexErrMsgTxt("Device must be noncomplex scalar double.");
    }

    /* Assign pointers to each input */
    dev = *mxGetPr(prhs[0]);
    if (dev < 1 || dev > AWUSB_MAX_DEVS) {
        mexErrMsgTxt("Device number out of range.");
    }
    dev--;

    /* Release device */
    if (device_table[dev] == NULL || awusb_release(device_table[dev]) != AWUSB_OK) {
        mexErrMsgTxt("Could not close ActiveWire device.");
    }
    
}

void AWUSBToggle(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *bitsWayPtr;
    int i;
    uint64_T dev;
    unsigned int awDirection = 0;

    /* Two input arguments required */
    if (nrhs != 2) {
        mexErrMsgTxt("Not enough input arguments.");
    }

    /* The device number must be noncomplex scalar double */
    if (!mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || !mxIsScalar(prhs[0])) {
        mexErrMsgTxt("Device must be noncomplex scalar double.");
    }

    /* The direction vector must be noncomplex scalar double */
    if (!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || mxGetM(prhs[1]) * mxGetN(prhs[1]) != 16) {
        mexErrMsgTxt("Direction vector must be 16 element double.");
    }

    /* Assign pointers to each input */
    dev = *mxGetPr(prhs[0]);
    if (dev < 1 || dev > AWUSB_MAX_DEVS) {
        mexErrMsgTxt("Device number out of range.");
    }
    dev--;
    bitsWayPtr = mxGetPr(prhs[1]);

    /* Convert double vector to scalar integer */
    for (i = 0; i < 16; i++) {
        if (bitsWayPtr[i] != 0) {
            awDirection = awDirection | (1 << i);
        }
    }

    /* Toggle port */
    if (device_table[dev] == NULL || awusb_toggleport(device_table[dev], awDirection) != AWUSB_OK) {
        mexErrMsgTxt("Could not write to ActiveWire device.");
    }

}

void AWUSBWrite(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *bitsWayPtr;
    int i;
    uint64_T dev;
    unsigned int awOutputValue = 0;

    /* Two input arguments required */
    if (nrhs != 2) {
        mexErrMsgTxt("Not enough input arguments.");
    }

    /* The device number must be noncomplex scalar double */
    if (!mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || !mxIsScalar(prhs[0])) {
        mexErrMsgTxt("Device must be noncomplex scalar double.");
    }

    /* The direction vector must be noncomplex scalar double */
    if (!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || mxGetM(prhs[1]) * mxGetN(prhs[1]) != 16) {
        mexErrMsgTxt("Direction vector must be 16 element double.");
    }

    /* Assign pointers to each input */
    dev = *mxGetPr(prhs[0]);
    if (dev < 1 || dev > AWUSB_MAX_DEVS) {
        mexErrMsgTxt("Device number out of range.");
    }
    dev--;
    bitsWayPtr = mxGetPr(prhs[1]);

    /* Convert double vector to scalar integer */
    for (i = 0; i < 16; i++) {
        if (bitsWayPtr[i] != 0) {
            awOutputValue = awOutputValue | (1 << i);
        }
    }

    /* Write port */
    if (device_table[dev] == NULL || awusb_write(device_table[dev], awOutputValue) != 2) {
        mexErrMsgTxt("Could not write to ActiveWire device.");
    }

}

void AWUSBRead(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *bitsInPtr;
    int i;
    uint64_T dev;
    unsigned int awInputValue = 0;

    /* The input must be noncomplex scalar double */
    if (nrhs != 1 || !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || !mxIsScalar(prhs[0])) {
            mexErrMsgTxt("Device must be noncomplex scalar double.");
    }

    /* Assign pointers to each input and output */
    dev = *mxGetPr(prhs[0]);
    if (dev < 1 || dev > AWUSB_MAX_DEVS) {
        mexErrMsgTxt("Device number out of range.");
    }
    dev--;
    plhs[0] = mxCreateDoubleMatrix(1, 16, mxREAL);
    bitsInPtr = mxGetPr(plhs[0]);

    /* Read data */
    if (device_table[dev] == NULL || awusb_read(device_table[dev], &awInputValue) != 2) {
        mexErrMsgTxt("Could not read from ActiveWire device.");
    }

    /* Convert to double matrix */
    for (i = 0; i < 16; i++) {
        if (awInputValue & (1 << i)) {
            bitsInPtr[i] = 1;
        } else {
            bitsInPtr[i] = 0;
        }
    }
    
}

static MexFunctionPtr SelectFunction(char *command)
{
    if (strcmp(command, "Open") == 0) return &AWUSBOpen;
    if (strcmp(command, "Close") == 0) return &AWUSBClose;
    if (strcmp(command, "CloseAll") == 0) return &AWUSBCloseAll;
    if (strcmp(command, "Read") == 0) return &AWUSBRead;
    if (strcmp(command, "Write") == 0) return &AWUSBWrite;
    if (strcmp(command, "Toggle") == 0) return &AWUSBToggle;

    /* Unknown command */
    return NULL;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    static char firstTime = 1;
    int i, buflen;
    char *command;
    MexFunctionPtr f;

    /* Command input argument is required. */
    if (nrhs < 1) {
        mexErrMsgTxt("Not enough input arguments.");
    }

    /* Clear out device table */
    if (firstTime) {
        for (i = 0; i < AWUSB_MAX_DEVS; i++) {
            device_table[i] = NULL;
        }
        firstTime = 0;
        awusb_scanbus(device_table, AWUSB_MAX_DEVS, AWUSB_TRUE);
        mexAtExit(atExit);
    }

    /* Input must be a string. */
    if (mxIsChar(prhs[0]) != 1) {
        mexErrMsgTxt("Command/first input must be a string.");
    }

    /* Input must be a row vector. */
    if (mxGetM(prhs[0]) != 1) {
        mexErrMsgTxt("Command/first input must be a row vector.");
    }

    buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;
    command = mxCalloc(buflen, sizeof(char));
    mxGetString(prhs[0], command, buflen);

    nrhs--;
    prhs++;

    f = SelectFunction(command);
    if (f == NULL) {
        mexErrMsgTxt("Unkown command.\n");
    }
    (*f)(nlhs, plhs, nrhs, prhs);

}


