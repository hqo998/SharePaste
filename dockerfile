# Stage 1: Build stage
FROM debian:trixie-slim AS build

# Install build-essential for compiling C++ code
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    cmake \
    ninja-build

# installl npm for building the front end
RUN apt-get install -y npm && apt-get clean

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY src ./src
COPY CMakeLists.txt .

# TODO temp : remove old frontend files to avoid confusion. Once react is fully integrated remove this.
RUN rm -rf /src/frontend

# Create build directory
RUN mkdir build

WORKDIR /app
COPY Web_Client ./Web_Client
WORKDIR /app/Web_Client

# Install dependencies and build the front end
RUN npm ci

RUN npm run build

# Compile the C++ code statically to ensure it doesn't depend on runtime libraries
WORKDIR /app/build
RUN cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. \
    && cmake --build . -- -j$(nproc)

RUN ls -l /app/Web_Client/dist

# =====================================

# Stage 2: Runtime stage
FROM scratch

# Copy the static binary from the build stage
COPY --from=build /app/build/bin/sharepaste /sharepaste

# Copy the front end files
COPY --from=build /app/Web_Client/dist /

# Expose the port on which the API will listen
EXPOSE 8080

# Make sure sharepaste exists
RUN ["/sharepaste", "--test"]


# Command to run the binary
CMD ["/sharepaste"]