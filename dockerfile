# Stage 1: Build stage
FROM debian:bookworm-slim AS build

# Install build-essential for compiling C++ code
RUN apt-get update && apt-get install -y build-essential \
    clang \
    cmake

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY hello.cpp .

# Compile the C++ code statically to ensure it doesn't depend on runtime libraries
RUN clang++ -std=c++20  hello.cpp -static -O3 -o hello




# Stage 2: Runtime stage
FROM scratch

# Copy the static binary from the build stage
COPY --from=build /app/hello /hello

# Command to run the binary
CMD ["/hello"]