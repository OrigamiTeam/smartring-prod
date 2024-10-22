/**
 * @file smart_ring_ui.h
 * @brief  Smart Ring UI header file
 * @retval None
 */

#ifndef SMART_RING_UI_H_
#define SMART_RING_UI_H_

#ifdef __cplusplus
extern "C" {
#endif

//===================//
//      INCLUDES     //
//===================//
#include "driver/gpio.h"
#include "esp_log.h"
#include <esp_system.h>
#include <esp_timer.h>
#include <stdio.h>

#include "./menus/boot.h"
#include "./menus/calibration.h"
#include "./menus/main.h"
#include "./menus/modal.h"
#include "./menus/order.h"
#include "./menus/pin.h"
#include "./menus/reset.h"
#include "./menus/stock.h"
#include "./menus/support.h"
#include "./menus/update.h"
#include "lvgl.h"

//===================//
//      DEFINES      //
//===================//

#define NO_UPDATE               -1
#define SLEEP_TIMEOUT           60
#define STANDBY_TIMEOUT         10

#define SR_DEFAULT_BTN_RADIUS   5
#define SR_DEFAULT_BTN_WIDTH    96
#define SR_DEFAULT_BTN_HEIGHT   35
#define SR_SMALL_BTN_SIZE       SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT
#define SR_MEDIUM_BTN_SIZE      SR_DEFAULT_BTN_WIDTH * 5 / 4, SR_DEFAULT_BTN_HEIGHT * 5 / 4
#define SR_BIG_BTN_SIZE         SR_DEFAULT_BTN_WIDTH * 3 / 2, SR_DEFAULT_BTN_HEIGHT * 3 / 2
#define SR_BIG_BTN_SIZE_STRETCH 250, SR_DEFAULT_BTN_HEIGHT * 3 / 2

/* MENU IDs */
/* BOOTING SCREENS */
#define MENU_ID_BOOT                     0
#define MENU_ID_SELECT_COMMUNICATION     1
#define MENU_ID_SELECTED_COMMUNICATION   2
#define MENU_ID_SLEEP                    3

/* MAIN SCREENS */
#define MENU_ID_MAIN                     10
#define MENU_ID_DASHBOARD                11
#define MENU_ID_PIN                      12
#define MENU_ID_SETTINGS                 13
#define MENU_ID_INFO                     14

/* SUPPORT */
#define MENU_ID_SUPPORT_REVIEW           20
#define MENU_ID_SUPPORT_SUBMITED         21

/* STOCK */
#define MENU_ID_CHANGED_STOCK            30

/* CALIBRATION */
#define MENU_ID_CALIBRATION_EMPTY        40
#define MENU_ID_CALIBRATION_FULL         41
#define MENU_ID_CALIBRATION_PROGRESS     42
#define MENU_ID_CALIBRATION_FINISHED     43

/* RESET */
#define MENU_ID_RESET                    50
#define MENU_ID_RESET_DEVICE             51
#define MENU_ID_RESET_WIFI               52
#define MENU_ID_RESET_PROGRESS           53
#define MENU_ID_RESET_SUCCESS            54
#define MENU_ID_RESET_FAILED             55

/* ORDER */
#define MENU_ID_NEW_ORDER                60
#define MENU_ID_REVIEW_ORDER             61
#define MENU_ID_PROCESSING_ORDER         62
#define MENU_ID_SUCCESSFULL_ORDER        63
#define MENU_ID_CANCELED_ORDER           64
#define MENU_ID_ERROR_ORDER              65
#define MENU_ID_MANAGE_ORDERS            66
#define MENU_ID_ERROR_MANAGE             67
#define MENU_ID_CHANGE_ORDER_MODE        68
#define MENU_ID_SUCCESSFULL_MODE_CHANGE  69

/* UPDATE*/
#define MENU_ID_UPDATE                   70

#define MENU_ID_FIRMWARE_UPDATE          72
#define MENU_ID_FIRMWARE_UPDATE_COMPLETE 73
#define MENU_ID_FIRMWARE_UPDATE_FAILED   74

#define MENU_ID_POPUP_PENDING_ORDER      75


/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief  Flags to integrate the UI component with the rtest of the
 * firmware and assure that the actions are taken at the right time by the
 * LVGL library.
 * @note   It may change during the course of the firmware integration
 */
typedef struct smart_ring_ui_flags_t {
    int update_state             : 1; // Update the state the UI is on
    int select_communication     : 1;
    int start_communication      : 1;
    int sign_in                  : 1;
    int check_option             : 1;
    int cancel_option            : 1;
    int change_stock             : 1;
    int send_ticket              : 1;
    int change_order_mode        : 1;
    int resend_order_mode        : 1;
    int manage_orders            : 1;
    int reset_device             : 1;
    int reset_wifi               : 1;
    int update                   : 1;
    int update_pin               : 1;
    int update_stock_manual      : 1; 
    int update_firmware          : 1;
    int pin_changed              : 1;
    int cancel_update            : 1;
    int success                  : 1;
    int start_login              : 1;
    int start_order              : 1;
    int start_calibration        : 1;
    int cancel_calibration       : 1;
    int empty_calibration        : 1;
    int confirm_order            : 1;
    int touch_pressed            : 1;
    int touch_released           : 1;
    int order_confirmed_response : 1;
    int order_popup_message      : 1;
    int register_water_level     : 1;
    int flag_25                  : 1;
    int flag_26                  : 1;
    int flag_27                  : 1;
    int flag_28                  : 1;
    int flag_29                  : 1;
    int flag_30                  : 1;
    int flag_31                  : 1;
    int flag_32                  : 1;
};

/**
 * @brief  Pending delivery information
 * @note   It may change during the course of the firmware integration
 */
typedef struct smart_ring_ui_delivery_t {
    char     date[32];
    char     ordered_date[32];
    uint8_t  bottles;
    uint16_t cups;
    char     status[16];
};

typedef struct smart_ring_ui_order_t {
    uint8_t  bottles;
    uint16_t cups;
    float    price;
    char     transaction_id[50];
    int      changing_bottles : 1;
    char     confirmation;
};

/**
 * @brief  Timer type to define the behavior on the timer trigger
 * @note   It may change during the course of the firmware integration
 * @retval None
 */
typedef enum smart_ring_ui_timer_type_t {
    CLOSE_MODAL,
    BOOT_START_COMMUNICATION,
    LOGIN_VALIDATION,
    LOGIN_SUCCESSFULL,
    REQUESTING_ORDERS,
    CALIBRATION_EMPTY,
    CALIBRATION_FULL,
    SLEEP,
    STANDBY,
    STANDBY_DASHBOARD,
    CONFIRM_ORDER,
    PIN_UPDATION,
    PENDING_ORDERS,
};

/**
 * @brief  States for the Finite State Machine to control the display
 * information and next screen or actions to be taken
 * @note   It may change during the course of the firmware integration
 */
typedef enum smart_ring_ui_state_machine_t {
    STATE_0,  // State 0 -> Boot screen with the fonte viva logo
    STATE_1,  // State 1 -> Select communication
    STATE_2,  // State 2 -> Communication selected
    STATE_3,  // State 3 -> Provision
    STATE_4,  // State 4 -> Connecting device to internet and back-end
    STATE_5,  // State 5 -> Main screen
    STATE_6,  // State 6 -> No connection modal
    STATE_7,  // State 7 -> Login
    STATE_8,  // State 8 -> Validating pin
    STATE_9,  // State 9 -> Wrong pin
    STATE_10, // State 10 -> Correct pin
    STATE_11, // State 11 -> Dashboard screen
    STATE_12, // State 12 -> Information screen
    STATE_13, // State 13 -> Stock view
    STATE_14, // State 14 -> Stock value successfully changed
    STATE_15, // State 15 -> Support view
    STATE_16, // State 16 -> Support submition
    STATE_17, // State 17 -> Support request confirmation
    STATE_18, // State 18 -> Manage orders
    STATE_19, // State 19 -> Manage orders error
    STATE_20, // State 20 -> Change order mode
    STATE_21, // State 21 -> Confirm order mode change
    STATE_22, // State 22 -> New order
    STATE_23, // State 23 -> New order review
    STATE_24, // State 24 -> Order canceled
    STATE_25, // State 25 -> Order processing
    STATE_26, // State 26 -> Order error
    STATE_27, // State 27 -> Order successfull
    STATE_28, // State 28 -> Settings screen
    STATE_29, // State 29 -> Calibrate no_deposit
    STATE_30, // State 30 -> Calibrate step processing
    STATE_31, // State 31 -> Calibrate step successfull
    STATE_32, // State 32 -> Calibrate full_deposit
    STATE_33, // State 33 -> Calibrate succcessfull
    STATE_34, // State 34 -> Firmware update processing
    STATE_35, // State 35 -> Verify Orders Modals
    STATE_36, // State 36 -> Reset screen
    STATE_37, // State 37 -> Reset device
    STATE_38, // State 38 -> Reset WiFi
    STATE_39, // State 39 -> Reset WiFi processing
    STATE_40, // State 40 -> Reset WiFi successfull
    STATE_41, // State 41 -> Reset WiFi error
    STATE_42, // State 42 -> Standby
    STATE_43, // State 43 -> Update Menu Options 
    STATE_44, // State 44 -> PIN update processing
    STATE_45, // State 45 -> PIN updation failed
    STATE_46, // State 46 -> order list failed
    STATE_47, // State 47 -> pop-up order
    STATE_48, // State 48 -> Shutting down for WiFi Reconnection
};

typedef struct smart_ring_ui_styles_t {
    lv_style_t blank_page;
    lv_style_t blue_button;
    lv_style_t white_button;
    lv_style_t boot_button;
    lv_style_t boot_communications;
    lv_style_t header_and_footer;
    lv_style_t font_12_normal;
    lv_style_t font_14_normal;
    lv_style_t font_16_normal;
    lv_style_t font_16_bold;
    lv_style_t font_18_normal;
    lv_style_t font_18_bold;
    lv_style_t font_22_normal;
    lv_style_t font_24_normal;
    lv_style_t font_26_normal;
};

typedef struct smart_ring_ui_standby_controller_t {
    bool standby_active;
    bool standby_dashboard_active;
    bool sleep_active;
    bool has_callback;
    bool timer_active;
};

/**
 * @brief  Controller that has the main information needed about the UI
 *         system, to prevent unnecessary functions
 * @note   It may change during the course of the firmware integration
 *
 * @param screen           Current screen object
 * @param menu             Current menu id
 * @param state            Current state of the state machine
 * @param has_flags        Inform the controller that there are flags, to prevent
 *                         running the checks every cycle
 * @param flags            Flags for the operation of the controller
 * @param timer            Timer used for timeouts or flow control
 * @param firmware_version Firmware version
 * @param device_name      Device name to present on main screen
 * @param mac_address      Mac address of the device
 * @param styles           Styles to be used on the device screens
 * @param stock            Bottle stock to show the user
 * @param support_type     Support type to communicate with the back-end
 * @param order_mode       Type of order - Manual (m) or Automatic (a)
 * @param ssid             SSID network to present to the user
 * @param connection_type  Type of connection - WiFi (w), Lora (l) or GSM (g)
 * @retval                 None
 */
typedef struct smart_ring_ui_controller_t {
    lv_obj_t                          *screen;
    uint16_t                           menu;
    enum smart_ring_ui_state_machine_t state;
    enum smart_ring_ui_state_machine_t old_state;
    union map {
        struct smart_ring_ui_flags_t flag;
        int                          has_flags;  
    } flags;
    enum smart_ring_ui_timer_type_t           timer_type;
    esp_timer_handle_t                        timer;
    enum smart_ring_ui_timer_type_t           standby_timer_type;
    esp_timer_handle_t                        standby_timer;
    char                                      firmware_version[9];
    char                                      device_name[32];
    char                                      mac_address[18];
    struct smart_ring_ui_styles_t             styles;
    uint8_t                                   stock;
    uint8_t                                   updated_stock;
    uint8_t                                   support_type;
    char                                      order_mode;
    char                                      updated_order_mode;
    char                                      ssid[32];
    char                                      connection_type;
    int                                       no_deposit;
    int                                       full_deposit;
    int                                       current_deposit;
    bool                                      is_stable;
    uint8_t                                   rssi_level;
    bool                                      is_admin;
    bool                                      is_master;
    struct smart_ring_ui_delivery_t          *deliveries;
    int                                       number_of_deliveries;
    struct smart_ring_ui_order_t              order;
    uint8_t                                   calibration_step;
    struct smart_ring_ui_standby_controller_t standby_controller;
    bool                                      newVersion;
};


//=======================//
//   GLOBAL PROTOTYPES   //
//=======================//

/**
 * @brief  Responsible for the ui system initialization and first configuration
 * @note   It may change during the course of the firmware integration
 * @retval if the initialization was configured sucessfully
 */
esp_err_t smart_ring_ui_init_system(void);

/**
 * @brief  Change the screen to the received id
 * @note   It may change during the course of the firmware integration
 * @param  menu_id: id of the screen to be activated
 * @retval None
 */
static void smart_ring_ui_select_menu(int menu_id);

/**
 * @brief  Get the ui firmware version to print on screen
 * @retval Ui version
 */
char *smart_ring_ui_get_firwmare(void);

/**
 * @brief  Set the ui firmware version to print on screen
 * @param  *version: Firmware version
 * @retval None
 */
void smart_ring_ui_set_firmware(char *version);

/**
 * @brief  Get the ui device name to print on screen
 * @retval Device name
 */
char *smart_ring_ui_get_device_name(void);

/**
 * @brief  Set the ui device name to print on screen
 * @param  *name: Device name
 * @retval None
 */
void smart_ring_ui_set_device_name(char *name);

char *smart_ring_ui_get_mac_address(void);
void  smart_ring_ui_set_mac_address(char *name);

char *smart_ring_ui_get_ssid(void);
void  smart_ring_ui_set_ssid(char *name);

/**
 * @brief  Get the ui order mode
 * @retval Order mode
 */
char smart_ring_ui_get_order_mode(void);

/**
 * @brief  Set the ui order mode
 * @param  mode: Order mode - Manual (m) or Automatic (a)
 */
void smart_ring_ui_set_order_mode(char mode);

char smart_ring_ui_get_connection_type(void);
void smart_ring_ui_set_connection_type(char mode);

void smart_ring_ui_set_calibration(int no_deposit, int full_deposit,
                                   bool is_stable);
void smart_ring_ui_set_current_deposit(int current_deposit);

void smart_ring_ui_set_stock(int stock);

uint8_t smart_ring_ui_get_rssi_level();
void    smart_ring_ui_set_rssi_level(uint8_t rssi_level);

/**
 * @brief  Create the header for the screens
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *title: Title String
 * @retval None
 */
void create_header(lv_obj_t *parent, char *title, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Function to check for existing flags and execute the proper
 * functionality
 * @note   It may change during the course of the firmware integration
 * @retval None
 */
void smart_ring_ui_check_flags();

void smart_ring_ui_update_state(enum smart_ring_ui_state_machine_t state);
struct smart_ring_ui_controller_t *smart_ring_ui_get_controller();

static void ui_callback_standby_timer_callback(void *arg);

/**
 * @brief  Timer callback
 * @note   It may change during the course of the firmware integration
 * @param  *arg: User argument
 * @retval None
 */
static void ui_callback_timer_callback(void *arg);

float map(int x, int in_min, int in_max, int out_min, int out_max);

//====================//
//       MACROS       //
//====================//

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SMART_RING_UI_H_ */
