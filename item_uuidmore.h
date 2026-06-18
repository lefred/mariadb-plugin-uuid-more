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

#ifndef ITEM_UUIDMORE_INCLUDED
#define ITEM_UUIDMORE_INCLUDED

#include "item.h"

class Item_func_uuid_node : public Item_str_func
{
public:
  Item_func_uuid_node(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_node>(thd, this); }
};

class Item_func_uuid_is_ordered : public Item_bool_func
{
public:
  Item_func_uuid_is_ordered(THD *thd, Item *arg1) : Item_bool_func(thd, arg1) {}
  bool val_bool() override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_is_ordered>(thd, this); }
};

class Item_func_uuid_variant : public Item_str_func
{
public:
  Item_func_uuid_variant(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_variant>(thd, this); }
};

class Item_func_uuid_to_base64 : public Item_str_func
{
public:
  Item_func_uuid_to_base64(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_to_base64>(thd, this); }
};

class Item_func_base64_to_uuid : public Item_str_func
{
public:
  Item_func_base64_to_uuid(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_base64_to_uuid>(thd, this); }
};

class Item_func_uuid_canonical : public Item_str_func
{
public:
  Item_func_uuid_canonical(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_canonical>(thd, this); }
};

class Item_func_uuid_info : public Item_str_func
{
public:
  Item_func_uuid_info(THD *thd, Item *arg1) : Item_str_func(thd, arg1) {}
  String *val_str(String *str) override;
  bool fix_length_and_dec(THD *thd) override;
  LEX_CSTRING func_name_cstring() const override;
  Item *shallow_copy(THD *thd) const override
  { return get_item_copy<Item_func_uuid_info>(thd, this); }
};

#endif
