#include "MIDI_COMMANDS.h"

#define POT PIN_A0

bool notePressedLastTick[107];

void setup()
{
    pinMode(2, INPUT_PULLUP);
    Serial.begin(115200);

    for (int i = 0; i < sizeof(notePressedLastTick); i++)
        notePressedLastTick[i] = false;
}

// If you don't want to send param2, set it to -1
void sendMessage(unsigned int command, int param1, int param2)
{
    char buffer[32];
    int size = 0;
    int messagesIncoming = 3;

    if (param2 < 0)
        messagesIncoming = 2;

    Serial.write(messagesIncoming);
    Serial.write(command);
    Serial.write(param1);

    if (param2 >= 0)
        Serial.write(param2);
}

void noteOn(unsigned int note, unsigned int velocity)
{
    sendMessage(NOTE_ON, note, velocity);
    setNotePressed(note, true);
}

void noteOff(unsigned int note, unsigned int velocity)
{
    sendMessage(NOTE_OFF, note, velocity);
    setNotePressed(note, false);
}

uint8_t wasNotePressedLastTick(unsigned int note)
{
    return notePressedLastTick[note - NOTE_OFFSET_FROM_0];
}

void setNotePressed(unsigned int note, unsigned int status)
{
    notePressedLastTick[note - NOTE_OFFSET_FROM_0] = status;
}

void loop()
{
    uint8_t middleC = 60;
    uint8_t note = middleC;

    if (digitalRead(2) == LOW && !wasNotePressedLastTick(note))
        noteOn(note, 127);
    else if (digitalRead(2) == HIGH && wasNotePressedLastTick(note))
        noteOff(note, 127);
}