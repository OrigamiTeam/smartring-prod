#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := SmartRing

EXTRA_COMPONENT_DIRS := $(realpath ../..) components/lvgl_esp32_drivers components/lvgl_esp32_drivers/lvgl_touch components/lvgl_esp32_drivers/lvgl_tft

include $(IDF_PATH)/make/project.mk

