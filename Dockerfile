FROM ubuntu:latest

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make

# Create working directory
WORKDIR /app

# Copy all source files
COPY . .

# Build the project
RUN make clean && make all

# Command to run the router_dealer
CMD ["./router_dealer"]
