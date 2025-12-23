# Stage 1: Build stage
FROM debian:trixie-slim AS build

# Install build-essential for compiling C++ code
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
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

# Logging to make sure build files exist
RUN ls -l /app/build
RUN ls -l /app/build/bin
RUN ls -l /app/build/bin/www

# Stage 2: Runtime stage
FROM scratch

# Copy the static binary from the build stage
COPY --from=build /app/build/bin/sharepaste /sharepaste

# Copy the front end files
COPY --from=build /app/build/bin/www /www

# Expose the port on which the API will listen
EXPOSE 8080

# MAake sure sharepaste exists
RUN ["/sharepaste", "--test"]


# Command to run the binary
CMD ["/sharepaste"]