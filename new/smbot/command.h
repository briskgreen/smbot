#ifndef _COMMAND_H
#define _COMMAND_H

#include "smbot.h"

#define BING_AU "Authorization: Basic OisrZkFPcUREQUFZYmxQeExMemVXYUE0WGJqVUdGc0FqZ0RDY1FZRFhOT0U=\n"
#define GOOGLE_KEY "your key"

void get_man_url(SMBOT_DATA *data);

void get_ip_addr(SMBOT_DATA *data);

void bing_dict(SMBOT_DATA *data);

void get_youku_url(SMBOT_DATA *data);

void get_bt(SMBOT_DATA *data);

void get_zip_code(SMBOT_DATA *data);

void get_weather(SMBOT_DATA *data);

void get_stack(SMBOT_DATA *data);

void get_id_information(SMBOT_DATA *data);

void check_id_card(SMBOT_DATA *data);

void get_url_encode(SMBOT_DATA *data);

void get_url_decode(SMBOT_DATA *data);

void get_joke(SMBOT_DATA *data);

void get_dream(SMBOT_DATA *data);

void get_song_url(SMBOT_DATA *data);

void get_bing(SMBOT_DATA *data);

void get_google_image_url(SMBOT_DATA *data);

void get_google(SMBOT_DATA *data);

#endif
