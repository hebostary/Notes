#!/bin/bash

END_POINTS="etcd1:2379,etcd2:2379,etcd3:2379"


echo "[INFO] Add 1 new member to cluster..."
docker exec -t etcd1 etcdctl member add etcd4 \
--endpoints=$END_POINTS \
--peer-urls=http://172.16.238.14:2380


echo "[INFO] Start etcd new member..."
rm -rf ./etcd4.data
mkdir -p ./etcd4.data

docker-compose down
docker-compose up -d

echo "[INFO] Check new etcd cluster memebers..."
END_POINTS="etcd1:2379,etcd2:2379,etcd3:2379,etcd4:2379"
docker exec -t etcd1 etcdctl member list --endpoints=$END_POINTS
docker exec -t etcd1 etcdctl endpoint status \
--write-out=table \
--endpoints=$END_POINTS 