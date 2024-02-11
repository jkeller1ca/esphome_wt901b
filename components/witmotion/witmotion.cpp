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
static const char ProtocolHeader = 0x55;


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

}

using namespace esphome;

void WitmotionComponent::setup() {
    ESP_LOGCONFIG(TAG, "Setting up witmotion...");
    };

void WitmotionComponent::loop() {
    this->read();
}

void WitmotionComponent::read() {
    int len;
    while ((len = this->stream_->available()) > 0) {
        char buf[128];
        len = std::min(len, 128);
        this->stream_->read_array(reinterpret_cast<uint8_t*>(buf), len);
        
        int i = 0;
        while ( buf[i] != ProtocolHeader && i<len)
            ++i;

        if (i==len) continue;
    
        switch (buf[i])
        {
            case DataContent::Time:

            break;
            case DataContent::Acceleration:
                 ESP_LOGCONFIG(TAG, "RX: Acceleration");
            break;
            case DataContent::Angular_Velocity:

            break;
            case DataContent::Angle:

            break;
            case DataContent::Magetic_Field:

            break;
            case DataContent::Port:
             ESP_LOGCONFIG(TAG, "RX: Port");

            break;
            case DataContent::Barometric_Altitude:

            break;
            case DataContent::Latitude_Longitude:

            break;
            case DataContent::Ground_Speed:

            break;
            case DataContent::Quaternion:

            break;
            case DataContent::GPS_Location_Accuracy:

            break;

        }

    }
}
