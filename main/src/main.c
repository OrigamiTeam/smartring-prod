/**
 * @file      main.c
 * @author    Mateus Araujo @ Quantum Leap (mateus@quleap.com)
 * @brief     Main program of the device, responsible for initializing the remaining
 *            threads and communication
 * 
 * @version   2.0
 * @date      2024-10-22
 *
 * @copyright Copyright (c) 2024
**/

/* Libraries */
#include "libs.h" 

/* Variables */
static DMA_ATTR lv_indev_drv_t indev_drv;     // Variable for Input Device Driver
static DMA_ATTR lv_disp_drv_t  disp_drv;      // Variable for Display Driver

static const char *TAG = "MAIN";              // Tag for logging to the monitor

SemaphoreHandle_t xGuiSemaphore;              // Semaphore variable for the GUI


/**
 * @name    lv_tick_task
 * @brief   Periodically updates the LVGL tick counter to maintain the internal time base.
 * 
 * @param   arg   Pointer to user-provided data (unused).
 * 
 * @note    This function is intended to be called at regular intervals to ensure that the LVGL
 *          library keeps accurate timing for tasks like animations, input handling, and timeouts.
 */
static void lv_tick_task(void *arg) {
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}


/**
 * @name    smart_ring_ui_touch_feedback
 * @brief   Detects press and release touches and activates corresponding smart ring UI flags.
 * 
 * @param   input_driver   Pointer to the input driver handling the touch events.
 * @param   event          Touch event.
 * 
 * @note    This function is used to handle touch feedback for the smart ring UI, setting the appropriate flags
 *          based on the detected touch events.
 */
static void smart_ring_ui_touch_feedback(struct _lv_indev_drv_t *input_driver,lv_event_t event) {
    if (event == LV_EVENT_PRESSED) {
        smart_ring_ui_get_controller()->flags.flag.touch_pressed = true;
    } else if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->flags.flag.touch_released = true;
    }
}


/**
 * @name    ui_thread
 * @brief   Thread responsible for initializing and handling the Smart Ring UI using LVGL.
 * 
 * @param   param   Pointer to user-provided parameters (unused).
 * 
 * @details This function initializes the LVGL graphics library and related hardware drivers
 *          (e.g., SPI or I2C for display and touch control). It sets up the LVGL display
 *          and input devices, creates a periodic timer for LVGL's tick increment, and starts
 *          the main loop to handle UI tasks.
 * 
 *          Key steps include:
 *          - Initialization of the LVGL library and hardware drivers.
 *          - Allocation of display buffer memory and display buffer initialization.
 *          - Setting up display and input drivers for LVGL.
 *          - Creating a periodic timer to call `lv_tick_task` for keeping LVGL timing accurate.
 *          - Running an infinite loop where the UI task (`lv_task_handler`) is executed while
 *            ensuring thread safety with a semaphore.
 * 
 *          The task will continuously check and process UI flags and interactions using LVGL
 *          and Smart Ring UI functions. Semaphore locking is used to ensure exclusive access
 *          to LVGL functions when shared across threads.
 * 
 *          This function is designed to run indefinitely as part of a FreeRTOS task, and
 *          should never return.
 */
static void ui_thread(void *param) {
    (void)param;

    /* Create a mutex semaphore for synchronizing GUI access */
    xGuiSemaphore = xSemaphoreCreateMutex();

    /* Initialize LVGL library */
    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    /* Allocate memory for the display buffer */
    lv_color_t *buf1 = heap_caps_malloc(
                                        DISP_BUF_SIZE * sizeof(lv_color_t), 
                                        MALLOC_CAP_DMA
                                       );
    assert(buf1 != NULL);                                                              // Ensure that memory allocation was successful
    static lv_color_t *buf2 = NULL;                                                    // Optional second buffer for double buffering

    /* Initialize the display buffer */
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    /* Initialize the display driver */ 
    lv_disp_drv_init(&disp_drv);                                                                
    disp_drv.flush_cb = disp_driver_flush;                                             // Set the function to flush the display buffer
    disp_drv.buffer = &disp_buf;                                                       // Assign the display buffer to the driver
    lv_disp_drv_register(&disp_drv);                                                   // Register the display driver with LVGL

    /* Initialize the input device driver */
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb     = touch_driver_read;                                         // Set the function to read touch input
    indev_drv.type        = LV_INDEV_TYPE_POINTER;                                     // Set the input device type to pointer
    indev_drv.feedback_cb = smart_ring_ui_touch_feedback;                              // Set feedback callback for touch events
    lv_indev_drv_register(&indev_drv);                                                 // Register the input device driver with LVGL

    /* Create a periodic timer to call the LVGL tick increment function */
    const esp_timer_create_args_t periodic_timer_args = {
                                                         .callback = &lv_tick_task, 
                                                         .name = "periodic_gui"
                                                        };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(
                                             periodic_timer, 
                                             LV_TICK_PERIOD_MS * 1000
                                            ));

    /* Initialize the Smart Ring UI system */
    smart_ring_ui_init_system();
    ESP_LOGI(TAG, "SmartRing UI initialized.");

    /* Main task loop to handle LVGL tasks and check UI flags */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10));                                                 // Delay to prevent excessive CPU usage

        // Try to lock the semaphore; if successful, process LVGL tasks
        if (xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();                                                         // Handle LVGL tasks such as animations and input
            smart_ring_ui_check_flags();                                               // Check and handle any Smart Ring UI flags
            xSemaphoreGive(xGuiSemaphore);                                             // Release the semaphore
        }
    }

    vTaskDelete(NULL);
}


/**
 * @name    initial_config
 * @brief   Configures the device during its first-time setup.
 * 
 * @details This function handles the initial configuration of the device, including loading settings
 *          from NVS (Non-Volatile Storage), setting up communication interfaces, initializing the
 *          user interface, and configuring system parameters.
 *          Key tasks performed:
 *          - Load information from NVS storage.
 *          - Set up the communication interface (WiFi, GSM, LoRa).
 *          - Update the Smart Ring UI system with necessary information.
 *          - Delay to allow for proper UI initialization.
 */
void initial_config() {

    // TODO: Initial config - Get the information from nvs
    //                        Set up sensors and timer interfaces
    //                        Create sleep mode

    /* Get the smart ring controller instance */
    struct smart_ring_controller_t *controller = smart_ring_get_controller();

    /* Configure the MAC address for the device */
    configure_mac_address();

    /* Load information from NVS storage */
    nvs_flash_init();                           // Initialize NVS flash storage
    nvs_load_calibration();                     // Load sensor calibration data from NVS
    nvs_load_connection_type();                 // Load the type of connection (WiFi, GSM, LoRa) from NVS
    nvs_load_order_mode();                      // Load the order mode configuration from NVS

    /* Initiate the Communication interface based on the connection type */
    switch (controller->connection.type) {
    case 'w':
        nvs_load_wifi_credentials();                            // Load WiFi credentials if the connection type is WiFi
        break;
    case 'g':
        // Placeholder for GSM connection initialization
        break;
    case 'l':
        // Placeholder for LoRa connection initialization
        break;
    default:
        ESP_LOGE(TAG, "No communication type selected");        // Log an error if no valid communication type is selected
        break;
    }

    /* Get the UI controller for the smart ring */
    controller->ui_controller = smart_ring_ui_get_controller();

    /* Send system information to the UI system */
    smart_ring_ui_set_connection_type(controller->connection.type);   // Set connection type in the UI
    smart_ring_ui_set_order_mode(controller->order_mode);             // Set order mode in the UI
    smart_ring_ui_set_calibration(controller->sensor.no_deposit,      // Set calibration values in the UI
                                  controller->sensor.full_deposit,
                                  controller->sensor.stable);
    smart_ring_ui_set_firmware(FIRMWARE_VERSION);                     // Set the firmware version in the UI
    smart_ring_ui_set_mac_address(controller->mac_address);           // Set the MAC address in the UI

    /* Set SSID in the UI if the device is provisioned */
    if (controller->connection.is_provisioned) {
        smart_ring_ui_set_ssid(controller->connection.wifi_controller.ssid);
    }

    /* Update the initial state of the UI */
    smart_ring_ui_update_state(STATE_0);

    /* Allow for 5 seconds to guarantee the UI initialization */
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    /* Update the UI state based on the connection type */
    if (controller->connection.type != NULL) {
        smart_ring_ui_set_connection_type(controller->connection.type);    // Update the UI with the connection type
        smart_ring_ui_update_state(STATE_2);                               // Set the UI state to indicate configuration completion
    } else {
        smart_ring_ui_update_state(STATE_1);                               // Set the UI state to indicate no connection type selected
    }

    /* Set flag indicating that loading of configuration is complete */
    controller->flags.flag.end_loading_config = false;
}


/**
 * @name    heap_caps_alloc_failed_hook
 * @brief   Callback function to handle heap allocation failures.
 * 
 * @details This function is called when a heap allocation fails. It logs the function name,
 *          requested memory size, and the capabilities required for the allocation.
 * 
 * @param   requested_size   The size of memory that failed to allocate.
 * @param   caps             The memory capabilities required for the allocation.
 * @param   function_name    The name of the function that requested the allocation.
 */
void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name) {
    printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",
           function_name, requested_size, caps);
}


/**
 * @name    app_main
 * @brief   Main application function for initializing and starting tasks.
 * 
 * @details This function sets up the core tasks needed to operate the smart ring. It registers a callback
 *          for handling heap allocation failures, initializes the UI thread, and creates other core threads
 *          such as the sensors and MQTT threads. It also manages flags for the smart ring and UI controllers
 *          in the main task loop.
 * 
 * @return  esp_err_t        Returns ESP_OK if the function executes successfully.
 */
esp_err_t app_main(void) {

    /* Register a callback function for heap allocation failures */
    esp_err_t error = heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook);

    /* Initialize the UI thread */
    // Note: The UI thread should not be on the same core as WiFi (core 0)
    xTaskCreatePinnedToCore(&ui_thread, "ui_thread", 4096, NULL, 10, NULL, 1);

    /* Perform initial configuration of the smart ring system */
    initial_config();

    /* Create a task to handle sensor data for the smart ring */
    xTaskCreate(&smart_ring_sensors_task, "sensors_thread", 4098, NULL, 10, NULL);

    /* Create a task for MQTT communication with AWS */
    xTaskCreatePinnedToCore(&mqtt_aws_thread, "mqtt_aws_thread", MQTT_APP_TASK_STACK_SIZE, NULL,
                                                                 MQTT_APP_TASK_PRIORITY, NULL, 
                                                                 MQTT_APP_TASK_CORE_ID);

    /* Get the smart ring controller and its associated UI controller */
    struct smart_ring_controller_t    *controller    = smart_ring_get_controller();
    struct smart_ring_ui_controller_t *ui_controller = controller->ui_controller;

    /* Main task loop to manage smart ring and UI flags */ 
    for (;;) {
        if (ui_controller->flags.has_flags || controller->flags.has_flags) {
            uiflag.startLogin(controller, ui_controller);                       // Handle user login process
            uiflag.selectCommunication(controller, ui_controller);              // Handle communication type selection
            uiflag.startCommunication(controller, ui_controller);               // Start communication process
            uiflag.signIn(controller);                                          // Handle sign-in process
            uiflag.changePin(controller);                                       // Handle PIN change request
            uiflag.sendTicket(controller);                                      // Handle sending of a ticket
            uiflag.changeStock(controller, ui_controller);                      // Handle stock changes
            uiflag.popupPendingOrders(controller);                              // Display pending orders popup
            uiflag.changeOrderMode(controller, ui_controller);                  // Change the order mode
            uiflag.manageOrders(controller);                                    // Manage orders in the system
            uiflag.resetDevice(controller);                                     // Reset the device
            uiflag.resetWifi(controller);                                       // Reset WiFi settings
            uiflag.updateFirmware(controller);                                  // Start firmware update
            uiflag.updateFirmwareFailed(controller);                            // Handle firmware update failure
            uiflag.updateFirmwareComplete(controller);                          // Handle firmware update completion
            uiflag.startOrder(controller);                                      // Start a new order
            uiflag.startCalibration(controller);                                // Start sensor calibration
            uiflag.emptyCalibration(controller);                                // Handle empty calibration
            uiflag.cancelCalibration(controller);                               // Cancel the calibration process
            uiflag.confirmOrder(controller);                                    // Confirm an order
        }

        /* Delay to prevent excessive CPU usage */
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return ESP_OK; 
}


