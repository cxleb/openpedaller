#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>
#include <driver/sdmmc_host.h>

#define SD_SPI SPI3_HOST
#define PIN_NUM_MISO  (gpio_num_t)6
#define PIN_NUM_MOSI  (gpio_num_t)5
#define PIN_NUM_CLK   (gpio_num_t)4
#define PIN_NUM_CS    (gpio_num_t)2
#define SDlist "/sd_card" // Directory, similar to a standard path
sdmmc_card_t *card = NULL;

void sdcard_init() {
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,  // Maximum number of open files
        .allocation_unit_size = 16 * 1024 // Similar to sector size
    };
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,   // Maximum transfer size   
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_initialize(SD_SPI, &bus_cfg, SDSPI_DEFAULT_DMA));

    //sdspi_dev_handle_t dev_handle;
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
    slot_config.host_id = SD_SPI;
    //sdspi_host_init_device(&slot_config,&dev_handle);
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SD_SPI;
    
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdspi_mount(SDlist, &host, &slot_config, &mount_config, &card)); // Mount the SD card
    if(card != NULL)
    {
        sdmmc_card_print_info(stdout, card); // Print card information
        printf("practical_size:%.2f\n",(float)(card->csd.capacity)/2048/1024);// Size in GB
    }
    else
    {
        printf("Failed to open the SD card.\n");
    }
}
