/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * File - status_strings.c
 *
 * Library for stringizing the status returned by WinDriver API.
 */

#if defined (__KERNEL_DRIVER__)
    #include "kd.h"
#elif defined (__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
#endif
#include "status_strings.h"

typedef struct
{
    WD_ERROR_CODES dwStatus;
    const char *sErrDesc;
} STATUS_STRINGS, *PSTATUS_STRINGS;

#define ERROR_CODE_NOT_FOUND "Unrecognized error code"

STATUS_STRINGS statusStrings[] = 
{
    {WD_STATUS_SUCCESS, "Success"},
    {WD_STATUS_INVALID_WD_HANDLE, "Invalid WinDriver handle"},
    
    /* The following statuses are returned by WinDriver: */
    {WD_WINDRIVER_STATUS_ERROR, "Error"},

    {WD_INVALID_HANDLE, "Invalid handle"},
    {WD_INVALID_PIPE_NUMBER, "Invalid pipe number"},
    {WD_READ_WRITE_CONFLICT, "Conflict between read and write operations"},
        /*
         * Request to read from an OUT (write) pipe or request to write to an
         * IN (read) pipe
         */
    {WD_ZERO_PACKET_SIZE, "Packet size is zero"},
    {WD_INSUFFICIENT_RESOURCES, "Insufficient resources"},
    {WD_UNKNOWN_PIPE_TYPE, "Unknown pipe type"},
    {WD_SYSTEM_INTERNAL_ERROR, "Internal system error"},
    {WD_DATA_MISMATCH, "Data mismatch"},
    {WD_NO_LICENSE, "No valid license"},
    {WD_NOT_IMPLEMENTED, "Function not implemented"},
    {WD_KERPLUG_FAILURE, "Kernel PlugIn failure"},
    {WD_FAILED_ENABLING_INTERRUPT, "Failed enabling interrupt"},
    {WD_INTERRUPT_NOT_ENABLED, "Interrupt not enabled"},
    {WD_RESOURCE_OVERLAP, "Resource overlap"},
    {WD_DEVICE_NOT_FOUND, "Device not found"},
    {WD_WRONG_UNIQUE_ID, "Wrong unique ID"},
    {WD_OPERATION_ALREADY_DONE, "Operation already done"},
    {WD_USB_DESCRIPTOR_ERROR, "Usb descriptor error"},
    {WD_SET_CONFIGURATION_FAILED, "Set configuration operation failed"},
    {WD_CANT_OBTAIN_PDO, "Cannot obtain PDO"},
    {WD_TIME_OUT_EXPIRED, "Timeout expired"},
    {WD_IRP_CANCELED, "IRP operation cancelled"},
    {WD_FAILED_USER_MAPPING, "Failed to map in user space"},
    {WD_FAILED_KERNEL_MAPPING, "Failed to map in kernel space"},
    {WD_NO_RESOURCES_ON_DEVICE, "No resources on the device"},
    {WD_NO_EVENTS, "No events"},
    {WD_INVALID_PARAMETER, "Invalid parameter"},
    {WD_INCORRECT_VERSION, "Incorrect WinDriver version installed"},
    {WD_TRY_AGAIN, "Try again"},
    {WD_WINDRIVER_NOT_FOUND, "Failed open WinDriver"}, 
    {WD_INVALID_IOCTL, "Received an invalid IOCTL"}, 
    {WD_OPERATION_FAILED, "Operation failed"},
    {WD_INVALID_32BIT_APP, "Received an invalid 32bit IOCTL"},
    {WD_TOO_MANY_HANDLES, "No room to add handle"},
    {WD_NO_DEVICE_OBJECT, "Driver not installed"},
    
    /*
     * The following status codes are returned by USBD:
     * USBD status types:
     */
    {WD_USBD_STATUS_SUCCESS, "USBD: Success"},
    {WD_USBD_STATUS_PENDING, "USBD: Operation pending"},
    {WD_USBD_STATUS_ERROR, "USBD: Error"},
    {WD_USBD_STATUS_HALTED, "USBD: Halted"},

    /*
     * USBD status codes:
     * NOTE: The following status codes are comprised of one of the status 
     * types above and an error code [i.e. 0xXYYYYYYYL - where: X = status type;
     * YYYYYYY = error code].
     * The same error codes may also appear with one of the other status types 
     * as well.
     *
     * HC (Host Controller) status codes.
     * [NOTE: These status codes use the WD_USBD_STATUS_HALTED status type]:
     */
    {WD_USBD_STATUS_CRC, "HC status: CRC"},
    {WD_USBD_STATUS_BTSTUFF, "HC status: Bit stuffing "},
    {WD_USBD_STATUS_DATA_TOGGLE_MISMATCH, "HC status: Data toggle mismatch"},
    {WD_USBD_STATUS_STALL_PID, "HC status: PID stall"},
    {WD_USBD_STATUS_DEV_NOT_RESPONDING, "HC status: Device not responding"},
    {WD_USBD_STATUS_PID_CHECK_FAILURE, "HC status: PID check failed"},
    {WD_USBD_STATUS_UNEXPECTED_PID, "HC status: Unexpected PID"},
    {WD_USBD_STATUS_DATA_OVERRUN, "HC status: Data overrun"},
    {WD_USBD_STATUS_DATA_UNDERRUN, "HC status: Data underrun"},
    {WD_USBD_STATUS_RESERVED1, "HC status: Reserved1"},
    {WD_USBD_STATUS_RESERVED2, "HC status: Reserved2"},
    {WD_USBD_STATUS_BUFFER_OVERRUN, "HC status: Buffer overrun"},
    {WD_USBD_STATUS_BUFFER_UNDERRUN, "HC status: Buffer Underrun"},
    {WD_USBD_STATUS_NOT_ACCESSED, "HC status: Not accessed"},
    {WD_USBD_STATUS_FIFO, "HC status: Fifo"},

#if defined(WIN32)
    {WD_USBD_STATUS_XACT_ERROR, "HC status: The host controller has set the "
        "Transaction Error (XactErr) bit in the transfer descriptor's status "
        "field"},
    {WD_USBD_STATUS_BABBLE_DETECTED, "HC status: Babble detected"},
    {WD_USBD_STATUS_DATA_BUFFER_ERROR, "HC status: Data buffer error"},
#endif

#if defined(WINCE)
    {WD_USBD_STATUS_ISOCH, "USBD: Isochronous transfer failed"},
    {WD_USBD_STATUS_NOT_COMPLETE, "USBD: Transfer not completed"},
    {WD_USBD_STATUS_CLIENT_BUFFER, "USBD: Cannot write to buffer"},
#endif
    {WD_USBD_STATUS_CANCELED, "USBD: Transfer canceled"},

    /*
     * Returned by HCD (Host Controller Driver) if a transfer is submitted to 
     * an endpoint that is stalled:
     */
    {WD_USBD_STATUS_ENDPOINT_HALTED,
        "HCD: Transfer submitted to a stalled endpoint"},

    /*
     * Software status codes
     * [NOTE: The following status codes have only the error bit set]:
     */
    {WD_USBD_STATUS_NO_MEMORY, "USBD: Out of memory"},
    {WD_USBD_STATUS_INVALID_URB_FUNCTION, "USBD: Invalid URB function"},
    {WD_USBD_STATUS_INVALID_PARAMETER, "USBD: Invalid parameter"},

    /*
     * Returned if client driver attempts to close an endpoint/interface
     * or configuration with outstanding transfers:
     */
    {WD_USBD_STATUS_ERROR_BUSY, "USBD: Attempted to close "
        "enpoint/interface/configuration with outstanding transfer"},

    /*
     * Returned by USBD if it cannot complete a URB request. Typically this
     * will be returned in the URB status field when the Irp is completed
     * with a more specific error code. [The Irp status codes are indicated in
     * WinDriver's Debug Monitor tool (wddebug/wddebug_gui):
     */
    {WD_USBD_STATUS_REQUEST_FAILED, "USBD: URB request failed"},

    {WD_USBD_STATUS_INVALID_PIPE_HANDLE, "USBD: Invalid pipe handle"},

    /*
     * Returned when there is not enough bandwidth available
     * to open a requested endpoint:
     */
    {WD_USBD_STATUS_NO_BANDWIDTH, "USBD: Not enough bandwidth for endpoint"},

    /* Generic HC (Host Controller) error: */
    {WD_USBD_STATUS_INTERNAL_HC_ERROR, "USBD: Host controller error"},

    /*
     * Returned when a short packet terminates the transfer
     * i.e. USBD_SHORT_TRANSFER_OK bit not set:
     */
    {WD_USBD_STATUS_ERROR_SHORT_TRANSFER,
        "USBD: Trasnfer terminated with short packet"},

    /*
     * Returned if the requested start frame is not within
     * USBD_ISO_START_FRAME_RANGE of the current USB frame,
     * NOTE: The stall bit is set:
     */
    {WD_USBD_STATUS_BAD_START_FRAME, "USBD: Start frame outside range"},

    /*
     * Returned by HCD (Host Controller Driver) if all packets in an 
     * isochronous transfer complete with an error:
     */
    {WD_USBD_STATUS_ISOCH_REQUEST_FAILED,
        "HCD: Isochronous transfer completed with error"},

    /*
     * Returned by USBD if the frame length control for a given
     * HC (Host Controller) is already taken by another driver:
     */
    {WD_USBD_STATUS_FRAME_CONTROL_OWNED,
        "USBD: Frame length control already taken"},

    /*
     * Returned by USBD if the caller does not own frame length control and
     * attempts to release or modify the HC frame length:
     */
    {WD_USBD_STATUS_FRAME_CONTROL_NOT_OWNED,
        "USBD: Attemped operation on frame length control not owned by caller"},
#if defined(WIN32)
    /* Additional USB 2.0 software error codes added for USB 2.0: */
    {WD_USBD_STATUS_NOT_SUPPORTED, "USBD: API not supported/implemented"},
    {WD_USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR, "USBD: Invalid " 
        "configuration descriptor"},
    {WD_USBD_STATUS_INSUFFICIENT_RESOURCES, "USBD: Insufficient resources"},
    {WD_USBD_STATUS_SET_CONFIG_FAILED, "USBD: Set configuration failed"},
    {WD_USBD_STATUS_BUFFER_TOO_SMALL, "USBD: Buffer too small"},
    {WD_USBD_STATUS_INTERFACE_NOT_FOUND, "USBD: Interface not found"},
    {WD_USBD_STATUS_INAVLID_PIPE_FLAGS, "USBD: Invalid pipe flags"},
    {WD_USBD_STATUS_TIMEOUT, "USBD: Timeout"},
    {WD_USBD_STATUS_DEVICE_GONE, "USBD: Device gone"},
    {WD_USBD_STATUS_STATUS_NOT_MAPPED, "USBD: Status not mapped"},

    /*
     * Extended isochronous error codes returned by USBD.
     * These errors appear in the packet status field of an isochronous 
     * transfer.
     * 
     * For some reason the controller did not access the TD associated with 
     * this packet:
     */
    {WD_USBD_STATUS_ISO_NOT_ACCESSED_BY_HW, "USBD: The controller did not "
        "access the TD associated with this packe"},
    /*
     * Controller reported an error in the TD. 
     * Since TD errors are controller specific they are reported 
     * generically with this error code:
     */
    {WD_USBD_STATUS_ISO_TD_ERROR,
        "USBD: Controller reported an error in the TD"},

    /*
     * The packet was submitted in time by the client but 
     * failed to reach the miniport in time:
     */
    {WD_USBD_STATUS_ISO_NA_LATE_USBPORT, "USBD: The packet was submitted in "
        "time by the client but failed to reach the miniport in time"},
    
    /*
     * The packet was not sent because the client submitted it too late 
     * to transmit:
     */
    {WD_USBD_STATUS_ISO_NOT_ACCESSED_LATE, "USBD: The packet was not sent "
        "because the client submitted it too late to transmit"},
#endif
};

const char * DLLCALLCONV Stat2Str(DWORD dwStatus) 
{
    DWORD i;

    for (i=0; i<sizeof(statusStrings)/sizeof(STATUS_STRINGS); i++)
    {
        if (statusStrings[i].dwStatus==(WD_ERROR_CODES)dwStatus)
            return statusStrings[i].sErrDesc;
    }
    return ERROR_CODE_NOT_FOUND;
}

