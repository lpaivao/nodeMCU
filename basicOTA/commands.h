/**
 * @file commands.h
 * @author Sergio Matos (sergiopugli@gmail.com), Lucas Paiva ()
 * @brief Comandos de comunicação entre nó de sensores NodeMCU e SBC
 * @version 0.1
 * @date 2022-11-02
 *
 */


/**
 * @brief Request commands: Comandos de solicitação enviados pelo SBC
 * 
 */
#define READ_DIGITAL 'D'
#define READ_ANALOG 'A'
#define NODE_STATUS 'S'
#define LED_TOGGLE 'L'

/**
 * @brief Response commands: Comandos de resposta enviados pelo NodeMCU
 * 
 */
#define NODE_TROUBLE 0x1F
#define NODE_NORMAL 0X00
#define ANALOG_READ 0X01
#define DIGITAL_READ 0x02