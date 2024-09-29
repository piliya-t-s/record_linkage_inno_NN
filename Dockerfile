# For more information, please refer to https://aka.ms/vscode-docker-python

# First stage: build the program
FROM gcc:latest as builder

# Install OpenSSL development libraries
RUN apt-get update && \
    apt-get install -y libssl-dev


# Set the working directory
WORKDIR /app

# Copy source code and compile the program
COPY . /app
RUN gcc -o sha1_example sha1_example.c -lssl -lcrypto


FROM python:3-slim

# Keeps Python from generating .pyc files in the container
ENV PYTHONDONTWRITEBYTECODE=1

# Turns off buffering for easier container logging
ENV PYTHONUNBUFFERED=1


# Install pip requirements
COPY requirements.txt .
RUN apt-get update && \
    python -m pip install -r requirements.txt

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get -y install gcc mono-mcs && \
    rm -rf /var/lib/apt/lists/*

RUN gcc -o main.c run -lssl -lcrypto   
    
WORKDIR /app
COPY . /app

# Creates a non-root user with an explicit UID and adds permission to access the /app folder
# For more info, please refer to https://aka.ms/vscode-docker-python-configure-containers
RUN adduser -u 5678 --disabled-password --gecos "" appuser && chown -R appuser /app
USER appuser


# During debugging, this entry point will be overridden. For more information, please refer to https://aka.ms/vscode-docker-python-debug
CMD ["python", "-m", "src.main"]
