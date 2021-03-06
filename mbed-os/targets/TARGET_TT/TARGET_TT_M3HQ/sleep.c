/* mbed Microcontroller Library
 * (C)Copyright TOSHIBA ELECTRONIC DEVICES & STORAGE CORPORATION 2018 All rights reserved
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sleep_api.h"
#include "gpio_include.h"

static void warming_up_time(void);

void hal_sleep(void)
{
    // Set low power consumption mode IDLE
    TSB_CG->STBYCR = CG_STBY_MODE_IDLE;
    __DSB(); // Enter idle mode
    __WFI();
}

void hal_deepsleep(void)
{
    uint32_t tmp;
    // WDT sysclock enable
    TSB_CG_FSYSENB_IPENB31 = ENABLE;
    while ((TSB_FC->SR0 & 0x01) != 0x01); // Flash Wait
    // Wait for end of Warming-up for IHOSC1
    while(TSB_CG_WUPHCR_WUEF);
    // Set Warm-up clock to IHOSC1
    TSB_CG_WUPHCR_WUCLK = DISABLE;
    // Set Warming-up time (xxxx) for IHOSC1 return from STOP1 mode
    warming_up_time();
    // Set low power consumption mode STOP1
    TSB_CG->STBYCR = CG_STBY_MODE_STOP1;
    // Set PLL of fsys to fosc(= PLL no USE)
    TSB_CG_PLL0SEL_PLL0SEL = DISABLE;
    // Wait for PLL status of fsys until off state(fosc = 0)
    while(TSB_CG_PLL0SEL_PLL0ST);
    TSB_CG_PLL0SEL_PLL0ON = DISABLE; // Stop PLL of fsys
    TSB_CG_OSCCR_IHOSC1EN = ENABLE; // Enable IHOSC1
    TSB_CG_OSCCR_OSCSEL = DISABLE; // Set fosc to IHOSC1
    while(TSB_CG_OSCCR_OSCF); // Wait for fosc status until IHOSC1
    tmp = TSB_CG->OSCCR; // Set EHOSC off
    tmp &= EXTERNEL_OSC_MASK;
    TSB_CG->OSCCR = tmp;
    TSB_CG_OSCCR_IHOSC2EN = DISABLE; //Stop IHOSC2 of OFD
    // Wait for status of OFD until off ???0???
    while(TSB_CG_OSCCR_IHOSC2F);
    __DSB(); // Enter STOP1 mode
    __WFI();
}

static void warming_up_time(void)
{
    uint32_t work;
    uint64_t x;
    x = (uint64_t)((uint64_t)(IHOSC_CFG_WARM_UP_TIME) * (uint64_t)(IHOSC_CFG_CLOCK));
    x = (uint64_t)(x / (uint64_t)(1000000));
    if (x > (uint64_t)(0xFFFF)) {
        // invalid value
    }
    work = (uint32_t)x;
    work  &= (uint32_t)(0xFFFFFFF0);
    work <<= 16;
    work  |= (uint32_t)(TSB_CG->WUPHCR & ~CGWUPHCR_WUPT_HIGH_MASK);
    TSB_CG->WUPHCR = work;
}
