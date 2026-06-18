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
#include "item_uuidmore.h"

#include "common.h"
#include <mysqld_error.h>

static bool get_uuid_arg(Item **args, uuid_more_t uuid, const char *func_name)
{
  String in_tmp;
  String *uuid_arg= args[0]->val_str(&in_tmp);
  if (!uuid_arg)
    return false;

  std::string in_str(uuid_arg->ptr(), uuid_arg->length());
  if (!uuid_more_parse(in_str, uuid))
  {
    my_printf_error(ER_UNKNOWN_ERROR, "%s: not a valid UUID", MYF(0),
                    func_name);
    return false;
  }
  return true;
}

static String *copy_string(String *str, const std::string &value,
                           CHARSET_INFO *cs, bool *null_value)
{
  if (str->copy(value.c_str(), static_cast<uint>(value.size()), cs))
  {
    *null_value= true;
    return nullptr;
  }

  *null_value= false;
  return str;
}

bool Item_func_uuid_node::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(17);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_node::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_node")};
  return name;
}

String *Item_func_uuid_node::val_str(String *str)
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_node"))
  {
    null_value= true;
    return nullptr;
  }

  std::string node;
  if (!uuid_more_node(uuid, &node))
  {
    null_value= true;
    return nullptr;
  }

  return copy_string(str, node, collation.collation, &null_value);
}

bool Item_func_uuid_is_ordered::fix_length_and_dec(THD *thd)
{
  decimals= 0;
  max_length= 1;
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_is_ordered::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_is_ordered")};
  return name;
}

bool Item_func_uuid_is_ordered::val_bool()
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_is_ordered"))
  {
    null_value= true;
    return false;
  }

  null_value= false;
  return uuid_more_is_ordered(uuid);
}

bool Item_func_uuid_variant::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(9);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_variant::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_variant")};
  return name;
}

String *Item_func_uuid_variant::val_str(String *str)
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_variant"))
  {
    null_value= true;
    return nullptr;
  }

  return copy_string(str, uuid_more_variant_name(uuid), collation.collation,
                     &null_value);
}

bool Item_func_uuid_to_base64::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(24);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_to_base64::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_to_base64")};
  return name;
}

String *Item_func_uuid_to_base64::val_str(String *str)
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_to_base64"))
  {
    null_value= true;
    return nullptr;
  }

  std::string out;
  if (!uuid_more_to_base64(uuid, &out))
  {
    null_value= true;
    return nullptr;
  }

  return copy_string(str, out, collation.collation, &null_value);
}

bool Item_func_base64_to_uuid::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(UUID_MORE_CANONICAL_LENGTH);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_base64_to_uuid::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("base64_to_uuid")};
  return name;
}

String *Item_func_base64_to_uuid::val_str(String *str)
{
  String in_tmp;
  String *base64_arg= args[0]->val_str(&in_tmp);
  if (!base64_arg)
  {
    null_value= true;
    return nullptr;
  }

  uuid_more_t uuid;
  std::string in_str(base64_arg->ptr(), base64_arg->length());
  if (!uuid_more_base64_to_uuid(in_str, uuid))
  {
    my_printf_error(ER_UNKNOWN_ERROR, "base64_to_uuid: not a valid base64 UUID",
                    MYF(0));
    null_value= true;
    return nullptr;
  }

  return copy_string(str, uuid_more_canonical(uuid), collation.collation,
                     &null_value);
}

bool Item_func_uuid_canonical::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(UUID_MORE_CANONICAL_LENGTH);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_canonical::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_canonical")};
  return name;
}

String *Item_func_uuid_canonical::val_str(String *str)
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_canonical"))
  {
    null_value= true;
    return nullptr;
  }

  return copy_string(str, uuid_more_canonical(uuid), collation.collation,
                     &null_value);
}

bool Item_func_uuid_info::fix_length_and_dec(THD *thd)
{
  collation.set(DTCollation_numeric());
  fix_char_length(128);
  set_maybe_null();
  return FALSE;
}

LEX_CSTRING Item_func_uuid_info::func_name_cstring() const
{
  static LEX_CSTRING name= {STRING_WITH_LEN("uuid_info")};
  return name;
}

String *Item_func_uuid_info::val_str(String *str)
{
  uuid_more_t uuid;
  if (!get_uuid_arg(args, uuid, "uuid_info"))
  {
    null_value= true;
    return nullptr;
  }

  return copy_string(str, uuid_more_info_json(uuid), collation.collation,
                     &null_value);
}
