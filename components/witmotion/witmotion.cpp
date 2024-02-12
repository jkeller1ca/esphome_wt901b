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
#define member_size(type, member) sizeof(((type *)0)->member)

static const uint8_t ProtocolHeader = 0x55U;

enum class  DataContent {
    Time = 0x50U,
    Acceleration = 0x51U,
    Angular_Velocity = 0x52U,
    Angle = 0x53U,
    Magetic_Field = 0x54U,
    Port = 0x55U,
    Barometric_Altitude = 0x56,
    Latitude_Longitude = 0x57,
    Ground_Speed = 0x58,
    Quaternion = 0x59,
    GPS_Location_Accuracy = 0x5A

};

struct wimotion_packet
{
    uint8_t header;
    uint8_t content;
    union 
    {
        uint8_t raw[8];
        int16_t as_int16[4];
        struct 
        {
            int16_t Ax;
            int16_t Ay;
            int16_t Az;
            int16_t temperature;
        } acceleration;
        struct 
        {
            int16_t roll;
            int16_t pitch;
            int16_t yaw;
            int16_t version;
        } angle;
        struct
        {   
            uint32_t pressure;
            uint32_t altitude;
        } pressure;
        
    };
    uint8_t crc;

} __attribute__((__packed__));

using namespace esphome;

void WitmotionComponent::setup() {
    ESP_LOGCONFIG(TAG, "Setting up witmotion...");

    lwrb_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */
    }

void WitmotionComponent::loop() {
    this->parse();
    this->read_from_serial();
}

void WitmotionComponent::read_from_serial()
{

    char buf[128];

    size_t len = std::min(std::min(lwrb_get_free(&this->buff), (lwrb_sz_t)sizeof(buf)),  (lwrb_sz_t)this->stream_->available());
    if (len > 0)
    {
        this->stream_->read_array(reinterpret_cast<uint8_t*>(buf), len);
        lwrb_write_ex(&this->buff, buf, len, NULL, LWRB_FLAG_WRITE_ALL);
    }

}

void WitmotionComponent::fix_endian(wimotion_packet * dat)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return;
#else
    for(int =0;i<member_size(wimotion_packet,as_int16);++i)
    {
        wimotion_packet.as_int16[i]=byteswap(wimotion_packet.as_int16[i]);
    }
#endif
}

void WitmotionComponent::parse() 
{
    while(lwrb_get_full(&this->buff) >= sizeof(wimotion_packet))
    {
        wimotion_packet dat;
        lwrb_sz_t ret = lwrb_peek(&this->buff, 0, &dat,sizeof(wimotion_packet));

        if(dat.header != ProtocolHeader)
        {

            lwrb_skip(&this->buff,1);
            continue;
        }

        if((dat.content & 0x50U) == 0)
        {
            lwrb_skip(&this->buff,2);
            continue;
        }

        /* Check CRC */
        uint8_t crc = dat.header + dat.content;
        for (int i=0;i< member_size(wimotion_packet,raw); ++i) crc+=dat.raw[i];

        if (crc == dat.crc)
        {
              switch(static_cast<DataContent>(dat.content))
              {
                case DataContent::Acceleration:
                {
                    fix_endian(&dat);
                    float temperature = dat.acceleration.temperature /100.0;
                    ESP_LOGD(TAG, "Acceleration Message: Temperature: %02f", temperature);
                    if (this->temperature_sensor_ != nullptr)
                        this->temperature_sensor_->publish_state(temperature);
                }
                break;
                case DataContent::Angle:
                {
                    fix_endian(&dat);
                    float roll = (float)dat.angle.roll / 32768 * 180.0;
                    float pitch = (float)dat.angle.pitch / 32768 * 180.0;
                    float yaw = (float)dat.angle.yaw / 32768 * 180.0;

                    if (this->roll_sensor_ != nullptr)
                        this->roll_sensor_->publish_state(roll);
                    if (this->pitch_sensor_ != nullptr)
                        this->pitch_sensor_->publish_state(pitch);
                    if (this->yaw_sensor_ != nullptr)
                        this->yaw_sensor_->publish_state(yaw);
                }
                break;
                case DataContent::Barometric_Altitude:
                {
                    if (this->barometric_pressure_sensor_ != nullptr)
                        this->barometric_pressure_sensor_->publish_state(dat.pressure.pressure/100.);

                }
                break;
              }

              lwrb_skip(&this->buff,sizeof(wimotion_packet));
              return;
        }
        else
        {
            lwrb_skip(&this->buff,1);
        }


    }

}

void WitmotionComponent::dump_config() {
        ESP_LOGCONFIG(TAG, "WITMOTION:");
        LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
        }
