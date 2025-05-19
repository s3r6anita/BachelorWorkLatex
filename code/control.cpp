/*
 * WRITE _REGISTER_AND_MASK - 0x02
 * This command modifies the content of a register using a logical AND operation. The
 * content of the register is read and a logical AND operation is performed with the provided
 * mask. The modified content is written back to the register.
 * The address of the register must exist. If the condition is not fulfilled, an exception is
 * raised.
 */
bool PN5180::writeRegisterWithAndMask(uint8_t reg, uint32_t mask) {
  PN5180DEBUG_PRINTF(F("PN5180::writeRegisterWithAndMask(reg=%d, mask=%d)"), reg, mask);
  PN5180DEBUG_PRINTLN();
  uint8_t *p = (uint8_t*)&mask;
  uint8_t cmd[] = { PN5180_WRITE_REGISTER_AND_MASK, reg, p[0], p[1], p[2], p[3] };
  return transceiveCommand(cmd, sizeof(cmd));
}

/** data transmit from PN5180 by SPI */
bool PN5180::transceiveCommand(uint8_t *sendBuffer, size_t sendBufferLen, uint8_t *recvBuffer, size_t recvBufferLen) {
    PN5180DEBUG_PRINTF("PN5180::transceiveCommand(*sendBuffer, sendBufferLen=%d, *recvBuffer, recvBufferLen=%d)\n", sendBufferLen, recvBufferLen);

    // 0. waiting BUSY low
    unsigned long startedWaiting = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOA, PN5180_BUSY) != GPIO_PIN_RESET) {
        if (HAL_GetTick() - startedWaiting > commandTimeout) {
            PN5180DEBUG("*** ERROR: transceiveCommand timeout (send/0)\n");
            HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
            return false;
        }
    }

    // 1. activate NSS (=0)
    HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_RESET);
    HAL_Delay(1);

    // 2. send data by SPI
    if (HAL_SPI_Transmit(&hspi1, sendBuffer, sendBufferLen, HAL_MAX_DELAY) != HAL_OK) {
        PN5180DEBUG("*** ERROR: SPI transmit failed\n");
        HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
        return false;
    }

    // 3. wait BUSY=1
    startedWaiting = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOA, PN5180_BUSY) != GPIO_PIN_SET) {
        if (HAL_GetTick() - startedWaiting > commandTimeout) {
            PN5180DEBUG("*** ERROR: transceiveCommand timeout (send/3)\n");
            HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
            return false;
        }
    }

    // 4. disable NSS (=1)
    HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET);
    HAL_Delay(1);

    // 5. wait BUSY=0
    startedWaiting = HAL_GetTick();

    while (HAL_GPIO_ReadPin(GPIOA, PN5180_BUSY) != GPIO_PIN_RESET) {
        if (HAL_GetTick() - startedWaiting > commandTimeout) {
            PN5180DEBUG("*** ERROR: transceiveCommand timeout (send/5)\n");
            HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
            return false;
        }
    }

    // check need receive data
    if ((recvBuffer == nullptr) || (recvBufferLen == 0)) {
      return true;
    }

    PN5180DEBUG_PRINTLN("Receiving SPI frame...");

    // 1. activate NSS (=0)
    HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_RESET);

    // 2. prepare buffer for data get
    memset(recvBuffer, 0xFF, recvBufferLen);

    // 3. receive data by SPI
    if (HAL_SPI_Receive(&hspi1, recvBuffer, recvBufferLen, HAL_MAX_DELAY) != HAL_OK) {
        PN5180DEBUG("*** ERROR: SPI receive failed\n");
        HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
        return false;
    }

    // 4. wait BUSY=1
    startedWaiting = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOA, PN5180_BUSY) != GPIO_PIN_SET) {
        if (HAL_GetTick() - startedWaiting > commandTimeout) {
            PN5180DEBUG("*** ERROR: transceiveCommand timeout (receive/4)\n");
            HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET); // disable NSS
            return false;
        }
    }

    // 5. disable NSS (=1)
    HAL_GPIO_WritePin(GPIOA, PN5180_NSS, GPIO_PIN_SET);

    // 6. wait BUSY=0
    startedWaiting = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOA, PN5180_BUSY) != GPIO_PIN_RESET) {
        if (HAL_GetTick() - startedWaiting > commandTimeout) {
            PN5180DEBUG("*** ERROR: transceiveCommand timeout (receive/6)\n");
            return false;
        }
    }

    return true;
}
