# Stage 1: Build stage
FROM debian:trixie-slim AS build

# Install build-essential for compiling C++ code
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    ninja-build

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY src ./src
COPY CMakeLists.txt .

# Create build directory
RUN mkdir build
WORKDIR /app/build

# Compile the C++ code statically to ensure it doesn't depend on runtime libraries

# RUN cmake -DCMAKE_BUILD_TYPE=Release .. 
#     && cmake --build . -- -j$(nproc)

RUN cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. \
    && cmake --build . -- -j$(nproc)


# Stage 2: Runtime stage
FROM scratch

# Copy the static binary from the build stage
# COPY --from=build /app/hello /hello
COPY --from=build /app/build/sharenote /sharenote


# Expose the port on which the API will listen
EXPOSE 80

# MAake sure sharenote exists
RUN ["/sharenote", "--help"]

# Command to run the binary
CMD ["/sharenote"]