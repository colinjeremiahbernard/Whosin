FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates cmake g++ git libasio-dev make ninja-build \
    && rm -rf /var/lib/apt/lists/*
RUN git clone --depth 1 --branch v1.3.2 https://github.com/CrowCpp/Crow.git /tmp/crow \
    && cmake -S /tmp/crow -B /tmp/crow-build -G Ninja \
       -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF -DCROW_INSTALL=ON \
       -DCROW_ENABLE_SSL=OFF -DCROW_ENABLE_COMPRESSION=OFF \
    && cmake --install /tmp/crow-build
WORKDIR /source
COPY CMakeLists.txt ./
COPY src/ ./src/
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel 2

FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --gid 10001 whosin \
    && useradd --uid 10001 --gid whosin --no-create-home whosin
WORKDIR /app
COPY --from=build /source/build/whosin ./whosin
COPY static/ ./static/
USER whosin
ENV PORT=8080
EXPOSE 8080
HEALTHCHECK --interval=30s --timeout=3s --start-period=10s --retries=3 \
    CMD curl --fail --silent "http://127.0.0.1:${PORT}/health" || exit 1
CMD ["./whosin"]
