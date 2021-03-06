/** @file wlan_scan.h
 *
 *  @brief Interface for the wlan network scan routines
 *
 *  Driver interface functions and type declarations for the scan module
 *    implemented in wlan_scan.c.
 *
 *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2006
 *
 *  @sa wlan_scan.c
 */
/*************************************************************
Change Log:
    01/11/06: Initial revision. New scan code, relocate related functions

************************************************************/

#ifndef _WLAN_SCAN_H
#define _WLAN_SCAN_H

/**
 *  @brief Maximum number of channels that can be sent in a setuserscan ioctl
 *
 *  @sa wlan_scan_cfg
 */
#define RT_WLAN_SCAN_CHAN_MAX  14

//! Infrastructure BSS scan type in wlan_scan_cmd_config
#define WLAN_SCAN_BSS_TYPE_BSS         1

//! Adhoc BSS scan type in wlan_scan_cmd_config
#define WLAN_SCAN_BSS_TYPE_IBSS        2

//! Adhoc or Infrastructure BSS scan type in wlan_scan_cmd_config, no filter
#define WLAN_SCAN_BSS_TYPE_ANY         3

/**
 * @brief Buffer pad space for newly allocated beacons/probe responses
 *
 * Beacons are typically 6 bytes longer than an equivalent probe response.
 *  For each scan response stored, allocate an extra byte pad at the end to
 *  allow easy expansion to store a beacon in the same memory a probe reponse
 *  previously contained
 */
#define SCAN_BEACON_ENTRY_PAD          6

//! Scan time specified in the channel TLV for each channel for passive scans
#define MRVDRV_PASSIVE_SCAN_CHAN_TIME  100

//! Scan time specified in the channel TLV for each channel for active scans
#define MRVDRV_ACTIVE_SCAN_CHAN_TIME   100

//! Scan time specified in the channel TLV for each channel for specific scans
#define MRVDRV_SPECIFIC_SCAN_CHAN_TIME 100

//! Max passive scan time for each channel in milliseconds
#define MRVDRV_MAX_PASSIVE_SCAN_CHAN_TIME   2000

//! Max active scan time for each channel in milliseconds 
#define MRVDRV_MAX_ACTIVE_SCAN_CHAN_TIME    500

/**
 * Max total scan time in milliseconds
 * The total scan time should be less than scan command timeout value (10s)
 */
#define MRVDRV_MAX_TOTAL_SCAN_TIME     (MRVDRV_TIMER_10S - MRVDRV_TIMER_1S)

typedef struct
{
    /**
     *  @brief BSS Type to be sent in the firmware command
     *
     *  Field can be used to restrict the types of networks returned in the
     *    scan.  Valid settings are:
     *
     *   - WLAN_SCAN_BSS_TYPE_BSS  (infrastructure)
     *   - WLAN_SCAN_BSS_TYPE_IBSS (adhoc)
     *   - WLAN_SCAN_BSS_TYPE_ANY  (unrestricted, adhoc and infrastructure)
     */
    u8 bssType;

    /**
     *  @brief Specific BSSID used to filter scan results in the firmware
     */
    u8 specificBSSID[MRVDRV_ETH_ADDR_LEN];

    /**
     *  @brief Length of TLVs sent in command starting at tlvBuffer
     */
    int tlvBufferLen;

    /**
     *  @brief SSID TLV(s) and ChanList TLVs to be sent in the firmware command
     *
     *  @sa TLV_TYPE_CHANLIST, MrvlIEtypes_ChanListParamSet_t
     *  @sa TLV_TYPE_SSID, MrvlIEtypes_SsIdParamSet_t
     */
    u8 tlvBuffer[1];            //!< SSID TLV(s) and ChanList TLVs are stored here
} wlan_scan_cmd_config;

/**
 *  @brief IOCTL channel sub-structure sent in wlan_scan_cfg
 *
 *  Multiple instances of this structure are included in the IOCTL command
 *   to configure a instance of a scan on the specific channel.
 */
typedef struct
{
    u8 chanNumber;              //!< Channel Number to scan
    u8 radioType;               //!< Radio type: 'B/G' Band = 0, 'A' Band = 1
    u8 scanType;                //!< Scan type: Active = 0, Passive = 1
    u8 validflag;
    u16 scanTime;               //!< Scan duration in milliseconds; if 0 default used
}  wlan_scan_chan;

/**
 *  @brief IOCTL SSID List sub-structure sent in wlan_scan_cfg
 * 
 *  Used to specify SSID specific filters as well as SSID pattern matching
 *    filters for scan result processing in firmware.
 */
typedef struct
{
    char ssid[MRVDRV_MAX_SSID_LENGTH + 1];
    u8 maxLen;
}  wlan_scan_ssid;

/**
 *  @brief IOCTL input structure to configure an immediate scan cmd to firmware
 *
 *  Used in the setuserscan (WLAN_SET_USER_SCAN) private ioctl.  Specifies
 *   a number of parameters to be used in general for the scan as well
 *   as a channel list (wlan_scan_chan) for each scan period
 *   desired.
 *
 *  @sa wlan_set_user_scan_ioctl
 */
typedef struct
{
    /*   - WLAN_SCAN_BSS_TYPE_BSS  (infrastructure)
     *   - WLAN_SCAN_BSS_TYPE_IBSS (adhoc)
     *   - WLAN_SCAN_BSS_TYPE_ANY  (unrestricted, adhoc and infrastructure)
     */
    u8 bssType;

    /**
     *  @brief Configure the number of probe requests for active chan scans
     */
    u8 numProbes;

    /**
     *  @brief BSSID filter sent in the firmware command to limit the results
     */
    u8 specificBSSID[MRVDRV_ETH_ADDR_LEN];

    /**
     *  @brief SSID filter list used in the to limit the scan results
     */
    wlan_scan_ssid ssidList;

    /**
     *  @brief Variable number (fixed maximum) of channels to scan up
     */
    wlan_scan_chan chanList;

}  wlan_scan_cfg;

/**
 *  @brief Sub-structure passed in wlan_ioctl_get_scan_table_entry for each BSS
 *
 *  Fixed field information returned for the scan response in the IOCTL
 *    response.
 */
typedef struct
{
    u8 bssid[6];                //!< BSSID of this network
    u8 channel;                 //!< Channel this beacon/probe response was detected
    u8 rssi;                    //!< RSSI for the received packet
    u8 networkTSF[8];             //!< TSF value from the firmware at packet reception
}  wlan_ioctl_get_scan_table_fixed;

/**
 *  @brief Structure passed in the wlan_ioctl_get_scan_table_info for each
 *         BSS returned in the WLAN_GET_SCAN_RESP IOCTL
 *
 *  @sa wlan_get_scan_table_ioctl
 */
typedef struct
{

    /**
     *  @brief Fixed field length included in the response.
     *
     *  Length value is included so future fixed fields can be added to the
     *   response without breaking backwards compatibility.  Use the length
     *   to find the offset for the bssInfoLength field, not a sizeof() calc.
     */
    u32 fixedFieldLength;

    /**
     *  @brief Always present, fixed length data fields for the BSS
     */
    wlan_ioctl_get_scan_table_fixed fixedFields;

    /**
     *  @brief Length of the BSS Information (probe resp or beacon) that
     *         follows starting at bssInfoBuffer
     */
    u32 bssInfoLength;

    /**
     *  @brief Probe response or beacon scanned for the BSS.
     *
     *  Field layout:
     *   - TSF              8 octets
     *   - Beacon Interval  2 octets
     *   - Capability Info  2 octets
     *
     *   - IEEE Infomation Elements; variable number & length per 802.11 spec
     */
    u8 bssInfoBuffer[1];
}wlan_ioctl_get_scan_table_entry;

/**
 *  @brief WLAN_GET_SCAN_RESP private IOCTL struct to retrieve the scan table
 *
 *  @sa wlan_get_scan_table_ioctl
 */
typedef struct
{

    /**
     *  - Zero based scan entry to start retrieval in command request
     *  - Number of scans entires returned in command response
     */
    u32 scanNumber;

     /**
      * Buffer marker for multiple wlan_ioctl_get_scan_table_entry structures.
      *   Each struct is padded to the nearest 32 bit boundary.
      */
    u8 scan_table_entry_buffer[1];

} wlan_ioctl_get_scan_table_info;

/**
 *  @brief Structure used to store information for each beacon/probe response
 */
typedef struct
{
    WLAN_802_11_MAC_ADDRESS MacAddress;

    WLAN_802_11_SSID Ssid;

    /* WEP encryption requirement */
    u32 Privacy;

    /* receive signal strength in dBm */
    WLAN_802_11_RSSI Rssi;

    u32 Channel;

    u16 BeaconPeriod;

    u32 ATIMWindow;
    u8 ERPFlags;

    WLAN_802_11_NETWORK_TYPE NetworkTypeInUse;
    WLAN_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
    WLAN_802_11_RATES SupportedRates;
    IEEEtypes_WmmParameter_t wmmIE;

    int extra_ie;

    u8 TimeStamp[8];            //!< TSF value included in the beacon/probe response

    IEEEtypes_PhyParamSet_t PhyParamSet;
    IEEEtypes_SsParamSet_t SsParamSet;
    IEEEtypes_CapInfo_t Cap;

    u8 DataRates[WLAN_SUPPORTED_RATES];

    u8 networkTSF[8];             //!< TSF timestamp from the current firmware TSF
 
    IEEEtypes_CountryInfoFullSet_t CountryInfo;

    IEEEtypes_VendorSpecific_t wpaIE;
    IEEEtypes_Generic_t rsnIE;

//    IEEEtypes_VendorSpecific_t wpsIE;

    u8 *pBeaconBuf;             //!< Pointer to the returned scan response
    u32 beaconBufSize;         //!< Length of the stored scan response		  //dennis
    u32 beaconBufSizeMax;      //!< Max allocated size for updated scan response //dennis

} BSSDescriptor_t;

typedef enum
{
	NoSecurity = 0,
	WEP,
	WPA_PSK,
	WPA2_PSK,
	UnSupported
} SecurityMode;

typedef struct _WlanConfig
{
	u8 password[64];
	u8 SSID[64];
	u8 MacAddr[6];
	SecurityMode security;

	u8 channel;

	WLAN_802_11_RATES SupportedRates;
}WlanConfig;

/**
 *  @brief Structure used to store information for each beacon/probe response
 */
typedef struct
{
    WLAN_802_11_MAC_ADDRESS MacAddress;

    WLAN_802_11_SSID Ssid;
    /* WEP encryption requirement */
    u32 Privacy;
	
    /* receive signal strength in dBm */
    WLAN_802_11_RSSI Rssi;
	
    u32 Channel;
	
    WLAN_802_11_NETWORK_TYPE NetworkTypeInUse;
	
    WLAN_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	
    WLAN_802_11_RATES SupportedRates;
	
    u8 DataRates[WLAN_SUPPORTED_RATES];
    SecurityMode Security;
} ScanResultItem;
#endif /* _WLAN_SCAN_H */
