## Table of Contents
* [LibYUI REST API v1](#libyui-rest-api-v1)
    * [API Version](#api-version)
        * [Description](#description)
        * [Response](#response)
        * [Examples](#examples)
    * [Application Data](#application-data)
        * [Description](#description)
        * [Response](#response)
        * [Examples](#examples)
    * [Dump Whole Dialog](#dump-whole-dialog)
        * [Description](#description)
        * [Response](#response)
        * [Examples](#examples)
    * [Read Only Specific Widgets](#read-only-specific-widgets)
        * [Description](#description)
        * [Parameters](#parameters)
        * [Response](#response)
        * [Examples](#examples)
    * [Change Widgets, Do an Action](#change-widgets-do-an-action)
        * [Description](#description)
        * [Parameters](#parameters)
        * [Response](#response)
        * [Examples](#examples)

# LibYUI REST API v1

This is the specification of the version `1` of the API.
Documentation refence is shown when accessing root url:
```
curl http://localhost:9999/
```

## API Version

Request: `GET /version`

### Description

Get the application and UI generic properties like text or graphical mode,
dialog size, screen size and supported UI features.

### Response

JSON format

```json
{
  "api_version" : "v1"
}
```

The `api_version` value defines the version of the API. It is used in the URL
as the path prefix.

### Examples

```
curl http://localhost:9999/version
```
---

## Application Data

Request: `GET /v1/application`

### Description

Get the application and UI generic properties like text or graphical mode,
dialog size, screen size and supported UI features.

### Response

JSON format

### Examples

```
curl http://localhost:9999/v1/application
```
---

## Dump Whole Dialog

 Request: `GET /v1/dialog`

### Description

Get the complete dialog structure in the JSON format. The result contains
a nested structure exactly following the structure of the current dialog.

### Response

JSON format

### Examples

```
curl http://localhost:9999/v1/dialog
```

---

## Read Only Specific Widgets

Request: `GET /v1/widgets`

### Description

Return only the selected widgets (in JSON format). The result is a flat list
(no nested structures).

### Parameters

Filter widgets:

- **id** - the widget ID serialized as string, might include special characters
  like backtick (\`)
- **label** - widget label as currently displayed (i.e. translated!)
- **type** - the widget type

Any combination of the filters are also allowed. This is extremely helpful
when multiple widgets have same id or label. Nevertheless, it's recommended
to use unique ids in the application in order to simplify testing.

### Response

JSON format

### Examples

```
curl 'http://localhost:9999/v1/widgets?label=Next
curl 'http://localhost:9999/v1/widgets?id=next
curl 'http://localhost:9999/v1/widgets?type=YCheckBox
curl 'http://localhost:9999/v1/widgets?type=YPushButton&label=ok
curl 'http://localhost:9999/v1/widgets?type=YPushButton&id=next
```

---

## Change Widgets, Do an Action

Request: `POST /v1/widgets`

### Description

Do an action with specified widgets.

### Parameters

Filter the widgets, one of:

- **id** - widget ID serialized as string, might include special characters
  like backtick (\`)
- **label** - widget label as currently displayed (i.e. translated!)
- **type** - type of the widget

Same as for reading, combinations of the filters are also allowed.

Then specify the action:

- **action** - the action to do
- **value** (optional) - new value or a parameter of the action
- **column** (optional) - integer, column number when selecting item in the table
- **row** (optional) - integer, row number when selecting item in the table
Supported actions:

- **press** - to press the button
- **check** | **uncheck**  | **toggle** - check, uncheck or toggle checkbox
- **enter_text** - set text in the field, the text is passed in the
  *value* parameter
- **select** - select value in the combobox, row in the table or node in the
  tree, item in button menu requires *value* parameter
  - In case of table: select row in the table with given value. If
        *column* parameter is not provided, the first column will be used.
        If table contains sub-items, child nodes can be selected by sending
        path, similarly to the Tree widgets: `root_row|sub_item_row`.
  - In case of tree: select node in the tree. Use `|` as a delimiter for
        the child nodes. For example: `root|subnode|subsubnode`.
  - In case of button menu: to select item, use `|` as a delimiter for the
  sub-menus. For example: 'File|Save as|PDF'
### Response

JSON format

### Examples

```shell
# press the "next" button
curl -X POST 'http://localhost:9999/v1/widgets?id=next&action=press'
# press the "next" button, filter by type and id
curl -X POST 'http://localhost:9999/v1/widgets?type=YPushButtonid=next&action=press'
# set value "test" for the InputField with label "Description"
curl -X POST 'http://localhost:9999/v1/widgets?label=Description&action=enter_text&value=test'
# select first row (counting from zero) in table with id "names"
curl -X POST 'http://localhost:9999/v1/widgets?id=names&action=select&row=1'
# select row with "test" cell value in the 2-nd column (counting from zero) in table with id "names"
curl -X POST 'http://localhost:9999/v1/widgets?id=names&action=select&value=test&column=2'
# select row with "sub_item_row" cell value, which is child row of "root_row" cell value in table with id "names"
curl -X POST 'http://localhost:9999/v1/widgets?id=names&action=select&value=root_row|sub_item_row'
# select tree item with in tree with id "files" and path 'root|subnode|subnode
curl -X POST 'http://localhost:9999/v1/widgets?id=files&action=select&value=root%7Csubnode%7Csubnode'
# press url (<a href=\"firewall\">(enable)</a>) in richtext
curl -X POST 'http://localhost:9999/v1/widgets?type=YRichText&action=select&value=firewall'
# select menu item with label "Image" in parent menu item with label "Document" in menu button
curl -X POST 'http://localhost:9999/v1/widgets?type=YMenuButton&action=select&value=Document%7CImage'
# select menu bar item with label "&Folder" in parent menu item with label "&Create" in menu bar
curl -X POST 'http://localhost:9999/v1/widgets?type=YMenuBar&action=select&value=%26Create%7C%26Folder'
```
