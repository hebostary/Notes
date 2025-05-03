#!/bin/bash

# Function to connect to TCP server
connect_to_server() {
    local server="$1"
    local port="$2"
    local message="$3"
    local clientid="$4"

    # Connect to TCP server using netcat
    echo "Connecting to $server:$port..."
    echo "$message" | nc -v -w 10 "$server" "$port"

    # Check if connection was successful
    if [ $? -eq 0 ]; then
        echo "[Client $clientid] Connection to $server:$port successful."
    else
        echo "[Client $clientid] Failed to connect to $server:$port."
    fi
}

# Define TCP server details
server="127.0.0.1"
port="12345"
message="Hello, TCP server!"

# Number of concurrent connections
num_connections=500

# Run concurrent connections
for ((i = 1; i <= $num_connections; i++)); do
    connect_to_server "$server" "$port" "$message" "$i" &
done

# Wait for all connections to finish
wait

echo "All connections completed."


#
# ./epoll_client.sh
#