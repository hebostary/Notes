#!/bin/bash

END_POINTS="etcd1:2379,etcd2:2379,etcd3:2379"

echo "[INFO] Start etcd cluster with 3 members..."
rm -rf ./etcd1.data ./etcd2.data ./etcd3.data
mkdir -p ./etcd1.data ./etcd2.data ./etcd3.data

docker-compose down
docker-compose up -d

echo "[INFO] Check etcd cluster memebers..."
docker exec -t etcd1 etcdctl member list --endpoints=$END_POINTS
docker exec -t etcd1 etcdctl --write-out=table \
--endpoints=$END_POINTS endpoint status