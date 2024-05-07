#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

int main() {
    HMIDISTRM hMidiDevice;
    MMRESULT result;
    UINT deviceID = 0;

    printf("Which device to open? (empty for 0): ");
    char input[10];

    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] != '\n') {
            deviceID = atoi(input);
        }
    }

    UINT IDCopy = deviceID;

    result = midiStreamOpen(&hMidiDevice,
        &deviceID,
        1,           
        (DWORD_PTR)NULL,
        (DWORD_PTR)NULL,
        CALLBACK_NULL);

    if (result != MMSYSERR_NOERROR) {
        printf("ERROR: Failed to open MIDI device with ID %u! Error code: %u\n", IDCopy, result);
        return -1;
    }

    printf("Device %d opened succesfully.\n", deviceID);
    midiStreamClose(hMidiDevice);

    return 0;
}
