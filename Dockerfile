# Use Ubuntu as the base image
FROM ubuntu:latest

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Update and install build tools required for compiling C programs
RUN apt-get update && apt-get install -y \
    build-essential \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /usr/src/mysh

# Copy the local source files to the Docker container
COPY . .

# Compile the shell using make
RUN make

# Copy a startup script
COPY startup.sh /usr/local/bin/startup.sh
RUN chmod +x /usr/local/bin/startup.sh

# Set the command to run when the container starts
CMD ["/usr/local/bin/startup.sh"]
