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

#define MYSQL_SERVER
#include "mariadb.h"
#include "common.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <mysql/service_base64.h>
#include <sstream>

static int hex_value(unsigned char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  ch= static_cast<unsigned char>(std::tolower(ch));
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  return -1;
}

static bool append_hex_byte(std::string *out, unsigned char value)
{
  static const char hex[]= "0123456789abcdef";
  out->push_back(hex[value >> 4]);
  out->push_back(hex[value & 0x0f]);
  return true;
}

bool uuid_more_parse(const std::string &str, uuid_more_t uuid)
{
  if (str.size() != 36 && str.size() != 32)
    return false;

  size_t pos= 0;
  for (int i= 0; i < UUID_MORE_LENGTH; i++)
  {
    if (str.size() == 36 && (pos == 8 || pos == 13 || pos == 18 || pos == 23))
    {
      if (str[pos] != '-')
        return false;
      pos++;
    }

    int hi= hex_value(static_cast<unsigned char>(str[pos]));
    int lo= hex_value(static_cast<unsigned char>(str[pos + 1]));
    if (hi < 0 || lo < 0)
      return false;

    uuid[i]= static_cast<unsigned char>((hi << 4) | lo);
    pos+= 2;
  }

  return pos == str.size();
}

std::string uuid_more_canonical(const uuid_more_t uuid)
{
  std::string out;
  out.reserve(UUID_MORE_CANONICAL_LENGTH);

  for (int i= 0; i < UUID_MORE_LENGTH; i++)
  {
    if (i == 4 || i == 6 || i == 8 || i == 10)
      out.push_back('-');
    append_hex_byte(&out, uuid[i]);
  }
  return out;
}

int uuid_more_version(const uuid_more_t uuid)
{
  return (uuid[6] >> 4) & 0x0f;
}

std::string uuid_more_variant_name(const uuid_more_t uuid)
{
  unsigned char octet= uuid[8];
  if ((octet & 0x80) == 0)
    return "NCS";
  if ((octet & 0xc0) == 0x80)
    return "RFC4122";
  if ((octet & 0xe0) == 0xc0)
    return "Microsoft";
  return "Future";
}

bool uuid_more_has_timestamp(const uuid_more_t uuid)
{
  int version= uuid_more_version(uuid);
  return version == 1 || version == 6 || version == 7;
}

static uint64 read_be(const uuid_more_t uuid, int start, int len)
{
  uint64 out= 0;
  for (int i= 0; i < len; i++)
    out= (out << 8) | uuid[start + i];
  return out;
}

static uint64 uuid_v1_timestamp_100ns(const uuid_more_t uuid)
{
  uint64 time_low= read_be(uuid, 0, 4);
  uint64 time_mid= read_be(uuid, 4, 2);
  uint64 time_hi= read_be(uuid, 6, 2) & 0x0fff;
  return (time_hi << 48) | (time_mid << 32) | time_low;
}

static uint64 uuid_v6_timestamp_100ns(const uuid_more_t uuid)
{
  uint64 high= read_be(uuid, 0, 6);
  uint64 low= read_be(uuid, 6, 2) & 0x0fff;
  return (high << 12) | low;
}

bool uuid_more_timestamp_ms(const uuid_more_t uuid, uint64 *timestamp_ms)
{
  static const uint64 uuid_epoch_to_unix_100ns= 122192928000000000ULL;
  static const uint64 ms_from_100ns= 10000ULL;

  if (!timestamp_ms)
    return false;

  switch (uuid_more_version(uuid))
  {
  case 1:
  {
    uint64 timestamp= uuid_v1_timestamp_100ns(uuid);
    if (timestamp < uuid_epoch_to_unix_100ns)
      return false;
    *timestamp_ms= (timestamp - uuid_epoch_to_unix_100ns) / ms_from_100ns;
    return true;
  }
  case 6:
  {
    uint64 timestamp= uuid_v6_timestamp_100ns(uuid);
    if (timestamp < uuid_epoch_to_unix_100ns)
      return false;
    *timestamp_ms= (timestamp - uuid_epoch_to_unix_100ns) / ms_from_100ns;
    return true;
  }
  case 7:
    *timestamp_ms= read_be(uuid, 0, 6);
    return true;
  default:
    return false;
  }
}

bool uuid_more_is_ordered(const uuid_more_t uuid)
{
  int version= uuid_more_version(uuid);
  return version == 6 || version == 7;
}

bool uuid_more_node(const uuid_more_t uuid, std::string *node)
{
  int version= uuid_more_version(uuid);
  if (!node || (version != 1 && version != 6))
    return false;

  node->clear();
  node->reserve(17);
  for (int i= 10; i < UUID_MORE_LENGTH; i++)
  {
    if (i != 10)
      node->push_back(':');
    append_hex_byte(node, uuid[i]);
  }
  return true;
}

bool uuid_more_to_base64(const uuid_more_t uuid, std::string *out)
{
  if (!out)
    return false;

  int length= my_base64_needed_encoded_length(UUID_MORE_LENGTH);
  if (length <= 0)
    return false;

  out->assign(static_cast<size_t>(length), '\0');
  if (my_base64_encode(uuid, UUID_MORE_LENGTH, &(*out)[0]))
    return false;

  out->resize(static_cast<size_t>(length - 1));
  return true;
}

bool uuid_more_base64_to_uuid(const std::string &str, uuid_more_t uuid)
{
  int length= my_base64_needed_decoded_length(static_cast<int>(str.size()));
  if (length < UUID_MORE_LENGTH)
    return false;

  std::string out(static_cast<size_t>(length), '\0');
  const char *end_ptr= nullptr;
  int decoded= my_base64_decode(str.c_str(), str.size(), &out[0], &end_ptr, 0);
  if (decoded != UUID_MORE_LENGTH || end_ptr != str.c_str() + str.size())
    return false;

  memcpy(uuid, out.data(), UUID_MORE_LENGTH);
  return true;
}

std::string uuid_more_info_json(const uuid_more_t uuid)
{
  uint64 timestamp_ms= 0;
  bool has_timestamp= uuid_more_timestamp_ms(uuid, &timestamp_ms);

  std::ostringstream out;
  out << "{\"version\":" << uuid_more_version(uuid) << ",\"variant\":\""
      << uuid_more_variant_name(uuid) << "\",\"timestamp\":";
  if (has_timestamp)
    out << timestamp_ms;
  else
    out << "null";
  out << ",\"sortable\":" << (uuid_more_is_ordered(uuid) ? "true" : "false")
      << "}";
  return out.str();
}
