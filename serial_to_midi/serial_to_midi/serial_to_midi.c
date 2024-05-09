#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include "../../MIDI_COMMANDS.h"

#pragma comment(lib, "winmm.lib")

typedef union _MIDI_MESSAGE
{
    DWORD dMessage;
    WORD wMessage[2];
    byte bMessage[4];
} MIDI_MESSAGE;

static void safeMessage(HMIDIOUT hMidiDevice, MIDI_MESSAGE msg) {
    MMRESULT result = midiOutShortMsg(hMidiDevice, msg.dMessage);

    if (result != MMSYSERR_NOERROR) {
        wchar_t errorText[256];
        midiOutGetErrorText(result, errorText, sizeof(errorText) / sizeof(wchar_t));
        printf("ERROR: Failed to send message. Error code: %u, Message: %ls\n", result, errorText);
    }
}

static void noteOn(HMIDIOUT hMidiDevice, DWORD note, DWORD velocity) {
    MIDI_MESSAGE message;
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
        midiOutGetErrorText(result, errorText, sizeof(errorText)/sizeof(wchar_t));
        printf("ERROR: Failed to open MIDI device with ID %u! Error code: %u, Message: %ls\n", deviceID, result, errorText);
        return -1;
    }

    printf("Device %d opened succesfully.\n", deviceID);
    
    while (1) {
        printf("MIDI OUT: Ch %d: Note %d on velocity %d\n", 1, 50, 127);
        noteOn(hMidiDevice, 50, 127);
        Sleep(5000);    
    }

    return 0;
}
