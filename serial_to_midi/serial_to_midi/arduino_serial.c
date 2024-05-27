#include <windows.h>
#include <stdio.h>
#include "arduino_serial.h"

typedef union _MIDI_MESSAGE
{
    DWORD dMessage;
    WORD wMessage[2];
    byte bMessage[4];
} MIDI_MESSAGE;

int openPort(LPCWSTR portName, HANDLE* hSerial) {
    *hSerial = CreateFileW(portName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (*hSerial == INVALID_HANDLE_VALUE)
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

int serialRead(HANDLE hSerial, char* buffer, int bytesToRead, int* bytesRead) {
    return !ReadFile(hSerial, buffer, bytesToRead, bytesRead, NULL);
}

int serialReadMessage(HANDLE hSerial, int *message, int *bytesRead) {
    int messagesIncoming = 0;
    int command = 0;
    int param1 = 0;
    int param2 = 0;

    if (serialRead(hSerial, &messagesIncoming, 1, bytesRead))
        return 1; // Error reading from serial port

    if (*bytesRead == 0)
        return 0;

    if (messagesIncoming == 3) {
        if (serialRead(hSerial, &command, 1, NULL))
            return 1; // Error reading from serial port
        if (serialRead(hSerial, &param1, 1, NULL))
            return 1; // Error reading from serial port
        if (serialRead(hSerial, &param2, 1, NULL))
            return 1; // Error reading from serial port
    }
    else if (messagesIncoming == 2) {
        if (serialRead(hSerial, &command, 1, NULL))
            return 1; // Error reading from serial port
        if (serialRead(hSerial, &param1, 1, NULL))
            return 1; // Error reading from serial port
    }

    message[0] = command;
    message[1] = param1;
    message[2] = param2;

    return 0;
}

void closePort(HANDLE hSerial) {
    CloseHandle(hSerial);
}
