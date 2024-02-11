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

struct wimotion_packet
{
    uint8_t header;
    uint8_t content;
    union 
    {
        uint8_t raw[8];
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

   if ( this->stream_->available() > 0 ) {
        char buf[128];
    
        size_t len = std::min(lwrb_get_free(&this->buff), (lwrb_sz_t)sizeof(buff));
        if (len > 0)
        {
            size_t bytes_read = this->stream_->read_array(reinterpret_cast<uint8_t*>(buf), len);
            ESP_LOGD(TAG, "Read %d bytes from serial port", (int)(bytes_read));
            lwrb_write_ex(&this->buff, buf, bytes_read, NULL, LWRB_FLAG_WRITE_ALL);
        }
   }
}


void WitmotionComponent::parse() 
{

    while(lwrb_get_full(&this->buff) >= sizeof(wimotion_packet))
    {
        wimotion_packet dat;
        ESP_LOGD(TAG, "Scanning for %d bytes", (int)(sizeof(wimotion_packet)));
        lwrb_sz_t ret = lwrb_peek(&buff, 0, &dat,sizeof(wimotion_packet));
        ESP_LOGD(TAG, "Got  %d bytes", (int)(ret));
        if(dat.header != ProtocolHeader)
        {
            lwrb_skip(&this->buff,1);
            continue;
        }

        if((dat.content & 0x50) == 0)
        {
            lwrb_skip(&this->buff,1);
            continue;
        }

        /* Check CRC */
        uint8_t crc = dat.header + dat.content;
        for (int i=0;i< member_size(wimotion_packet,raw), ++i) crc+=dat.raw[i];

        ESP_LOGD(TAG, "CRC Should be: 0x%02x is 0x%02x", dat.crc, crc);

        if (crc == dat.crc)
        {
              ESP_LOGD(TAG, "Good Packet, type %d", dat.content);
              lwrb_skip(&this->buff,sizeof(wimotion_packet));
        }


    }

