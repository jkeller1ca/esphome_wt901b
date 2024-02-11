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
        
        for(int i=0;i<len;i++)
            ESP_LOGCONFIG(TAG, "RX: 0x%02x",buf[len]);
    }
}
