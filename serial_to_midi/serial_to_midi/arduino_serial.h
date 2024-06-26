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
 * Reads bytesToRead bytes written at port.
 *
 * @param hSerial Device handler
 * @param *buffer String buffer to be filled. The buffer is \0 terminated.
 * @param bytesToRead How many bytes to read from port
 * @param *bytesRead Number of bytes read to be filled
 * @return Status code: 0 - success, 1 - Error reading from the serial port
 */
int serialRead(HANDLE hSerial, char* buffer, int bytesToRead, int* bytesRead);

/**
 * Reads serial written at port. Every packet should be preceded by packet size (one byte).
 *
 * @param hSerial Device handler
 * @param *message Array of 3 ints: command, param1, param2.
 * @param *bytesRead Number of bytes read to be filled. Does not count the size byte.
 * @return Status code: 0 - success, 1 - Error reading from the serial port
 */
int serialReadMessage(HANDLE hSerial, int* message, int* bytesRead);

/**
 * Closes the serial port
 *
 * @param hSerial Device handler
 */
void closePort(HANDLE hSerial);

#endif ARDUINO_SERIAL_H_