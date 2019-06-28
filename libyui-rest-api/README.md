# libyui-rest-api

Libyui UI REST API framework for integration testing.
Project started by @lslezak, with support of @cwh42 and @OleksandrOrlov.

The solution allows to query the UI properties over HTTP using a REST API.
This allows to automate the UI interaction steps and avoid screen-based tools.
The API allows reading properties of the UI, so the displayed values can be validated.
It also allows interacting with the UI (clicking buttons, toggling
check boxes, entering text,...).

This repository contains the shared functionlity, you need to install
the additional bindings for the specific UI frontends
((libyui-ncurses-rest-api)[https://github.com/libyui/libyui-ncurses-rest-api] or
or (libyui-qt-rest-api)[https://github.com/libyui/libyui-ncurses-rest-api]).

### Features

- Optional plugins which extend the standard libyui library
  - Less dependencies
  - Can be installed only when needed
- Uses the standard HTTP protocol
- The data is sent using the JSON data format
- Optional remote access
- IPv6 support
- Can read the whole structure of the currently displayed dialog
- Can query only the selected widgets
- Allows sending the user input (clicking buttons, entering text,...)

### TODO

- [ ] Properties of some widgets are missing
- [ ] Allow sending more user actions
- [ ] Some widgets do not send notify events when changed via API
- [ ] Authentication support (avoid unauthorized access)
- [ ] SSL encryption/peer verification (needed for transferring sensitive data
      like passwords)

### Usage

To start the application with rest API enabled, use the following commands:
* `xdg-su -c 'YUI_HTTP_PORT=9999 yast2 host'` for QT
* `sudo YUI_HTTP_PORT=9999 yast2 host` for ncurses.

After that, you can get the documentation how to interact with the UI by accessing
http://localhost:9999`.(or http://ipv6-localhost:9999 via IPv6).

### Remote Access

By setting `YUI_HTTP_REMOTE=1` environmental variable, one can allow connections
from remote hosts.

:warning: Security warning: Enable the remote access only in trusted environment,
do not use it for production systems!


---
<h1>LibYUI Embedded Webserver</h1>
<p>This webserver provides a REST API for the LibYUI application.</p>
<p>It can be used for testing and controlling the application in automated tests.</p>
<br>
<h2>Short Documentation</h2>
<h3>Application</h3>
<p>Request: <pre>GET /application</pre>
</p>
<h4>Description</h4>
<p>Get the application and UI generic properties like text or graphical mode, dialog size, screen size and supported UI featues.</p>
<h4>Response</h4>
<p>JSON format</p>
<h4>Examples</h4>
<p>
        <pre>curl http://localhost:9999/application</pre>
</p>
<hr>
<h3>Dump Whole Dialog</h3>
<p>Request: <pre>GET /dialog</pre>
</p>
<h4>Description</h4>
<p>Get the complete dialog structure in the JSON format. The result contains a nested structure exactly following the structure of the current dialog.</p>
<h4>Response</h4>
<p>JSON format</p>
<h4>Examples</h4>
<p>
        <pre>curl http://localhost:9999/dialog</pre>
</p>
<hr>
<h3>Read Specific Widgets</h3>
<p>Request: <pre>GET /widgets</pre>
</p>
<h4>Description</h4>
<p>Return only the selected widgets (in JSON format). The result is a flat list (no nested structures).</p>
<h4>Parameters</h4>
<p>Filter widgets: <ul>
                <li>
                        <b>id</b> - widget ID serialized as string, might include special characters like backtick (\`)</li>
                <li>
                        <b>label</b> - widget label as currently displayed (i.e. translated!) </li>
                <li>
                        <b>type</b> - widget type</li>
        </ul>
</p>
<h4>Response</h4>
<p>JSON format</p>
<h4>Examples</h4>
<p>
        <pre>curl 'http://localhost:9999/widgets?id=next'</pre>
        <pre>curl 'http://localhost:9999/widgets?label=Next'</pre>
        <pre>curl 'http://localhost:9999/widgets?type=YCheckBox'</pre>
</p>
<hr>
<h3>Change Widgets, Do an Action</h3>
<p>Request: POST /widgets</p>
<h4>Description</h4>
<p>Do an action with specified widgets.</p>
<h4>Parameters</h4>
<p>Filter the widgets, one of: <ul>
                <li>
                        <b>id</b> - widget ID serialized as string, might include special characters like backtick (\`)</li>
                <li>
                        <b>label</b> - widget label as currently displayed (i.e. translated!) </li>
                <li>
                        <b>type</b> - widget type</li>
        </ul> Then specify the action: <ul>
                <li>
                        <b>action</b> - action to do</li>
                <li>
                        <b>value</b> (optional) - new value or a parameter of the action</li>
        </ul>
</p>
<h4>Response</h4>
<p>JSON format</p>
<h4>Examples</h4>
<p>
        Press the "next" button:
        <pre>curl -X POST 'http://localhost:9999/widgets?id=next&action=press'</pre>
        Set value "test" for the InputField with label "Description":
        <pre>curl -X POST 'http://localhost:9999/widgets?label=Description&action=enter&value=test'</pre>
</p>
