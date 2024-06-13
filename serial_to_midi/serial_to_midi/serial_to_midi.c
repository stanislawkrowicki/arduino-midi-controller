#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <string.h>
#include "../../MIDI_COMMANDS.h"
#include "arduino_serial.h"
#include <wchar.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "OneCore.lib")

typedef union _MIDI_MESSAGE
{
    DWORD dMessage;
    WORD wMessage[2];
    byte bMessage[4];
} MIDI_MESSAGE;

static void safeMessage(HMIDIOUT hMidiDevice, MIDI_MESSAGE msg) {
    MMRESULT result = midiOutShortMsg(hMidiDevice, msg.dMessage);

    //printf("MIDI Command: %d %d %d\n", msg.bMessage[0], msg.bMessage[1], msg.bMessage[2]);
    if (result != MMSYSERR_NOERROR) {
        wchar_t errorText[256];
        midiOutGetErrorText(result, errorText, sizeof(errorText) / sizeof(wchar_t));
        printf("ERROR: Failed to send message. Error code: %u, Message: %ls\n", result, errorText);
    }
}

static void noteOn(HMIDIOUT hMidiDevice, DWORD note, DWORD velocity) {
    MIDI_MESSAGE message = { .dMessage = "" };
    message.bMessage[0] = NOTE_ON;
    message.bMessage[1] = 50;
    message.bMessage[2] = 127;

    safeMessage(hMidiDevice, message);
}

int main() {
    UINT numMidiOutDevices = midiOutGetNumDevs();
    MIDIOUTCAPS midiOutCaps;
    MMRESULT result;

    printf("Number of MIDI Input Devices: %u\n", numMidiOutDevices);

    for (UINT i = 0; i < numMidiOutDevices; ++i) {
        result = midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS));
        if (result == MMSYSERR_NOERROR) {
            printf("Device ID: %u\n", i);
            printf("Name: %ls\n", midiOutCaps.szPname);
        }
        else {
            printf("Failed to get device capabilities for device ID %u\n", i);
        }
    }

    HMIDIOUT hMidiDevice;
    UINT deviceID = 0;

    printf("Which device to open? (empty for 0): ");
    char input[10];

    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] != '\n') {
            deviceID = atoi(input);
        }
    }

    result = midiOutOpen(&hMidiDevice,
        deviceID,
        (DWORD_PTR)NULL,
        (DWORD_PTR)NULL,
        CALLBACK_NULL);

    if (result != MMSYSERR_NOERROR) {
        wchar_t errorText[256];
        midiOutGetErrorText(result, errorText, sizeof(errorText) / sizeof(wchar_t));
        printf("ERROR: Failed to open MIDI device with ID %u! Error code: %u, Message: %ls\n", deviceID, result, errorText);
        return -1;
    }

    printf("Device %d opened succesfully.\n", deviceID);

    ULONG lpPortNumbers[25];
    ULONG portNumbersFound = 0;

    ULONG portSeekRes = GetCommPorts(lpPortNumbers, 25, &portNumbersFound);

    if (portSeekRes == ERROR_FILE_NOT_FOUND) {
        printf("There are no available COM ports to open.\n");
        midiOutClose(hMidiDevice);
        Sleep(10000);
    }
    else if (portSeekRes == ERROR_MORE_DATA) {
        printf("There was more available COM ports than expected. Some ports may not be visible.\n");
    }

    printf("Available COM ports:\n");

    for (int i = 0; i < portNumbersFound; i++) {
        printf("COM%d\n", lpPortNumbers[i]);
    }

    printf("Which port to open? Empty for first possible (COM%d): ", lpPortNumbers[0]);

    wchar_t comPort[8];

    if (fgetws(comPort, sizeof(comPort) / sizeof(wchar_t), stdin) != NULL) {
        // Remove the newline character if it exists
        size_t len = wcslen(comPort);
        if (len > 0 && comPort[len - 1] == L'\n') {
            comPort[len - 1] = L'\0';
        }

        if (comPort[0] == L'\0') {
            swprintf(comPort, sizeof(comPort) / sizeof(wchar_t), L"COM%d", lpPortNumbers[0]);
        }
    }

    HANDLE hSerial;
    int uartRes = openPort(comPort, &hSerial);

    if (uartRes != 0) {
        // TODO: Change showing error code to error message
        wprintf(L"Error while opening port %ls. Error code: %d\n", comPort, uartRes);
        return -1;
    }
    else {
        wprintf(L"Port %ls opened successfully.\n", comPort);
    }

    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    int res = setupDevice(hSerial, dcbSerialParams, timeouts);
   
    if (res != 0) {
        printf("Error while setting up device! Code: %d", res);
        return 1;
    }

    int message[3];
    int bytesRead = 0;

    while (1) {
        int status = serialReadMessage(hSerial, message, &bytesRead);

        if (status != 0) {
            printf("Failed to read message! Code: %d", status);
            return 1;
        }

        if (bytesRead > 0) {
            MIDI_MESSAGE midiMessage = { .dMessage = "" };
            midiMessage.bMessage[0] = message[0];
            midiMessage.bMessage[1] = message[1];
            midiMessage.bMessage[2] = message[2];
            safeMessage(hMidiDevice, midiMessage);
        }
    }

    midiOutClose(hMidiDevice);
    closePort(hSerial);
    return 0;
}
