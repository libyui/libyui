# LibYUI REST API v1

This is the specification of the version 1 of the API.

## API Documentation

### API Version

Request: `GET /version`

#### Description

Get the application and UI generic properties like text or graphical mode,
dialog size, screen size and supported UI features.

#### Response

JSON format

```json
{
  "api_version" : "v1"
}
```

The `api_version` value defines the version of the API. It is used in the URL
as the path prefix.

##### Examples

```
curl http://localhost:9999/version
```
---

### Application Data

Request: `GET /v1/application`

#### Description

Get the application and UI generic properties like text or graphical mode,
dialog size, screen size and supported UI features.

#### Response

JSON format

##### Examples

```
curl http://localhost:9999/v1/application
```
---

### Dump Whole Dialog
 
 Request: `GET /v1/dialog`

#### Description

Get the complete dialog structure in the JSON format. The result contains
a nested structure exactly following the structure of the current dialog.

#### Response

JSON format

##### Examples

```
curl http://localhost:9999/v1/dialog
```

---

### Read Only Specific Widgets

Request: `GET /v1/widgets`

#### Description

Return only the selected widgets (in JSON format). The result is a flat list
(no nested structures).

#### Parameters

Filter widgets:

- **id** - the widget ID serialized as string, might include special characters
  like backtick (\`)
- **label** - widget label as currently displayed (i.e. translated!)
- **type** - the widget type

#### Response

JSON format

#### Examples

```
curl 'http://localhost:9999/v1/widgets?label=Next
curl 'http://localhost:9999/v1/widgets?id=next
curl 'http://localhost:9999/v1/widgets?type=YCheckBox
```

---

### Change Widgets, Do an Action</h3>

Request: `POST /v1/widgets`

#### Description

Do an action with specified widgets.

#### Parameters

Filter the widgets, one of:

- **id** - widget ID serialized as string, might include special characters
  like backtick (\`)
- **label** - widget label as currently displayed (i.e. translated!)
- **type** - type of the widget

Then specify the action:

- **action** - the action to do
- **value** (optional) - new value or a parameter of the action
- **column** (optional) - column id when selecting item in the table

Supported actions:

- **press** - to press the button
- **check** | **uncheck**  | **toggle** - check, uncheck or toggle checkbox
- **enter_text** - set text in the field, the text is passed in the
  *value* parameter
- **switch_radio** - activate radio button
- **select** - select value in the combobox, row in the table or node in the
  tree, requires *value*parameter
  - In case of table: select row in the table with given value. If
        *column* parameter is not provided, the first column will be used.  
  - In case of tree: select node in the tree. Use `|` as a delimiter for
        the child nodes.

#### Response

JSON format

#### Examples

```shell
# press the "next" button
curl -X POST 'http://localhost:9999/v1/widgets?id=next&action=press'
# set value "test" for the InputField with label "Description"
curl -X POST 'http://localhost:9999/v1/widgets?label=Description&action=enter_text&value=test'
# select row with "test" cell value in the 2-nd column (counting from zero) in table with id "names"
curl -X POST 'http://localhost:9999/v1/widgets?id=names&action=select&value=test&column=2'
# select tree item with in tree with id "files"
curl -X POST 'http://localhost:9999/v1/widgets?id=files&action=select&value=root|subnode|subnode'
```