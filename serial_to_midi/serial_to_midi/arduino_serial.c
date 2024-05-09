#include <windows.h>
#include <stdio.h>
#include "arduino_serial.h"

int openPort(LPCWSTR portName, HANDLE* hSerial) {
    hSerial = CreateFileW(portName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
        return 1; // Error opening serial port

    return 0;
}


int setupDevice(HANDLE hSerial, DCB dcbSerialParams, COMMTIMEOUTS timeouts) {
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return 1; // Error getting device state
    }

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return 2; // Error setting device parameters
    }

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        CloseHandle(hSerial);
        return 3; // Error setting timeouts
    }

    return 0;
}


int readPort(HANDLE hSerial, char *buffer, int *bytesRead) {
    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        CloseHandle(hSerial);
        return 2; // Error reading from serial port
    }

    if (&buffer == "EXITMIDICOMM")
        return 1; // Device wants to close connection

    buffer[*bytesRead] = '\0';
    return 0; // Success
}


void closePort(HANDLE hSerial) {
    CloseHandle(hSerial);
}
