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

#include <mariadb.h>
#include "item_uuidmore.h"
#include <mysql/plugin_function.h>
#include <sql_class.h>

template <class Item_func>
class Create_func_uuid_more_arg1 : public Create_func_arg1
{
public:
  Item *create_1_arg(THD *thd, Item *arg1) override
  {
    return new (thd->mem_root) Item_func(thd, arg1);
  }
  static Create_func_uuid_more_arg1 s_singleton;

protected:
  Create_func_uuid_more_arg1() {}
  ~Create_func_uuid_more_arg1() override {}
};

template <class Item_func>
Create_func_uuid_more_arg1<Item_func>
    Create_func_uuid_more_arg1<Item_func>::s_singleton;

#define BUILDER(F) &Create_func_uuid_more_arg1<F>::s_singleton

static Plugin_function plugin_descriptor_function_uuid_node(
    BUILDER(Item_func_uuid_node)),
    plugin_descriptor_function_uuid_is_ordered(
        BUILDER(Item_func_uuid_is_ordered)),
    plugin_descriptor_function_uuid_variant(BUILDER(Item_func_uuid_variant)),
    plugin_descriptor_function_uuid_to_base64(
        BUILDER(Item_func_uuid_to_base64)),
    plugin_descriptor_function_base64_to_uuid(
        BUILDER(Item_func_base64_to_uuid)),
    plugin_descriptor_function_uuid_canonical(
        BUILDER(Item_func_uuid_canonical)),
    plugin_descriptor_function_uuid_info(BUILDER(Item_func_uuid_info));

/*************************************************************************/

maria_declare_plugin(uuid_more){
    MariaDB_FUNCTION_PLUGIN,
    &plugin_descriptor_function_uuid_node,
    "uuid_node",
    "lefred",
    "Function UUID_NODE()",
    PLUGIN_LICENSE_GPL,
    0,
    0,
    0x0100,
    NULL,
    NULL,
    "1.0",
    MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_uuid_is_ordered,
     "uuid_is_ordered",
     "lefred",
     "Function UUID_IS_ORDERED()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_uuid_variant,
     "uuid_variant",
     "lefred",
     "Function UUID_VARIANT()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_uuid_to_base64,
     "uuid_to_base64",
     "lefred",
     "Function UUID_TO_BASE64()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_base64_to_uuid,
     "base64_to_uuid",
     "lefred",
     "Function BASE64_TO_UUID()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_uuid_canonical,
     "uuid_canonical",
     "lefred",
     "Function UUID_CANONICAL()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA},
    {MariaDB_FUNCTION_PLUGIN,
     &plugin_descriptor_function_uuid_info,
     "uuid_info",
     "lefred",
     "Function UUID_INFO()",
     PLUGIN_LICENSE_GPL,
     0,
     0,
     0x0100,
     NULL,
     NULL,
     "1.0",
     MariaDB_PLUGIN_MATURITY_BETA} maria_declare_plugin_end;
