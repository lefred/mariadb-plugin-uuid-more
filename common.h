/* Copyright (c) 2026 lefred (Frederic Descamps)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#ifndef UUID_MORE_COMMON_INCLUDED
#define UUID_MORE_COMMON_INCLUDED

#include "mariadb.h"
#include <string>

#define UUID_MORE_LENGTH 16
#define UUID_MORE_CANONICAL_LENGTH 36

typedef unsigned char uuid_more_t[UUID_MORE_LENGTH];

bool uuid_more_parse(const std::string &str, uuid_more_t uuid);
std::string uuid_more_canonical(const uuid_more_t uuid);
int uuid_more_version(const uuid_more_t uuid);
std::string uuid_more_variant_name(const uuid_more_t uuid);
bool uuid_more_has_timestamp(const uuid_more_t uuid);
bool uuid_more_timestamp_ms(const uuid_more_t uuid, uint64 *timestamp_ms);
bool uuid_more_is_ordered(const uuid_more_t uuid);
bool uuid_more_node(const uuid_more_t uuid, std::string *node);
bool uuid_more_to_base64(const uuid_more_t uuid, std::string *out);
bool uuid_more_base64_to_uuid(const std::string &str, uuid_more_t uuid);
std::string uuid_more_info_json(const uuid_more_t uuid);

#endif
