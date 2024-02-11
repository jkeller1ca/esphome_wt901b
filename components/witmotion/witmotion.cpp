/* Copyright (C) 2020-2022 Oxan van Leeuwen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "witmotion.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/util.h"



static const char *TAG = "witmotion";


/* From: https://wit-motion.gitbook.io/witmotion-sdk/wit-standard-protocol/wit-standard-communication-protocol
*/

static const size_t BUFFER_SIZE = 2 * 11;  /* Size of 2 WiMotion Messages */

static const uint8_t ProtocolHeader = 0x55;

enum class  DataContent {
    Time = 0x50,
    Acceleration = 0x51,
    Angular_Velocity = 0x52,
    Angle = 0x53,
    Magetic_Field = 0x54,
    Port = 0x55,
    Barometric_Altitude = 0x56,
    Latitude_Longitude = 0x57,
    Ground_Speed = 0x58,
    Quaternion = 0x59,
    GPS_Location_Accuracy = 0x5A

};

using namespace esphome;

void WitmotionComponent::setup() {
    ESP_LOGCONFIG(TAG, "Setting up witmotion...");

    /* Declare rb instance & raw data */
    lwrb_t buff;
    uint8_t buff_data[8];

    /* Application code ... */
    lwrb_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */

    /* Write 4 bytes of data */
    lwrb_write(&buff, "0123", 4);

    /* Print number of bytes in buffer */
    ESP_LOGCONFIG(TAG,"Bytes in buffer: %d\r\n", (int)lwrb_get_full(&buff));

    }

void WitmotionComponent::loop() {
    this->read();
}

void WitmotionComponent::read() {
    


    }

