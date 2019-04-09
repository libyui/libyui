# libyui-rest-api

YaST UI rest api framework for integration testing.
Project started by @lslezak, with support of @cwh42 and @OleksandrOrlov.

Solution allows to query UI properties over http using API. This allows to
automate UI interaction steps and avoid screen-based tools.
API allows reading properties of the UI, so entered text can be validated.

To start application with rest API enabled, use following commands:
* `xdg-su -c 'Y2TEST=1 YUI_HTTP_PORT=9999 yast2 host'` for QT
* `sudo Y2TEST=1 YUI_HTTP_PORT=9999 yast2 host` for ncurses.

After that, you can get documentation how to interact with UI by accessing
`http://localhost:9999`.

By setting `YUITest_HTTP_REMOTE=1` environmental variable, one can allow connections
from the remote host.


<body>
        <h1>LibYUI Embedded Webserver</h1>
        <p>This webserver provides a REST API for the LibYUI application.</p>
        <p>It can be used for testing and controlling the application in automated tests.</p>
        <br>
        <h2>Short Documentation</h2>
        <h3>Application</h3>
        <p>Request: GET <a href='/application'>/application</a>
        </p>
        <h4>Description</h4>
        <p>Get the application and UI generic properties like text or graphical mode, dialog size, screen size and supported UI featues.</p>
        <h4>Response</h4>
        <p>JSON format</p>
        <h4>Examples</h4>
        <p>
                <pre>`  curl http://localhost:9999/application`</pre>
        </p>
        <hr>
        <h3>Dump Whole Dialog</h3>
        <p>Request: GET <a href='/dialog'>/dialog</a>
        </p>
        <h4>Description</h4>
        <p>Get the complete dialog structure in the JSON format. The result contains a nested structure exactly following the structure of the current dialog.</p>
        <h4>Response</h4>
        <p>JSON format</p>
        <h4>Examples</h4>
        <p>
                <pre>`  curl http://localhost:9999/dialog`</pre>
        </p>
        <hr>
        <h3>Read Specific Widgets</h3>
        <p>Request: GET <a href='/widgets'>/widgets</a>
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
                <pre>`  curl 'http://localhost:9999/widgets?id=next'`</pre>
                <pre>`  curl 'http://localhost:9999/widgets?label=Next'`</pre>
                <pre>`  curl 'http://localhost:9999/widgets?type=YCheckBox'`</pre>
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
                <pre>` press the next button  curl -X POST 'http://localhost:9999/widgets?id=next&action=press'`</pre>
                <pre>` set value "test" for the InputField with label "Description"  curl -X POST 'http://localhost:9999/widgets?label=Description&action=enter&value=test'`</pre>
        </p>
</body>
