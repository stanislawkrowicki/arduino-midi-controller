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

    if (param2 >= 0)
        snprintf(buffer, 32, "%d %d %d", command, param1, param2);
    else
        snprintf(buffer, 32, "%d %d", command, param1);

    Serial.print(buffer);
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
    int potValue = analogRead(POT);
    float noteOffset = potValue * (5.0 / 1024.0);

    uint8_t middleC = 60;
    uint8_t note = middleC + noteOffset;

    if (digitalRead(2) == LOW && !wasNotePressedLastTick(note))
        noteOn(note, 127);
    else if (digitalRead(2) == HIGH && wasNotePressedLastTick(note))
        noteOff(note, 127);
}