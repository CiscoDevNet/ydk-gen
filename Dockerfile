FROM ubuntu:jammy

MAINTAINER http://ydk.io

COPY . /root/ydk-gen

WORKDIR /root/ydk-gen

RUN echo 'Installing dependencies'

RUN /bin/bash -c './test/dependencies_ubuntu.sh && ./test/dependencies_linux_gnmi.sh'

RUN pip3 install -r requirements.txt

RUN python3 generate.py -i --core --cpp
RUN python3 generate.py -i --service profiles/services/gnmi-0.4.0.json --cpp

RUN python3 generate.py -i --core
RUN python3 generate.py -i --service profiles/services/gnmi-0.4.0.json
RUN pip3 install ydk-models-openconfig

RUN /bin/bash -c './install_ydk.sh --go --core --service gnmi'

RUN /bin/bash -c './test/clean_builds.sh'

RUN ln -s /usr/bin/python3 /usr/bin/python
