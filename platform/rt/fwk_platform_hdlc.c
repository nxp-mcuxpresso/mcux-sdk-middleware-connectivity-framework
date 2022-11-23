/* -------------------------------------------------------------------------- */
/*                           Copyright 2021-2022 NXP                          */
/*                            All rights reserved.                            */
/*                    SPDX-License-Identifier: BSD-3-Clause                   */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                                  Includes                                  */
/* -------------------------------------------------------------------------- */

#include <stdint.h>

#include "fwk_platform_hdlc.h"
#include "fsl_component_serial_manager.h"
#include "board.h"

/* -------------------------------------------------------------------------- */
/*                               Private macros                               */
/* -------------------------------------------------------------------------- */

#ifndef SPINEL_UART_INSTANCE
#define SPINEL_UART_INSTANCE 3
#endif
#ifndef SPINEL_ENABLE_RX_RTS
#define SPINEL_ENABLE_RX_RTS 1
#endif
#ifndef SPINEL_ENABLE_TX_RTS
#define SPINEL_ENABLE_TX_RTS 1
#endif

#ifndef SPINEL_UART_BAUD_RATE
#define SPINEL_UART_BAUD_RATE 1000000
#endif

#ifndef SPINEL_UART_CLOCK_RATE
#define SPINEL_UART_CLOCK_RATE BOARD_BT_UART_CLK_FREQ
#endif

#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U) && (SPINEL_ENABLE_TX_RTS > 0U) && \
     (SPINEL_ENABLE_RX_RTS > 0U))
#error "DMA on UART with flow control not required"
#endif

#define LPUART_TX_DMA_CHANNEL 0U
#define LPUART_RX_DMA_CHANNEL 1U

#define SPINEL_HDLC_MALLOC pvPortMalloc
#define SPINEL_HDLC_FREE   vPortFree

/* -------------------------------------------------------------------------- */
/*                                Private types                               */
/* -------------------------------------------------------------------------- */

enum
{
    /* Ring buffer size should be >= to the RCP max TX buffer size value which is 2048 */
    kMaxRingBufferSize = 2048,
};

/* -------------------------------------------------------------------------- */
/*                               Private memory                               */
/* -------------------------------------------------------------------------- */

static SERIAL_MANAGER_HANDLE_DEFINE(otTransceiverSerialHandle);
static SERIAL_MANAGER_READ_HANDLE_DEFINE(otTransceiverSerialReadHandle);

static uint8_t s_ringBuffer[kMaxRingBufferSize];

static platform_hdlc_rx_callback_t hdlcRxCallback;
static void *                      callbackParam = NULL;

#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && (FSL_FEATURE_SOC_DMAMUX_COUNT > 0U))
static const dma_mux_configure_t dma_mux = {
    .dma_dmamux_configure =
        {
            .dma_mux_instance = 0,
            .rx_request       = kDmaRequestMuxLPUART8Rx,
            .tx_request       = kDmaRequestMuxLPUART8Tx,
        },
};
#endif
#endif

#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
#if (defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && (FSL_FEATURE_EDMA_HAS_CHANNEL_MUX > 0U))
static const dma_mux_configure_t dma_channel_mux = {
    .dma_dmamux_configure =
        {
            .dma_mux_instance   = 0,
            .dma_tx_channel_mux = kDmaRequestMuxLPUART8Rx,
            .dma_rx_channel_mux = kDmaRequestMuxLPUART8Tx,
        },
};
#endif
#endif

#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
static serial_port_uart_dma_config_t uartConfig = {
#else
static serial_port_uart_config_t uartConfig = {
#endif
    .baudRate        = SPINEL_UART_BAUD_RATE,
    .parityMode      = kSerialManager_UartParityDisabled,
    .stopBitCount    = kSerialManager_UartOneStopBit,
    .enableRx        = 1,
    .enableTx        = 1,
    .enableRxRTS     = SPINEL_ENABLE_RX_RTS,
    .enableTxCTS     = SPINEL_ENABLE_TX_RTS,
    .instance        = SPINEL_UART_INSTANCE,
    .txFifoWatermark = 0,
    .rxFifoWatermark = 0,
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
    .dma_instance = 0,
    .rx_channel   = LPUART_RX_DMA_CHANNEL,
    .tx_channel   = LPUART_TX_DMA_CHANNEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && (FSL_FEATURE_SOC_DMAMUX_COUNT > 0U))
    .dma_mux_configure = &dma_mux,
#else
    .dma_mux_configure         = NULL,
#endif
#if (defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && (FSL_FEATURE_EDMA_HAS_CHANNEL_MUX > 0U))
    .dma_channel_mux_configure = &dma_channel_mux,
#else
    .dma_channel_mux_configure = NULL,
#endif
#endif
};

static const serial_manager_config_t serialManagerConfig = {
    .ringBuffer     = &s_ringBuffer[0],
    .ringBufferSize = sizeof(s_ringBuffer),
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
    .type = kSerialPort_UartDma,
#else
    .type = kSerialPort_Uart,
#endif
    .blockType  = kSerialManager_NonBlocking,
    .portConfig = (serial_port_uart_config_t *)&uartConfig,
};

/* -------------------------------------------------------------------------- */
/*                             Private prototypes                             */
/* -------------------------------------------------------------------------- */

static int  PLATFORM_InitHdlcUart(void);
static int  PLATFORM_TerminateHdlcUart(void);
static void PLATFORM_HdlcSerialManagerTxCallback(void *                             pData,
                                                 serial_manager_callback_message_t *message,
                                                 serial_manager_status_t            status);
static void PLATFORM_HdlcSerialManagerRxCallback(void *                             pData,
                                                 serial_manager_callback_message_t *message,
                                                 serial_manager_status_t            status);

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

int PLATFORM_InitHdlcInterface(platform_hdlc_rx_callback_t callback, void *param)
{
    int ret = 0;

    hdlcRxCallback = callback;
    callbackParam  = param;

    /* Init UART interface */
    if (PLATFORM_InitHdlcUart() != 0)
    {
        hdlcRxCallback = NULL;
        callbackParam  = NULL;
        ret            = -1;
    }

    return ret;
}

int PLATFORM_TerminateHdlcInterface(void)
{
    int ret = 0;

    hdlcRxCallback = NULL;
    callbackParam  = NULL;

    if (PLATFORM_TerminateHdlcUart() != 0)
    {
        ret = -1;
    }

    return ret;
}

int PLATFORM_SendHdlcMessage(uint8_t *msg, uint32_t len)
{
    serial_manager_status_t serialManagerStatus;
    uint8_t *               pWriteHandleAndFrame = NULL;
    uint8_t *               pNewFrameBuffer      = NULL;
    int                     ret                  = 0;

    /* Disable IRQs to prevent concurrent accesses to class fields or
     * serial manager global variables that could be accessed in the
     * Write function
     */
    OSA_InterruptDisable();

    do
    {
        pWriteHandleAndFrame = (uint8_t *)SPINEL_HDLC_MALLOC(SERIAL_MANAGER_WRITE_HANDLE_SIZE + len);
        if (pWriteHandleAndFrame == NULL)
        {
            ret = -1;
            break;
        }

        pNewFrameBuffer = pWriteHandleAndFrame + SERIAL_MANAGER_WRITE_HANDLE_SIZE;
        memcpy(pNewFrameBuffer, msg, len);

        serialManagerStatus = SerialManager_OpenWriteHandle((serial_handle_t)otTransceiverSerialHandle,
                                                            (serial_write_handle_t)pWriteHandleAndFrame);
        if (serialManagerStatus != kStatus_SerialManager_Success)
        {
            ret = -2;
            break;
        }

        serialManagerStatus = SerialManager_InstallTxCallback(
            (serial_write_handle_t)pWriteHandleAndFrame, PLATFORM_HdlcSerialManagerTxCallback, pWriteHandleAndFrame);
        if (serialManagerStatus != kStatus_SerialManager_Success)
        {
            ret = -3;
            break;
        }

        serialManagerStatus =
            SerialManager_WriteNonBlocking((serial_write_handle_t)pWriteHandleAndFrame, pNewFrameBuffer, len);
        if (serialManagerStatus != kStatus_SerialManager_Success)
        {
            ret = -4;
            break;
        }
    } while (0);

    if (ret != 0)
    {
        SPINEL_HDLC_FREE(pWriteHandleAndFrame);
    }

    OSA_InterruptEnable();

    return ret;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

static int PLATFORM_InitHdlcUart(void)
{
    serial_manager_status_t status;
    int                     ret = 0;

    /*
     * Make sure to disable interrupts while initializating the serial manager interface
     * Some issues could happen a UART IRQ is fired during serial manager initialization
     */
    OSA_InterruptDisable();
    do
    {
        /* Retrieve the UART clock rate at runtime as it can depend on clock config */
        uartConfig.clockRate = SPINEL_UART_CLOCK_RATE;

        status = SerialManager_Init((serial_handle_t)otTransceiverSerialHandle, &serialManagerConfig);
        if (status != kStatus_SerialManager_Success)
        {
            ret = -1;
            break;
        }

        status = SerialManager_OpenReadHandle((serial_handle_t)otTransceiverSerialHandle,
                                              (serial_read_handle_t)otTransceiverSerialReadHandle);
        if (status != kStatus_SerialManager_Success)
        {
            ret = -2;
            break;
        }

        status = SerialManager_InstallRxCallback((serial_read_handle_t)otTransceiverSerialReadHandle,
                                                 PLATFORM_HdlcSerialManagerRxCallback, NULL);
        if (status != kStatus_SerialManager_Success)
        {
            ret = -3;
            break;
        }
    } while (0);
    OSA_InterruptEnable();
    assert(status == kStatus_SerialManager_Success);

    return ret;
}

static int PLATFORM_TerminateHdlcUart(void)
{
    serial_manager_status_t status;
    int                     ret = 0;

    do
    {
        status = SerialManager_CloseReadHandle((serial_read_handle_t)otTransceiverSerialReadHandle);
        if (status != kStatus_SerialManager_Success)
        {
            ret = -1;
            break;
        }

        status = SerialManager_Deinit((serial_handle_t)otTransceiverSerialHandle);
        if (status != kStatus_SerialManager_Success)
        {
            ret = -2;
            break;
        }
    } while (0);

    return ret;
}

static void PLATFORM_HdlcSerialManagerTxCallback(void *                             pData,
                                                 serial_manager_callback_message_t *message,
                                                 serial_manager_status_t            status)
{
    OSA_InterruptDisable();
    /* Close the write handle */
    SerialManager_CloseWriteHandle((serial_write_handle_t)pData);
    OSA_InterruptEnable();
    /* Free the buffer */
    SPINEL_HDLC_FREE(pData);
}

static void PLATFORM_HdlcSerialManagerRxCallback(void *                             param,
                                                 serial_manager_callback_message_t *message,
                                                 serial_manager_status_t            status)
{
    uint8_t                 mUartRxBuffer[256];
    uint32_t                bytesRead = 0U;
    serial_manager_status_t smStatus;

    do
    {
        OSA_InterruptDisable();
        smStatus = SerialManager_TryRead((serial_read_handle_t)otTransceiverSerialReadHandle, mUartRxBuffer,
                                         sizeof(mUartRxBuffer), &bytesRead);
        OSA_InterruptEnable();

        if ((hdlcRxCallback != NULL) && (bytesRead > 0U) && (smStatus == kStatus_SerialManager_Success))
        {
            hdlcRxCallback(mUartRxBuffer, bytesRead, callbackParam);
        }
    } while (bytesRead != 0U);
}
