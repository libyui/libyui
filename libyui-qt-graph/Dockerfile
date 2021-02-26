# Use the libyui-devel image as the base
FROM registry.opensuse.org/devel/libraries/libyui/sle-15/sp2/containers/libyui-devel

RUN zypper --non-interactive in --no-recommends \
  graphviz-devel \
  && zypper clean -a

COPY . /usr/src/app
