# Build Docker container for nmon's local test
# curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
# sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
# sudo gpasswd -a $USER docker
# newgrp docker
# docker build -t matteyeux/nmon_test .
# docker run --rm -ti matteyeux/nmon_test

FROM ubuntu:16.04

RUN apt-get update  --fix-missing
RUN apt install git sudo -y
RUN git clone https://github.com/white-wolf-project/ragnarok
WORKDIR /ragnarok
RUN ./scripts/setup_env.sh
RUN make
