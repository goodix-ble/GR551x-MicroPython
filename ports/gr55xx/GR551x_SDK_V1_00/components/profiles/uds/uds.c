/**
 *****************************************************************************************
 *
 * @file uds.c
 *
 * @brief The implementation of User Data Service.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/* 
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "uds.h"
#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/* The octets of attributes mask. */
#ifndef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
#define CHARS_MASK_OCTS                     8
#else
#define CHARS_MASK_OCTS                     9
#endif
/** UDS spec table 3.4 */
#define UDS_USER_CTRL_POINT_VAL_MAX_LEN     19
/** For both Date of Birth and Date of Threshold Assessment */
#define UDS_DATE_VAL_MAX_LEN                4

#define MANDATORY_ATTRS_MASK    0x00000000000000FF
                           /**< Mandatory attributes to be added into ATT database.
                            * bit0 - User Data Service declaration.
                            * bit1 - User Index declaration.
                            * bit2 - User Index value.
                            * bit3 - User Control Point declaration.
                            * bit4 - User Control Point value.
                            * bit5 - User Control Point CCCD.
                            * bit6 - Database Change Increment declaration.
                            * bit7 - Database Change Increment value. */
#define DB_CHANGE_CCCD_MASK     0x00000100
                            /**< Bit mask for Database Change Increment CCCD
                             * which is mandatory if the Server supports the
                             * update of one or more UDS Characteristic values. */
/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief UDS Attributes database index list.
 * @note This should have the same order as uds_char_mask_t. */
enum uds_attr_idx_tag
{
    UDS_IDX_SVC,                                /* 0 */

    /* Mandatory characteristics */
    UDS_IDX_USER_INDEX_CHAR,                    /* 1 */
    UDS_IDX_USER_INDEX_VAL,                     /* 2 */
    UDS_IDX_USER_CTRL_POINT_CHAR,               /* 3 */
    UDS_IDX_USER_CTRL_POINT_VAL,                /* 4 */
    UDS_IDX_USER_CTRL_POINT_CCCD,               /* 5 */
    UDS_IDX_DB_CHANGE_INCREMENT_CHAR,           /* 6 */
    UDS_IDX_DB_CHANGE_INCREMENT_VAL,            /* 7 */

    /* At least one UDS Characteristic shall be exposed. */
    UDS_IDX_FIRST_NAME_CHAR,                    /* 8 */
    UDS_IDX_FIRST_NAME_VAL,                     /* 9 */
    UDS_IDX_LAST_NAME_CHAR,                     /* 10 */
    UDS_IDX_LAST_NAME_VAL,                      /* 11 */
    UDS_IDX_EMAIL_ADDR_CHAR,                    /* 12 */
    UDS_IDX_EMAIL_ADDR_VAL,                     /* 13 */
    UDS_IDX_AGE_CHAR,                           /* 14 */
    UDS_IDX_AGE_VAL,                            /* 15 */

    UDS_IDX_DATE_OF_BIRTH_CHAR,                 /* 16 */
    UDS_IDX_DATE_OF_BIRTH_VAL,                  /* 17 */
    UDS_IDX_GENDER_CHAR,                        /* 18 */
    UDS_IDX_GENDER_VAL,                         /* 19 */
    UDS_IDX_WEIGHT_CHAR,                        /* 20 */
    UDS_IDX_WEIGHT_VAL,                         /* 21 */
    UDS_IDX_HEIGHT_CHAR,                        /* 22 */
    UDS_IDX_HEIGHT_VAL,                         /* 23 */

    UDS_IDX_VO2_MAX_CHAR,                       /* 24 */
    UDS_IDX_VO2_MAX_VAL,                        /* 25 */
    UDS_IDX_HEART_RATE_MAX_CHAR,                /* 26 */
    UDS_IDX_HEART_RATE_MAX_VAL,                 /* 27 */
    UDS_IDX_RESTING_HEART_RATE_CHAR,            /* 28 */
    UDS_IDX_RESTING_HEART_RATE_VAL,             /* 29 */
    UDS_IDX_MAX_RECOMMENDED_HEART_RATE_CHAR,    /* 30 */
    UDS_IDX_MAX_RECOMMENDED_HEART_RATE_VAL,     /* 31 */

    UDS_IDX_AEROBIC_THRESHOLD_CHAR,             /* 32 */
    UDS_IDX_AEROBIC_THRESHOLD_VAL,              /* 33 */
    UDS_IDX_ANAEROBIC_THRESHOLD_CHAR,           /* 34 */
    UDS_IDX_ANAEROBIC_THRESHOLD_VAL,            /* 35 */
    UDS_IDX_SPORT_TYPE_CHAR,                    /* 36 */
    UDS_IDX_SPORT_TYPE_VAL,                     /* 37 */
    UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_CHAR,  /* 38 */
    UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_VAL,   /* 39 */

    UDS_IDX_WAIST_CIRCUMFERENCE_CHAR,           /* 40 */
    UDS_IDX_WAIST_CIRCUMFERENCE_VAL,            /* 41 */
    UDS_IDX_HIP_CIRCUMFERENCE_CHAR,             /* 42 */
    UDS_IDX_HIP_CIRCUMFERENCE_VAL,              /* 43 */
    UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_CHAR, /* 44 */
    UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL,  /* 45 */
    UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_CHAR,  /* 46 */
    UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_VAL,   /* 47 */

    UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_CHAR,  /* 48 */
    UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_VAL,   /* 49 */
    UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_CHAR,   /* 50 */
    UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_VAL,    /* 51 */
    UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_CHAR,/* 52 */
    UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL, /* 53 */
    UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_CHAR, /* 54 */
    UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL,  /* 55 */

    UDS_IDX_FIVE_ZONE_HEART_RATE_LIMITS_CHAR,   /* 56 */
    UDS_IDX_FIVE_ZONE_HEART_RATE_LIMITS_VAL,    /* 57 */
    UDS_IDX_THREE_ZONE_HEART_RATE_LIMITS_CHAR,  /* 58 */
    UDS_IDX_THREE_ZONE_HEART_RATE_LIMITS_VAL,   /* 59 */
    UDS_IDX_TWO_ZONE_HEART_RATE_LIMIT_CHAR,     /* 60 */
    UDS_IDX_TWO_ZONE_HEART_RATE_LIMIT_VAL,      /* 61 */
    UDS_IDX_LANGUAGE_CHAR,                      /* 62 */
    UDS_IDX_LANGUAGE_VAL,                       /* 63 */

#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
    /* The Notify property is mandatory if the Server supports the update of
     * one or more UDS Characteristic values (e.g. through its User Interface
     * or any other out-of-band mechanism), otherwise, excluded from this
     * version of the service. */
    UDS_IDX_DB_CHANGE_INCREMENT_CCCD,           /* 64 */
#endif
    UDS_IDX_NB,
};

enum uds_opcode_tag
{
    UDS_OPCODE_REGISTER_NEW_USER = 0x01,
    UDS_OPCODE_CONSENT           = 0x02,
    UDS_OPCODE_DELETE_USER_DATA  = 0x03,
    UDS_OPCODE_REPONSE_CODE      = 0x04,
};

/**@brief User Data Service initialization variable. */
typedef struct
{
    uds_init_t uds_init;
    uint16_t   start_hdl;
    /** Client characteristic configuration descriptor of User Control Point for
     * each connection. */
    uint16_t   cp_cccd[UDS_CONNECTION_MAX];
    uint8_t    chars_mask[CHARS_MASK_OCTS];
#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
    /** Client characteristic configuration descriptor of Database Change
     * Increment for each connection. */
    uint16_t   db_change_cccd[UDS_CONNECTION_MAX];
#endif
} uds_env_t;
/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
/** Pointer to User Data Service environment variable. */
static uds_env_t s_uds_env;

/**@brief UDS attributes descriptor which is used to add attributes into
 *        the ATT database.
 */ 
static const attm_desc_t uds_attr_tab[UDS_IDX_NB] =
{
    // User Data Service Declaration
    [UDS_IDX_SVC] = {BLE_ATT_DECL_PRIMARY_SERVICE, READ_PERM_UNSEC, 0, 0},
    // User Index Characteristic
    [UDS_IDX_USER_INDEX_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                 READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_USER_INDEX_VAL]  = {BLE_ATT_CHAR_USER_INDEX,
                                 READ_PERM(AUTH), ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // User Control Point Characteristic
    [UDS_IDX_USER_CTRL_POINT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                      READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_USER_CTRL_POINT_VAL]  = {BLE_ATT_CHAR_USER_CONTROL_POINT,
                                      WRITE_PERM(AUTH) | INDICATE_PERM(AUTH),
                                      ATT_VAL_LOC_USER, UDS_USER_CTRL_POINT_VAL_MAX_LEN},
    [UDS_IDX_USER_CTRL_POINT_CCCD] = {BLE_ATT_DESC_CLIENT_CHAR_CFG,
                                      READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                      ATT_VAL_LOC_USER, 0},
    // Database Change Increment Characteristic
    [UDS_IDX_DB_CHANGE_INCREMENT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                          READ_PERM_UNSEC, 0, 0},
#ifndef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
    [UDS_IDX_DB_CHANGE_INCREMENT_VAL]  = {BLE_ATT_CHAR_DATABASE_CHANGE_INCREMENT,
                                          WRITE_PERM(AUTH) | READ_PERM(AUTH),
                                          ATT_VAL_LOC_USER, sizeof(uint32_t)},
#else
    [UDS_IDX_DB_CHANGE_INCREMENT_VAL]  = {BLE_ATT_CHAR_DATABASE_CHANGE_INCREMENT,
                                          WRITE_PERM(AUTH) | READ_PERM(AUTH) | NOTIFY_PERM(AUTH),
                                          ATT_VAL_LOC_USER, sizeof(uint32_t)},
#endif
    // First Name Characteristic
    [UDS_IDX_FIRST_NAME_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                 READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_FIRST_NAME_VAL]  = {BLE_ATT_CHAR_FIRST_NAME,
                                 READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                 ATT_VAL_LOC_USER, UDS_FIRST_NAME_MAX_LEN},
    // Last Name Characteristic
    [UDS_IDX_LAST_NAME_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_LAST_NAME_VAL]  = {BLE_ATT_CHAR_LAST_NAME,
                                READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                ATT_VAL_LOC_USER, UDS_LAST_NAME_MAX_LEN},
    // Email Address Characteristic
    [UDS_IDX_EMAIL_ADDR_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                 READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_EMAIL_ADDR_VAL]  = {BLE_ATT_CHAR_FIRST_NAME,
                                 READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                 ATT_VAL_LOC_USER, UDS_EMAIL_ADDR_MAX_LEN},
    // Age Characteristic
    [UDS_IDX_AGE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                          READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_AGE_VAL]  = {BLE_ATT_CHAR_AGE,
                          READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                          ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Date of Birth Characteristic
    [UDS_IDX_DATE_OF_BIRTH_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                    READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_DATE_OF_BIRTH_VAL]  = {BLE_ATT_CHAR_AGE,
                                    READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                    ATT_VAL_LOC_USER, UDS_DATE_VAL_MAX_LEN},
    // Gender Characteristic
    [UDS_IDX_GENDER_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                             READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_GENDER_VAL]  = {BLE_ATT_CHAR_GENDER,
                             READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                             ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Weight Characteristic
    [UDS_IDX_WEIGHT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                             READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_WEIGHT_VAL]  = {BLE_ATT_CHAR_WEIGHT,
                             READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                             ATT_VAL_LOC_USER, sizeof(uint16_t)},
    // Height Characteristic
    [UDS_IDX_HEIGHT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                             READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_HEIGHT_VAL]  = {BLE_ATT_CHAR_HEIGHT,
                             READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                             ATT_VAL_LOC_USER, sizeof(uint16_t)},
    // VO2 Max Characteristic
    [UDS_IDX_VO2_MAX_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                              READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_VO2_MAX_VAL]  = {BLE_ATT_CHAR_VO2_MAX,
                              READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                              ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Heart Rate Max Characteristic
    [UDS_IDX_HEART_RATE_MAX_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                     READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_HEART_RATE_MAX_VAL]  = {BLE_ATT_CHAR_MAX_HEART_RATE,
                                     READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                     ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Resting Heart Rate Characteristic
    [UDS_IDX_RESTING_HEART_RATE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                         READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_RESTING_HEART_RATE_VAL]  = {BLE_ATT_CHAR_RESTING_HEART_RATE,
                                         READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                         ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Max Recommended Heart Rate Characteristic
    [UDS_IDX_MAX_RECOMMENDED_HEART_RATE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                 READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_MAX_RECOMMENDED_HEART_RATE_VAL]  = {BLE_ATT_CHAR_MAXIMUM_RECOMMENDED_HEART_RATE,
                                                 READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                 ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Aerobic Threshold Characteristic
    [UDS_IDX_AEROBIC_THRESHOLD_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                        READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_AEROBIC_THRESHOLD_VAL]  = {BLE_ATT_CHAR_AEROBIC_THRESHOLD,
                                        READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                        ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Anaerobic Threshold Characteristic
    [UDS_IDX_ANAEROBIC_THRESHOLD_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                          READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_ANAEROBIC_THRESHOLD_VAL]  = {BLE_ATT_CHAR_ANAEROBIC_THRESHOLD,
                                          READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                          ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Sport Type Characteristic
    [UDS_IDX_SPORT_TYPE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                 READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_SPORT_TYPE_VAL]  = {BLE_ATT_CHAR_SPORT_TYPE_FOR_AEROBIC_AND_ANAEROBIC_THRESHOLDS,
                                 READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                 ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Date of Threshold Assessment Characteristic
    [UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                   READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_VAL]  = {BLE_ATT_CHAR_DATE_OF_THRESHOLD_ASSESSMENT,
                                                   READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                   ATT_VAL_LOC_USER, UDS_DATE_VAL_MAX_LEN},
    // Waist Circumference Characteristic
    [UDS_IDX_WAIST_CIRCUMFERENCE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                          READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_WAIST_CIRCUMFERENCE_VAL]  = {BLE_ATT_CHAR_WAIST_CIRCUMFERENCE,
                                         READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                         ATT_VAL_LOC_USER, sizeof(uint16_t)},
    // Hip Circumference Characteristic
    [UDS_IDX_HIP_CIRCUMFERENCE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                        READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_HIP_CIRCUMFERENCE_VAL]  = {BLE_ATT_CHAR_HIP_CIRCUMFERENCE,
                                        READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                        ATT_VAL_LOC_USER, sizeof(uint16_t)},
    // Fat Burn Heart Rate Lower Limit Characteristic
    [UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                    READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL]  = {BLE_ATT_CHAR_FAT_BURN_HEART_RATE_LOWER_LIMIT,
                                                    READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                    ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Fat Burn Heart Rate Upper Limit Characteristic
    [UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                   READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_VAL]  = {BLE_ATT_CHAR_FAT_BURN_HEART_RATE_UPPER_LIMIT,
                                                   READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                   ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Aerobic Heart Rate Lower Limit Characteristic
    [UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                   READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_VAL]  = {BLE_ATT_CHAR_AEROBIC_HEART_RATE_LOWER_LIMIT,
                                                   READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                   ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Aerobic Heart Rate Upper Limit Characteristic
    [UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                  READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_VAL]  = {BLE_ATT_CHAR_AEROBIC_HEART_RATE_UPPER_LIMIT,
                                                  READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                  ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Anaerobic Heart Rate Lower Limit Characteristic
    [UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                     READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL]  = {BLE_ATT_CHAR_ANAEROBIC_HEART_RATE_LOWER_LIMIT,
                                                     READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                     ATT_VAL_LOC_USER, sizeof(uint8_t)},
    // Anaerobic Heart Rate Upper Limit Characteristic
    [UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                                    READ_PERM_UNSEC, 0, 0},
    [UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL]  = {BLE_ATT_CHAR_ANAEROBIC_HEART_RATE_UPPER_LIMIT,
                                                    READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                                    ATT_VAL_LOC_USER, sizeof(uint8_t)},

#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
    [UDS_IDX_DB_CHANGE_INCREMENT_CCCD] = {BLE_ATT_DESC_CLIENT_CHAR_CFG,
                                          READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                          ATT_VAL_LOC_USER, 0},
#endif
};

/*
 * LOCAL FUNCTION DECLARATIONS
 *****************************************************************************************
 */
static sdk_err_t   uds_init(void);
static void uds_on_connect(uint8_t conn_idx);
static void uds_on_disconnect(uint8_t conn_idx, uint8_t reason);
static sdk_err_t   uds_write_att_cb(uint8_t conn_idx,
                                    const gatts_write_req_cb_t *p_param);
static sdk_err_t   uds_read_att_cb(uint8_t conn_idx,
                                   const gatts_read_req_cb_t *p_param);

/**@brief UDS interface required by profile manager. */
static ble_prf_manager_cbs_t uds_mgr_cbs = {
    uds_init,
    NULL,
    NULL
};

/**@brief UDS GATT server Callbacks. */
static gatts_prf_cbs_t uds_gatts_cbs = {
    uds_read_att_cb,
    uds_write_att_cb,
    NULL,
    NULL
};

/**@brief UDS Information. */
static const prf_server_info_t uds_prf_info = {
    .max_connection_nb = UDS_CONNECTION_MAX,
    .manager_cbs       = &uds_mgr_cbs,
    .gatts_prf_cbs          = &uds_gatts_cbs
};

/**
 *****************************************************************************************
 * @brief Initialize User Data service and create db in att.
 *
 * @return BLE_ATT_ERR_NO_ERROR on success, otherwise error code.
 *****************************************************************************************
 */
static sdk_err_t   uds_init(void)
{
    const uint8_t uds_svc_uuid[] = BLE_ATT_16_TO_16_ARRAY(BLE_ATT_SVC_USER_DATA);
    gatts_create_db_t gatts_db;
    /* The start handle is an in/out parameter of ble_gatts_srvc_db_create().
     * It must be set with PRF_INVALID_HANDLE to be allocated automatically by
     * BLE Stack. */
    uint16_t start_hdl           = PRF_INVALID_HANDLE;

    memcpy(s_uds_env.chars_mask, &s_uds_env.uds_init.uds_chars, sizeof(uds_char_mask_t));
    s_uds_env.chars_mask[0] |= MANDATORY_ATTRS_MASK;
#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
    s_uds_env.chars_mask[CHARS_MASK_OCTS - 1] = 0x01;
#endif
    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                 = &start_hdl;
    gatts_db.uuid                 = (uint8_t *)uds_svc_uuid;
    gatts_db.attr_tab_cfg         = s_uds_env.chars_mask;
    gatts_db.max_nb_attr          = UDS_IDX_NB;
    gatts_db.srvc_perm            = 0;
    gatts_db.attr_tab_type        = SERVICE_TABLE_TYPE_16;
    gatts_db.attr_tab.attr_tab_16 = uds_attr_tab;
    
    sdk_err_t   err_code = ble_gatts_srvc_db_create(&gatts_db);
    if (err_code == SDK_SUCCESS)
    {
        s_uds_env->start_hdl = *gatts_db.shdl;
    }

    return err_code;
}

/**
 *****************************************************************************************
 * @brief Handles reception of the write request.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_param:  Pointer to the parameters of the write request.
 *
 * @return If the request was consumed or not.
 *****************************************************************************************
 */
static sdk_err_t   uds_write_att_cb(uint8_t conn_idx,
                                    const gatts_write_req_cb_t *p_param)
{
    gatts_write_cfm_t cfm;
    uint8_t           idx = prf_find_idx_by_handle(p_param->handle,
                                                   s_uds_env->start_hdl,
                                                   UDS_IDX_NB,
                                                   s_uds_env->attrs_mask);

    cfm.handle = p_param->handle;

    switch (idx)
    {
        case UDS_IDX_USER_CTRL_POINT_VAL:
            break;
        case UDS_IDX_USER_CTRL_POINT_CCCD:
            break;
        case UDS_IDX_DB_CHANGE_INCREMENT_VAL:
            break;
#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
        case UDS_IDX_DB_CHANGE_INCREMENT_CCCD:
            break;
#endif
        case UDS_IDX_FIRST_NAME_VAL:
        case UDS_IDX_LAST_NAME_VAL:
        case UDS_IDX_EMAIL_ADDR_VAL:
        case UDS_IDX_AGE_VAL:
        case UDS_IDX_DATE_OF_BIRTH_VAL:
        case UDS_IDX_GENDER_VAL:
        case UDS_IDX_WEIGHT_VAL:
        case UDS_IDX_HEIGHT_VAL:
        case UDS_IDX_VO2_MAX_VAL:
        case UDS_IDX_HEART_RATE_MAX_VAL:
        case UDS_IDX_RESTING_HEART_RATE_VAL:
        case UDS_IDX_MAX_RECOMMENDED_HEART_RATE_VAL:
        case UDS_IDX_AEROBIC_THRESHOLD_VAL:
        case UDS_IDX_ANAEROBIC_THRESHOLD_VAL:
        case UDS_IDX_SPORT_TYPE_VAL:
        case UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_VAL:
        case UDS_IDX_WAIST_CIRCUMFERENCE_VAL:
        case UDS_IDX_HIP_CIRCUMFERENCE_VAL:
        case UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_FIVE_ZONE_HEART_RATE_LIMITS_VAL:
        case UDS_IDX_THREE_ZONE_HEART_RATE_LIMITS_VAL:
        case UDS_IDX_TWO_ZONE_HEART_RATE_LIMIT_VAL:
        case UDS_IDX_LANGUAGE_VAL:
            if (s_uds_env.evt_handler)
            {
                uds_env_t evt;

                evt.evt_type = UDS_EVT_UDS_CHAR_WRITE;
                evt.conn_idx = conn_idx;
                evt.uds_char = UDS_CHAR_FIRST_NAME << (idx - UDS_IDX_FIRST_NAME_VAL);
                evt.length   = p_param->length;
                evt.p_value  = (const void *)p_param->value;

                return s_uds_env.evt_handler(&evt);
            }
            else
            {
                cfm.status = BLE_ATT_WRITE_NOT_PERMITTED;
            }
            break;
        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    return ble_gatts_write_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the read request.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_param:  Pointer to the parameters of the read request.
 *
 * @return If the request was consumed or not.
 *****************************************************************************************
 */
static sdk_err_t   uds_read_att_cb(uint8_t conn_idx,
                                   const gatts_read_req_cb_t *p_param)
{
    gatts_read_cfm_t cfm;
    uint8_t          idx = prf_find_idx_by_handle(p_param->handle,
                                                  s_uds_env->start_hdl,
                                                  UDS_IDX_NB,
                                                  s_uds_env->attrs_mask);

    cfm.handle = p_param->handle;
    cfm.length = 0;
    cfm.value  = NULL;

    switch (idx)
    {
        case UDS_IDX_USER_CTRL_POINT_VAL:
            break;
        case UDS_IDX_USER_CTRL_POINT_CCCD:
            break;
        case UDS_IDX_DB_CHANGE_INCREMENT_VAL:
            break;
#ifdef SUPPORT_UPDATE_OF_UDS_CHAR_VALUES
        case UDS_IDX_DB_CHANGE_INCREMENT_CCCD:
            break;
#endif
        case UDS_IDX_FIRST_NAME_VAL:
        case UDS_IDX_LAST_NAME_VAL:
        case UDS_IDX_EMAIL_ADDR_VAL:
        case UDS_IDX_AGE_VAL:
        case UDS_IDX_DATE_OF_BIRTH_VAL:
        case UDS_IDX_GENDER_VAL:
        case UDS_IDX_WEIGHT_VAL:
        case UDS_IDX_HEIGHT_VAL:
        case UDS_IDX_VO2_MAX_VAL:
        case UDS_IDX_HEART_RATE_MAX_VAL:
        case UDS_IDX_RESTING_HEART_RATE_VAL:
        case UDS_IDX_MAX_RECOMMENDED_HEART_RATE_VAL:
        case UDS_IDX_AEROBIC_THRESHOLD_VAL:
        case UDS_IDX_ANAEROBIC_THRESHOLD_VAL:
        case UDS_IDX_SPORT_TYPE_VAL:
        case UDS_IDX_DATE_OF_THRESHOLD_ASSESSMENT_VAL:
        case UDS_IDX_WAIST_CIRCUMFERENCE_VAL:
        case UDS_IDX_HIP_CIRCUMFERENCE_VAL:
        case UDS_IDX_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_FAT_BURN_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_AEROBIC_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_AEROBIC_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL:
        case UDS_IDX_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL:
        case UDS_IDX_FIVE_ZONE_HEART_RATE_LIMITS_VAL:
        case UDS_IDX_THREE_ZONE_HEART_RATE_LIMITS_VAL:
        case UDS_IDX_TWO_ZONE_HEART_RATE_LIMIT_VAL:
        case UDS_IDX_LANGUAGE_VAL:
            if (s_uds_env.evt_handler)
            {
                uds_env_t evt;

                evt.evt_type = UDS_EVT_UDS_CHAR_READ;
                evt.conn_idx = conn_idx;
                evt.uds_char = UDS_CHAR_FIRST_NAME << (idx - UDS_IDX_FIRST_NAME_VAL);
                evt.length   = 0;
                evt.p_value  = NULL;

                return s_uds_env.evt_handler(&evt);
            }
            else
            {
                cfm.status = BLE_ATT_WRITE_NOT_PERMITTED;
            }
            break;
        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    return ble_gatts_read_cfm(conn_idx, &cfm);
}

static uint8_t get_uds_char_idx(uds_char_mask_t uds_char)
{
    uint8_t idx;

    for (idx = 0; idx < (UDS_IDX_NB - UDS_IDX_FIRST_NAME_VAL); idx + 2)
    {
        if (uds_char & (UDS_CHAR_FIRST_NAME << idx))
        {
            return (idx + UDS_IDX_FIRST_NAME_VAL);
        }
    }

    return UDS_IDX_NB;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void uds_service_init(uds_init_t *p_uds_init)
{
    s_uds_env.uds_init = *p_uds_init;
    ble_server_prf_add(&uds_prf_info);
}

sdk_err_t   uds_char_write_cfm(uint8_t conn_idx, uds_char_mask_t uds_char,
                               sdk_err_t   err_code)
{
    gatts_write_cfm_t cfm;
    uint8_t           idx = get_uds_char_idx(uds_char);

    if (UDS_IDX_NB == idx)
    {
        cfm.handle = PRF_INVALID_HANDLE;
        cfm.status = BLE_ATT_ATTRIBUTE_NOT_FOUND;
    }
    else
    {
        cfm.handle = prf_find_handle_by_idx(idx, s_uds_env.start_hdl,
                                            s_uds_env.chars_mask);
        cfm.status = err_code;
    }

    return ble_gatts_write_cfm(conn_idx, &cfm);
}

sdk_err_t   uds_char_read_cfm(uint8_t conn_idx, uds_char_mask_t uds_char,
                              sdk_err_t   err_code,
                              uint16_t length, const void *p_value)
{
    gatts_read_cfm_t cfm;
    uint8_t          idx = get_uds_char_idx(uds_char);

    if (UDS_IDX_NB == idx)
    {
        cfm.handle = PRF_INVALID_HANDLE;
        cfm.status = BLE_ATT_ATTRIBUTE_NOT_FOUND;
        cfm.length = 0;
        cfm.value  = NULL;
    }
    else
    {
        cfm.handle = prf_find_handle_by_idx(idx, s_uds_env.start_hdl,
                                            s_uds_env.chars_mask);
        cfm.status = err_code;
        cfm.length = length;
        cfm.value  = p_value;
    }

    return ble_gatts_read_cfm(conn_idx, &cfm);
}

