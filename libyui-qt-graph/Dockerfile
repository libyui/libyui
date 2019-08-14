# Use the libyui/devel image as the base
FROM libyui/devel:sle12-sp5



RUN zypper --non-interactive in --no-recommends \
  graphviz-devel \
  && zypper clean -a

COPY . /usr/src/app
