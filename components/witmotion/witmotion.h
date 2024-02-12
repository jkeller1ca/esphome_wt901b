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
#include "esphome/components/sensor/sensor.h"

#define LWRB_DISABLE_ATOMIC 1
#include "lwrb/lwrb.h"

#include <memory>
#include <string>
#include <vector>

struct wimotion_packet;
class WitmotionComponent : public esphome::Component {
public:
    WitmotionComponent() = default;
    explicit WitmotionComponent(esphome::uart::UARTComponent *stream) : stream_{stream} {}
    void set_uart_parent(esphome::uart::UARTComponent *parent) { this->stream_ = parent;
    this->stream_->set_rx_buffer_size(128); }

    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_temperature_sensor(esphome::sensor::Sensor *s) { temperature_sensor_ = s; }
    void set_roll_sensor(esphome::sensor::Sensor *s) { roll_sensor_ = s; }
    void set_pitch_sensor(esphome::sensor::Sensor *s) { pitch_sensor_ = s; }
    void set_yaw_sensor(esphome::sensor::Sensor *s) { yaw_sensor_ = s; }
    void set_pressure_sensor(esphome::sensor::Sensor *s) { barometric_pressure_sensor_ = s; }
     
    float get_setup_priority() const override { return esphome::setup_priority::BUS; }

	
protected:
    void read_from_serial();
    void parse();
    void fix_endian(wimotion_packet * );

    esphome::uart::UARTComponent *stream_{nullptr};

    lwrb_t buff;
    uint8_t buff_data[5*11];
     
    esphome::sensor::Sensor *temperature_sensor_{nullptr};
    esphome::sensor::Sensor *roll_sensor_{nullptr};
    esphome::sensor::Sensor *pitch_sensor_{nullptr};
    esphome::sensor::Sensor *yaw_sensor_{nullptr};
    esphome::sensor::Sensor *barometric_pressure_sensor_{nullptr};
    

};