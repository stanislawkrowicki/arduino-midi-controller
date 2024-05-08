#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

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
    //MMRESULT result;
    UINT deviceID = 0;

    printf("Which device to open? (empty for 0): ");
    char input[10];

    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] != '\n') {
            deviceID = atoi(input);
        }
    }

    UINT IDCopy = deviceID;

    result = midiOutOpen(&hMidiDevice,
        deviceID,      
        (DWORD_PTR)NULL,
        (DWORD_PTR)NULL,
        CALLBACK_NULL);


    if (result != MMSYSERR_NOERROR) {
        wchar_t errorText[256];
        midiOutGetErrorText(result, errorText, sizeof(errorText)/sizeof(wchar_t));
        printf("ERROR: Failed to open MIDI device with ID %u! Error code: %u, Message: %ls\n", IDCopy, result, errorText);
        Sleep(5000);
        return -1;
    }

    printf("Device %d opened succesfully.\n", deviceID);
    Sleep(5000);

    return 0;
}
