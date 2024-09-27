FROM ubuntu:22.04

RUN apt update && apt install -y \
        python3 \
        python3-pip \
        python3.10-venv \
        git \
        cmake

RUN pip3 install --upgrade pip setuptools

RUN pip3 install -U platformio==6.1.11

WORKDIR /app

COPY . .

ENV USER=root

RUN git config --global --add safe.directory /app && \
    git config --global --add safe.directory /app/lib/encoder-mbed && \
    git config --global --add safe.directory /app/lib/imu-driver && \
    git config --global --add safe.directory /app/lib/motor-driver-mbed && \
    git config --global --add safe.directory /app/lib/vl53l0x-mbed && \
    git submodule update --init --recursive && \
    pio lib install

RUN python3 -m venv /root/.platformio/penv/bin/activate

CMD pio run