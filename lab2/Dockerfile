# syntax=docker/dockerfile:1

FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libfmt-dev \
    libpq-dev \
    libspdlog-dev \
    ninja-build \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja \
    && cmake --build build -j"$(nproc)"

FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest AS api

RUN apt-get update && apt-get install -y \
    libfmt8 \
    libpq5 \
    libspdlog1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/fitness_tracker_api /usr/local/bin/fitness_tracker_api
COPY configs/config.yaml /etc/fitness_tracker/config.yaml

EXPOSE 8080
ENTRYPOINT ["/usr/local/bin/fitness_tracker_api", "--config", "/etc/fitness_tracker/config.yaml"]
