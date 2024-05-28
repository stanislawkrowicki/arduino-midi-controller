#include "MIDI_COMMANDS.h"

#define VOLUME_POT PIN_A0
#define FIRST_BUTTON 2

bool notePressedLastTick[107];

int notes[8] = {NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B, NOTE_C2};

void setup()
{
    for (int i = FIRST_BUTTON; i <= FIRST_BUTTON + 8; i++)
    {
        pinMode(i, INPUT_PULLUP);
    }

    Serial.begin(115200);

    for (int i = 0; i < sizeof(notePressedLastTick); i++)
        notePressedLastTick[i] = false;
}

// If you don't want to send param2, set it to -1
void sendMessage(unsigned int command, int param1, int param2)
{
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
    uint8_t keyOffset = 4;
    uint8_t octaveOffset = 1;

    int potValue = analogRead(VOLUME_POT);
    uint8_t volume = map(potValue, 0, 1023, 0, 127);

    for (int i = FIRST_BUTTON; i <= FIRST_BUTTON + 8; i++)
    {
        uint8_t note = notes[i - FIRST_BUTTON] + keyOffset + (12 * octaveOffset);

        if (digitalRead(i) == LOW && !wasNotePressedLastTick(note))
        {
            noteOn(note, volume);
        }
        else if (digitalRead(i) == HIGH && wasNotePressedLastTick(note))
        {
            noteOff(note, volume);
        }
    }
}