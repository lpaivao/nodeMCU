/* UART Select Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "esp8266/gpio_register.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// COMANDOS DE REQUISIÇÃO
#define SITUACAO_ATUAL_NODE 0x03
#define SOLICITA_ENTRADA_ANALOGICA 0x04
#define SOLICITA_ENTRADA_DIGITAL 0x05
#define ACENDE_LED 0x06

// COMANDOS DE RESPOSTA
#define NODE_COM_PROBLEMA 0x1F
#define NODE_FUNCIONANDO 0x00
#define MEDIDA_ENTRADA_ANALOGICA 0x01
#define ESTADO_ENTRADA_DIGITAL 0x02

#define BAUD_RATE_STD 9600

//Tamanho do buffer da uart de 256 bits
#define BUF_SIZE 24

/**
 * RX Function
*/
/*void receiveData(uint8_t *data){

    // Configure a temporary buffer for the incoming data
    uint8_t *data = calloc(3 ,sizeof(data));

    uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
    free(data);
} */

/**
 * TX Function
*/
/*void sendData(){
    // Configure a temporary buffer for the sending data
    uint8_t *data = calloc(3 ,sizeof(data));
    size_t size = sizeof(data);

    uart_write_bytes(UART_NUM_0, (const char *) data, size);
    free(data);
} */

/* bool protocolosRequisicao(uint8_t comando, uint_fast8_t enderecoSensor){

    //uint8_t offset = GPIO_REG_READ(enderecoSensor); //calcula PERIPHS_GPIO_BASEADDR + enderecoSensor

    if(comando == SITUACAO_ATUAL_NODE){
        return true;
    }
    else if(comando == SOLICITA_ENTRADA_ANALOGICA){
        return true;
    }
    else if(comando == SOLICITA_ENTRADA_DIGITAL){
        return true;
    }
    else if(comando == ACENDE_LED){
        return true;
    }
    else
        return false;
} */

/* uint_fast8_t protocolosResposta(uint8_t resposta){

    if(resposta == NODE_COM_PROBLEMA){
    }
    else if(resposta == NODE_FUNCIONANDO){
    }
    else if(resposta == MEDIDA_ENTRADA_ANALOGICA){
    }
    else if(resposta == ESTADO_ENTRADA_DIGITAL){
    }
    else{
        
    }
}

void testeSensor(uint8_t address, gpio_config_t *gpio){

    if(address == GPIO_PIN14_ADDRESS){
        gpio_get_level(GPIO_NUM_14);
    }
    else if(address == GPIO_PIN12_ADDRESS){    
        gpio_get_level(GPIO_NUM_12);
    }
    else if(address == GPIO_PIN13_ADDRESS){    
        gpio_get_level(GPIO_NUM_13);
    }
    else if(address == GPIO_PIN15_ADDRESS){    
        gpio_get_level(GPIO_NUM_15);
    }
} */

/**
 * Setup para UART0
 */

void uart0_init(void) {
  uart_config_t uart_config = {
      .baud_rate = BAUD_RATE_STD,             // constante 9600 baud rate
      .data_bits = UART_DATA_8_BITS,          // 8 bit bytes
      .parity = UART_PARITY_DISABLE,          // no partity
      .stop_bits = UART_STOP_BITS_1,          // one stop bit
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // no flow control
      .rx_flow_ctrl_thresh = 0,               // unused
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_driver_install(UART_NUM_0, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
}

void configGPIO(gpio_config_t io_conf, uint32_t pin_bit_mask, gpio_mode_t mode){
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = mode;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = pin_bit_mask;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    printf("GPIO configured succefully");

}

void configADC(adc_config_t adc_config){
    // Depend on menuconfig->Component config->PHY->vdd33_const value
    // When measuring system voltage(ADC_READ_VDD_MODE), vdd33_const must be set to 255.
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8; // ADC sample collection clock = 80MHz/clk_div = 10MHz
    ESP_ERROR_CHECK(adc_init(&adc_config));
}

void app_main() {

    uart0_init();

    gpio_config_t gpio14;
    configGPIO(gpio14, GPIO_Pin_14, GPIO_MODE_OUTPUT);

    // 1. init adc (A0)
    adc_config_t adc_config;
    configADC(adc_config);

    printf("Driver is installed on UART0: ");
    if(uart_is_driver_installed(UART_NUM_0) == true){
        printf("True");
    }
    else
        printf("False");
}