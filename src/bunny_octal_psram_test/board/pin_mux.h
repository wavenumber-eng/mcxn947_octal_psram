/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define PCR_IBE_ibe1 0x01u /*!<@brief Input Buffer Enable: Enables */

/*! @name PORT1_8 (coord A1), P1_8/J9[32]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_RX_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_RX_PIN 8U                   /*!<@brief PORT pin number */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_RX_PIN_MASK (1U << 8U)      /*!<@brief PORT pin mask */
                                                                        /* @} */

/*! @name PORT1_9 (coord B1), P1_9/J9[30]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_TX_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_TX_PIN 9U                   /*!<@brief PORT pin number */
#define BOARD_INITDEBUG_UARTPINS_DEBUG_UART_TX_PIN_MASK (1U << 9U)      /*!<@brief PORT pin mask */
                                                                        /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UARTPins(void);

/*! @name PORT0_1 (coord A16), P0_1/SWCLK/J23[4]/D16[5]/J22[2]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDCLK_PORT PORT0               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDCLK_PIN 1U                   /*!<@brief PORT pin number */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDCLK_PIN_MASK (1U << 1U)      /*!<@brief PORT pin mask */
                                                                          /* @} */

/*! @name PORT0_0 (coord A17), P0_0/SWDIO/J23[2]/D16[6]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDIO_PORT PORT0               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDIO_PIN 0U                   /*!<@brief PORT pin number */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWDIO_PIN_MASK (1U << 0U)      /*!<@brief PORT pin mask */
                                                                         /* @} */

/*! @name PORT0_2 (coord B16), P0_2/SWO/J23[6]/D16[4]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWO_PORT PORT0               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWO_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_INITSWD_DEBUGPINS_DEBUG_SWD_SWO_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                                                       /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSWD_DEBUGPins(void);

/*! @name PORT1_2 (coord C4), P1_2/SJ4[1]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITLEDSPINS_LED_BLUE_GPIO GPIO1               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_BLUE_GPIO_PIN 2U              /*!<@brief GPIO pin number */
#define BOARD_INITLEDSPINS_LED_BLUE_GPIO_PIN_MASK (1U << 2U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITLEDSPINS_LED_BLUE_PORT PORT1               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_BLUE_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_INITLEDSPINS_LED_BLUE_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name PORT0_10 (coord B12), P0_10/SJ5[1]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITLEDSPINS_LED_RED_GPIO GPIO0                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_RED_GPIO_PIN 10U              /*!<@brief GPIO pin number */
#define BOARD_INITLEDSPINS_LED_RED_GPIO_PIN_MASK (1U << 10U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITLEDSPINS_LED_RED_PORT PORT0                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_RED_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITLEDSPINS_LED_RED_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name PORT0_27 (coord E10), P0_27/SJ6[1]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO GPIO0                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO_PIN 27U              /*!<@brief GPIO pin number */
#define BOARD_INITLEDSPINS_LED_GREEN_GPIO_PIN_MASK (1U << 27U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITLEDSPINS_LED_GREEN_PORT PORT0                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITLEDSPINS_LED_GREEN_PIN 27U                   /*!<@brief PORT pin number */
#define BOARD_INITLEDSPINS_LED_GREEN_PIN_MASK (1U << 27U)      /*!<@brief PORT pin mask */
                                                               /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDsPins(void);

/*! @name PORT0_6 (coord C14), P0_6/J23[7]/SW3[3]/SW3[4]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITBUTTONSPINS_SW3_GPIO GPIO0               /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITBUTTONSPINS_SW3_GPIO_PIN 6U              /*!<@brief GPIO pin number */
#define BOARD_INITBUTTONSPINS_SW3_GPIO_PIN_MASK (1U << 6U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITBUTTONSPINS_SW3_PORT PORT0               /*!<@brief PORT peripheral base pointer */
#define BOARD_INITBUTTONSPINS_SW3_PIN 6U                   /*!<@brief PORT pin number */
#define BOARD_INITBUTTONSPINS_SW3_PIN_MASK (1U << 6U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PORT0_23 (coord B7), P0_23/SJ9[1]/SW2[3]/SW2[4]
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_INITBUTTONSPINS_SW2_GPIO GPIO0                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITBUTTONSPINS_SW2_GPIO_PIN 23U              /*!<@brief GPIO pin number */
#define BOARD_INITBUTTONSPINS_SW2_GPIO_PIN_MASK (1U << 23U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITBUTTONSPINS_SW2_PORT PORT0                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITBUTTONSPINS_SW2_PIN 23U                   /*!<@brief PORT pin number */
#define BOARD_INITBUTTONSPINS_SW2_PIN_MASK (1U << 23U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitBUTTONsPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
