FROM debian:bullseye-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    valgrind \
    libreadline-dev \
    bash \
    tmux \
    gdb \
    gdbserver \
    make \
    git \
    strace \
    libncurses-dev \
    gettext \
    less \
    python3 \
    python3-pip \
    && pip3 install gdbgui \
    && apt-get clean

WORKDIR /src

EXPOSE 5000

CMD ["bash"]

