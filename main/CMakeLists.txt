idf_component_register(SRCS
    src/http_client.c
    src/http_server.c
    src/main.c
    src/mqtt.c
    src/nvs.c
    src/sleep.c
    src/vars.c
    src/sensors.c
    src/wifi.c
    src/spiffs.c
    src/sntpprotocol.c
    lib/app/uiflag_app.c
    INCLUDE_DIRS   "include" "webpage" "lib/app"
    EMBED_FILES     lib/app/uiflag_app.h  webpage/index.css webpage/app.js webpage/index.html webpage/jquery-3.6.0.min.js webpage/fonte_viva.png webpage/quantum_leap.png
    EMBED_TXTFILES certs/certs/ota_root.pem)

# if(CONFIG_DEVMODE_MQTT_USE_CUSTOM_MQTT_BROKER)
# target_add_binary_data(${COMPONENT_TARGET} "dev_certs/aws-root-ca.pem" TEXT)
# target_add_binary_data(${COMPONENT_TARGET} "dev_certs/certificate.pem.crt" TEXT)
# target_add_binary_data(${COMPONENT_TARGET} "dev_certs/private.pem.key" TEXT)
# else


 target_add_binary_data(${COMPONENT_TARGET} "certs/certs/aws-root-ca.pem" TEXT)
 target_add_binary_data(${COMPONENT_TARGET} "certs/certs_8640/certificate.pem.crt" TEXT)
 target_add_binary_data(${COMPONENT_TARGET} "certs/certs_8640/private.pem.key" TEXT)