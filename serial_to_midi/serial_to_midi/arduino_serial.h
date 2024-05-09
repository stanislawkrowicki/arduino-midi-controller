#pragma once

#ifndef ARDUINO_SERIAL_H_
#define ARDUINO_SERIAL_H_

/**
 * Opens the specified serial port
 *
 * @param portName A Windows port identifier to open, eg. L"COM3"
 * @param *hSerial A pointer to your device handler to be filled
 * @return Error code: 0 - success, 1 - Error opening serial port
 */
int openPort(LPCWSTR portName, HANDLE* hSerial);

/**
 * Setups connection params and timeouts
 *
 * @param hSerial Device handler
 * @param dcbSerialParams 
 * @params timeouts
 * @return Error code: 0 - success, 1 - Error getting device state, 2 - Error setting device parameters, 3 - Error setting timeouts
 */
int setupDevice(HANDLE hSerial, DCB dcbSerialParams, COMMTIMEOUTS timeouts);

/**
 * Reads serial written at port
 *
 * @param hSerial Device handler
 * @param *buffer String buffer to be filled. The buffer is already \0 terminated (doesn't count to bytesRead).
 * @param *bytesRead Number of bytes read to be filled
 * @return Status code: 0 - success, 1 - Close connection signal sent (you should close the port yourself now), 2 - Error reading from the serial port
 */
int readPort(HANDLE hSerial, char* buffer, int* bytesRead);

/**
 * Closes the serial port
 *
 * @param hSerial Device handler
 */
void closePort(HANDLE hSerial);

#endif ARDUINO_SERIAL_H_