FROM ubuntu:xenial

MAINTAINER http://ydk.io

COPY . /root/ydk-gen

RUN echo 'Installing dependencies'

WORKDIR /root/ydk-gen

RUN /bin/bash -c './test/dependencies_ubuntu_basic.sh && pip install -r requirements.txt && ./generate.py --libydk && make -C gen-api/cpp/ydk/build install && ./generate.py --core --python && pip install gen-api/python/ydk/dist/ydk*.tar.gz'
