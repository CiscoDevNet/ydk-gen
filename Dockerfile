FROM ubuntu:bionic

COPY . /root/ydk-gen

RUN echo 'Installing dependencies'

WORKDIR /root/ydk-gen

RUN /bin/bash -c './test/dependencies_ubuntu.sh && ./test/dependencies_linux_gnmi.sh'

RUN pip3 install -r requirements.txt

RUN python3 generate.py -i --core --cpp
RUN python3 generate.py -i --service profiles/services/gnmi-0.4.0.json --cpp

RUN python3 generate.py -i --core
RUN python3 generate.py -i --service profiles/services/gnmi-0.4.0.json
RUN pip3 install ydk-models-openconfig

RUN export GOROOT=/usr/local/go && export PATH=$GOROOT/bin:$PATH
RUN export GOPATH=/root/golang
RUN python3 generate.py -i --core --go
RUN python3 generate.py -i --service profiles/services/gnmi-0.4.0.json --go

RUN ln -s /usr/bin/python3 /usr/bin/python
