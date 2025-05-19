int8_t PN5180ISO14443::readCardSerial(uint8_t *buffer) {
    PN5180DEBUG_PRINTLN("PN5180ISO14443::readCardSerial(*buffer)");

    // Always return 10 bytes
    // Offset 0..1 is ATQA
    // Offset 2 is SAK.
    // UID 4 bytes: offset 3 to 6 is UID, offset 7 to 9 to Zero
    // UID 7 bytes: offset 3 to 9 is UID
    uint8_t response[10] = {0};
    int8_t uidLength = activateTypeA(response, 0);

    if (uidLength <= 0) {
        return uidLength;
    }
  // UID length must be at least 4 bytes
    if (uidLength < 4) {
        return 0;
    }

    // Check for invalid ATQA (0xFF, 0xFF)
    if ((response[0] == 0xFF) && (response[1] == 0xFF)) {
        return 0;
    }

    // First UID byte should not be 0x00 or 0xFF
    if ((response[3] == 0x00) || (response[3] == 0xFF)) {
        return 0;
    }

    // Check for valid UID, skip first byte (0x04)
    bool validUID = false;
    for (int i = 1; i < uidLength; i++) {
        if ((response[i + 3] != 0x00) && (response[i + 3] != 0xFF)) {
            validUID = true;
            break;
        }
    }

    // Additional checks for 4-byte UID
    if (uidLength == 4) {
        if (response[3] == 0x88) {
            // Must not be the CT-flag (0x88)!
            validUID = false;
        }
    }

    // Additional checks for 7-byte UID
    if (uidLength == 7) {
        if (response[6] == 0x88) {
            // Must not be the CT-flag (0x88)!
            validUID = false;
        }
        if ((response[6] == 0x00) && (response[7] == 0x00) && (response[8] == 0x00) && (response[9] == 0x00)) {
            validUID = false;
        }
        if ((response[6] == 0xFF) && (response[7] == 0xFF) && (response[8] == 0xFF) && (response[9] == 0xFF)) {
            validUID = false;
        }
    }

    // If UID is valid, copy it to the buffer
    if (validUID) {
        for (int i = 0; i < uidLength; i++) {
            buffer[i] = response[i + 3];
        }
        return uidLength;
    } else {
        return 0;
    }
}
