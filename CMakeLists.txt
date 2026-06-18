# Copyright (c) 2026 lefred (Frederic Descamps)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/plugin/type_uuid
)

MYSQL_ADD_PLUGIN(uuid_more
                 plugin.cc item_uuidmore.cc common.cc
                 ${CMAKE_SOURCE_DIR}/plugin/type_uuid/item_uuidfunc.cc
                 RECOMPILE_FOR_EMBEDDED
                 MODULE_ONLY COMPONENT Test)
