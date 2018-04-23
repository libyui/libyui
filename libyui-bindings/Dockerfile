# Use the libyui/devel image as the base
FROM libyui/devel:sle12-sp4


RUN zypper --non-interactive in --no-recommends \
  python-devel \
  ruby-devel \
  swig \
  && zypper clean -a

COPY . /usr/src/app
