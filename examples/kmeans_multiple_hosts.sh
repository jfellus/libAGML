#!/bin/bash

if [ -z "$1" ]; then
    echo "$0 <host number>"
    echo "Run $0 2 in a first shell, and then $0 1 in a second shell" 
    exit 2
fi

if [[ "$1" == 1 ]]; then
    ../bin/agml_server_clustering -p 10001 model "$(cat kmeans_multiple_host.model)" || exit 2
elif [[ "$1" == 2 ]]; then
    ../bin/agml_server_clustering -p 10002 || exit 2
else
    echo "Invalid host number"
    exit 2
fi

echo
echo "Everything went fine !"
echo " -> Codebooks for each node and evolution of MSE have been stored in the current working directory"
