## Run docker-compose down -v && docker-compose up --build if u wanna use docker
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential \
    gcc \
    libc6-dev \
    manpages-dev \
    libpthread-stubs0-dev \
    libaio-dev \
    libcap-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

RUN gcc -o router-dealer main.c -lrt

CMD ["./router-dealer"]
