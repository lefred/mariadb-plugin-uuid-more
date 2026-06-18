# mariadb-plugin-uuid-more

![mariadb-plugin-uuid-more](logo/uuid_more.png)

MariaDB function plugin that provides UUID metadata, canonicalization, and raw
byte Base64 conversion helpers.

The plugin provides seven SQL functions:

| Function | Description |
| --- | --- |
| `uuid_node(uuid)` | Returns the UUID node/MAC address for UUID versions that carry one |
| `uuid_is_ordered(uuid)` | Returns whether the UUID layout is time-sortable |
| `uuid_variant(uuid)` | Returns the UUID variant name, such as `RFC4122`, `Microsoft`, `NCS`, or `Future` |
| `uuid_to_base64(uuid)` | Encodes the raw 16 UUID bytes as Base64 |
| `base64_to_uuid(base64)` | Decodes a Base64-encoded 16-byte UUID into canonical UUID text |
| `uuid_canonical(uuid)` | Returns lowercase canonical UUID text with hyphens |
| `uuid_info(uuid)` | Returns UUID metadata as JSON |

Invalid UUID strings raise an error. Functions that ask for metadata not present
in a valid UUID return `NULL`.

## Build

This plugin is intended to be built as part of a MariaDB source tree. Place the
plugin directory under the MariaDB plugin directory and build MariaDB with the
plugin enabled.

The CMake target builds the module as `uuid_more`.

## Installation

Install the plugin module in MariaDB:

```sql
INSTALL SONAME 'uuid_more';
```

Verify that the seven function plugins are loaded:

```sql
SELECT plugin_name, plugin_type, plugin_library, plugin_description,
       plugin_author
FROM information_schema.PLUGINS
WHERE plugin_type = 'FUNCTION'
  AND plugin_library = 'uuid_more.so'
ORDER BY plugin_name;
```

Expected functions:

```text
+-----------------+-------------+----------------+-----------------------------+---------------+
| plugin_name     | plugin_type | plugin_library | plugin_description          | plugin_author |
+-----------------+-------------+----------------+-----------------------------+---------------+
| base64_to_uuid  | FUNCTION    | uuid_more.so   | Function BASE64_TO_UUID()   | lefred        |
| uuid_canonical  | FUNCTION    | uuid_more.so   | Function UUID_CANONICAL()   | lefred        |
| uuid_info       | FUNCTION    | uuid_more.so   | Function UUID_INFO()        | lefred        |
| uuid_is_ordered | FUNCTION    | uuid_more.so   | Function UUID_IS_ORDERED()  | lefred        |
| uuid_node       | FUNCTION    | uuid_more.so   | Function UUID_NODE()        | lefred        |
| uuid_to_base64  | FUNCTION    | uuid_more.so   | Function UUID_TO_BASE64()   | lefred        |
| uuid_variant    | FUNCTION    | uuid_more.so   | Function UUID_VARIANT()     | lefred        |
+-----------------+-------------+----------------+-----------------------------+---------------+
```

Uninstall the plugin module:

```sql
UNINSTALL SONAME 'uuid_more';
```

## Examples

### `uuid_node()`

Return the node value from UUID versions that include one, such as UUIDv1 and
UUIDv6:

```sql
SET @uuid_v1= '896e34e0-2d0c-11ea-8000-001122334455';
SET @uuid_v6= '1e02d0c8-96e3-64e0-8000-001122334455';
SET @uuid_v4= 'f47ac10b-58cc-4372-a567-0e02b2c3d479';

SELECT uuid_node(@uuid_v1) AS v1_node,
       uuid_node(@uuid_v6) AS v6_node,
       uuid_node(@uuid_v4) AS v4_node;
```

Result:

```text
+-------------------+-------------------+---------+
| v1_node           | v6_node           | v4_node |
+-------------------+-------------------+---------+
| 00:11:22:33:44:55 | 00:11:22:33:44:55 | NULL    |
+-------------------+-------------------+---------+
```

### `uuid_is_ordered()`

Return whether the UUID can be sorted by its canonical text order to preserve
time order:

```sql
SELECT uuid_is_ordered(@uuid_v1) AS v1_ordered,
       uuid_is_ordered(@uuid_v6) AS v6_ordered,
       uuid_is_ordered('018bcfe5-687b-7000-8000-000000000000') AS v7_ordered,
       uuid_is_ordered(@uuid_v4) AS v4_ordered;
```

Result:

```text
+------------+------------+------------+------------+
| v1_ordered | v6_ordered | v7_ordered | v4_ordered |
+------------+------------+------------+------------+
|          0 |          1 |          1 |          0 |
+------------+------------+------------+------------+
```

### `uuid_variant()`

Return the UUID variant:

```sql
SELECT uuid_variant('00000000-0000-0000-0000-000000000000') AS ncs_variant,
       uuid_variant(@uuid_v4) AS rfc_variant,
       uuid_variant('00000000-0000-0000-c000-000000000000') AS microsoft_variant,
       uuid_variant('00000000-0000-0000-e000-000000000000') AS future_variant;
```

Result:

```text
+-------------+-------------+-------------------+----------------+
| ncs_variant | rfc_variant | microsoft_variant | future_variant |
+-------------+-------------+-------------------+----------------+
| NCS         | RFC4122     | Microsoft         | Future         |
+-------------+-------------+-------------------+----------------+
```

### `uuid_to_base64()` and `base64_to_uuid()`

Convert between canonical UUID text and Base64-encoded raw UUID bytes:

```sql
SELECT uuid_to_base64(@uuid_v4) AS v4_base64,
       base64_to_uuid(uuid_to_base64(@uuid_v4)) AS v4_from_base64;
```

Result:

```text
+--------------------------+--------------------------------------+
| v4_base64                | v4_from_base64                       |
+--------------------------+--------------------------------------+
| 9HrBC1jMQ3KlZw4CssPUeQ== | f47ac10b-58cc-4372-a567-0e02b2c3d479 |
+--------------------------+--------------------------------------+
```

### `uuid_canonical()`

Normalize uppercase or bare 32-hex UUID text to lowercase canonical form:

```sql
SELECT uuid_canonical('F47AC10B58CC4372A5670E02B2C3D479') AS canonical_bare,
       uuid_canonical('F47AC10B-58CC-4372-A567-0E02B2C3D479') AS canonical_hyphenated;
```

Result:

```text
+--------------------------------------+--------------------------------------+
| canonical_bare                       | canonical_hyphenated                 |
+--------------------------------------+--------------------------------------+
| f47ac10b-58cc-4372-a567-0e02b2c3d479 | f47ac10b-58cc-4372-a567-0e02b2c3d479 |
+--------------------------------------+--------------------------------------+
```

### `uuid_info()`

Return metadata as JSON:

```sql
SELECT uuid_info('018bcfe5-687b-7000-8000-000000000000') AS info;
```

Result:

```text
+--------------------------------------------------------------------------------+
| info                                                                           |
+--------------------------------------------------------------------------------+
| {"version":7,"variant":"RFC4122","timestamp":1700000000123,"sortable":true} |
+--------------------------------------------------------------------------------+
```

Extract individual JSON fields:

```sql
SELECT json_value(uuid_info('018bcfe5-687b-7000-8000-000000000000'), '$.version') AS version,
       json_value(uuid_info('018bcfe5-687b-7000-8000-000000000000'), '$.variant') AS variant,
       json_value(uuid_info('018bcfe5-687b-7000-8000-000000000000'), '$.timestamp') AS timestamp,
       json_value(uuid_info('018bcfe5-687b-7000-8000-000000000000'), '$.sortable') AS sortable;
```

## NULL and Error Behavior

`NULL` input returns `NULL`:

```sql
SELECT uuid_node(NULL),
       uuid_is_ordered(NULL),
       uuid_variant(NULL),
       uuid_to_base64(NULL),
       base64_to_uuid(NULL),
       uuid_canonical(NULL),
       uuid_info(NULL);
```

Valid UUIDs without a node return `NULL` for `uuid_node()`:

```sql
SELECT uuid_node('f47ac10b-58cc-4372-a567-0e02b2c3d479') AS node;
```

Result:

```text
+------+
| node |
+------+
| NULL |
+------+
```

Invalid UUID text raises an error:

```sql
SELECT uuid_canonical('not-a-uuid');
```

Result:

```text
ERROR 1105 (HY000): uuid_canonical: not a valid UUID
```

Invalid Base64 input for `base64_to_uuid()` also raises an error:

```sql
SELECT base64_to_uuid('not-base64');
```

Result:

```text
ERROR 1105 (HY000): base64_to_uuid: not a valid base64 UUID
```
