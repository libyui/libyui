# TODO

## Important

- [ ] Split the package into separate Ncurses and Qt parts
(less dependencies, not possible to test a minimal system without X)
- [ ] Improve the plugin loading (it requires to pass the `ui`
UI and decide in libyui whether to use the Qt/ncurses with REST API)
- [ ] Support for more widgets and attributes

## Security

- [ ] User authentication via basic auth([tutorial](
https://www.gnu.org/software/libmicrohttpd/tutorial.html#Supporting-basic-authentication),
[documentation](https://www.gnu.org/software/libmicrohttpd/manual/html_node/microhttpd_002ddauth-basic.html#microhttpd_002ddauth-basic),
[example](https://github.com/Metaswitch/libmicrohttpd/blob/master/src/examples/authorization_example.c))
- [ ] SSL for encryption and peer verification ([tutorial](https://www.gnu.org/software/libmicrohttpd/tutorial.html#Adding-a-layer-of-security))


## Optional

- [ ] Support also the Gtk UI
- [ ] IPv6 support ([example](https://github.com/rboulton/libmicrohttpd/blob/master/src/examples/dual_stack_example.c))
