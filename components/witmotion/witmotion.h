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

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#define LWRB_DISABLE_ATOMIC 1
#include "lwrb/lwrb.h"

#include <memory>
#include <string>
#include <vector>

class WitmotionComponent : public esphome::Component {
public:
    WitmotionComponent() = default;
    explicit WitmotionComponent(esphome::uart::UARTComponent *stream) : stream_{stream} {}
    void set_uart_parent(esphome::uart::UARTComponent *parent) { this->stream_ = parent;
    this->stream_->set_rx_buffer_size(128); }

    void setup() override;
    void loop() override;


    float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; }

	
protected:
    void read();
    void write();

    esphome::uart::UARTComponent *stream_{nullptr};

};