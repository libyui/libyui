## Table of Contents
* [libyui-rest-api](#libyui-rest-api)
    * [Features](#features)
    * [TODO](#todo)
    * [Usage](#usage)
        * [Remote Access](#remote-access)
        * [User Authentication](#user-authentication)
    * [Contributing](#contributing)
    * [Building](#building)
    * [Testing](#testing)
* [License](#license)

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
((libyui-ncurses-rest-api)[https://github.com/libyui/libyui-ncurses-rest-api]
or (libyui-qt-rest-api)[https://github.com/libyui/libyui-ncurses-rest-api]).

### Features

- Optional plugins which extend the standard libyui library
  - Less dependencies
  - Can be installed only when needed
- Can read the whole structure of the currently displayed dialog
- Can query only the selected widgets
- Allows sending the user input (clicking buttons, entering text,...)
- Uses the standard HTTP protocol and the JSON data format
  - Not bound to any specific programming language or framework
  - Easy integration with any testing framework
- Optional remote access (by default accessible only from the same machine)
- Optional client authentication (HTTP Basic Auth)
  - :warning: But without any encryption it is still sent in clear text!
- IPv6 support (quite interesting for testing virtual machines, the IPv6 link-local
  address is based on the MAC address, you can easily get the IPv6 address for
  your testing machine)

### TODO

- [ ] Properties of some widgets are still missing
- [ ] Allow sending more user actions
- [ ] Some widgets do not send notify events when changed via the API
- [ ] SSL encryption/peer verification (needed for secure transferring of sensitive data
    like passwords)
- [ ] Allow connection via Unix domain sockets

### Usage

To start the application with rest API enabled, use the following commands:
* `xdg-su -c 'YUI_HTTP_PORT=9999 yast2 host'` for Qt
* `sudo YUI_HTTP_PORT=9999 yast2 host` for ncurses.

After that, you can get the documentation how to interact with the UI by accessing
http://localhost:9999 (or http://ipv6-localhost:9999 via IPv6).

NOTE: For MultiItemSelector and CustomItemSelector, rest-api doesn't work as expected
in ncurses with `notify` set to true, when using pure C++ code. This limitation is
due to widget implementation. With ruby wrapper, there is no issue.

### Remote Access

By setting `YUI_HTTP_REMOTE=1` environmental variable, one can allow connections
from remote hosts.

:warning: Security warning: Enable the remote access only in trusted environment,
do not use it for production systems!

### User Authentication

The REST API supports user authentication via the [HTTP Basic Authentication](
https://en.wikipedia.org/wiki/Basic_access_authentication).

The allowed user name and password can be set using the `YUI_AUTH_USER` and the
`YUI_AUTH_PASSWD` environment variables. It is possible to configure only single
access credentials.

:warning: Security warning: Currently the user name and the password is sent
in clear text without any encryption (only converted to the Base64 encoding).
That means anybody on the way could read the user name and the password
([MITM attack](https://en.wikipedia.org/wiki/Man-in-the-middle_attack)).

### Reuse of the socket

By default server side has `MHD_OPTION_LISTENING_ADDRESS_REUSE` set `0`,
so port reuse by other processes is NOT allowed for security reasons.
In case of quick restart of the app on the same port, it might be still binded
and `Failed to bind to port XXXX: Address already in use` error is thrown.
Most of the systems have timeout of 60 second before port can be reused,
run `sysctl net.ipv4.tcp_fin_timeout` to check the value on the system.

In order to allow reusing socket by other processes, set `YUI_REUSE_PORT` to `1`.
For example:
```
YUI_REUSE_PORT=1 YUI_HTTP_PORT=9999 /sbin/yast2 examples/Table5.rb --qt
```

## Contributing

1. Fork it
2. Create your feature branch (git checkout -b my-new-feature)
3. Commit your changes (git commit -am 'Add some feature')
4. Push to the branch (git push origin my-new-feature)
5. Create new Pull Request

Please, keep coding style consistent, in case of doubts, please, refer to
[CppCoreGuidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

## Building

In order to build project locally one can use `make`:
* `make -f Makefile.cvs`
* `make -C build/ install`

Recommended way is to use `rake`, please refer to
[libyui Building section](https://github.com/libyui/libyui#building).

Also, see [yast-rake](https://github.com/yast/yast-rake) documentation for
useful tasks, like `rake version:bump`.
One can run `rake --tasks` locally to see all available tasks.

## Testing

In order to test changes, one can use yast modules in her system.
As an alternative, examples from `https://github.com/yast/yast-ycp-ui-bindings`
project can be used.

For running those examples `yast2-core` package has to be preinstalled.

After `yast-ycp-ui-bindings` project is checked out or `yast2-ycp-ui-bindings-devel`
package is installed, navigate to the directory with examples. In case of
`yast2-ycp-ui-bindings` package, this is normally
`/usr/share/doc/packages/yast2-ycp-ui-bindings/`, otherwise simply open directory
with cloned github repo.

For instance, to run `Table5.rb`, you can use following command:
```
YUI_HTTP_PORT=9999 /sbin/yast2 examples/Table5.rb --ncurses
```

To run qt version of the app, simply replace `ncurses` parameter with `qt` as
follows:
```
YUI_HTTP_PORT=9999 /sbin/yast2 examples/Table5.rb --qt
```

After that server should be available on the provided port and http request can
be sent to it.

# License
This package is licensed under
[LGPL-2.1](http://www.gnu.org/licenses/lgpl-2.1.html).
