#!/bin/bash

../bin/agml_server_clustering model "$(cat kmeans_single_host.model)" || exit 2

echo
echo "Everything went fine !"
echo " -> Codebooks for each node and evolution of MSE have been stored in the current working directory"

