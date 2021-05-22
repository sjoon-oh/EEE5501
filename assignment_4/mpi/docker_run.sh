#!/bin/bash

# docker rm $(docker ps -a -q)
# docker run --name my-mpi -d -it ubuntu:mpi bash
docker run --name my-mpi -it ubuntu:mpi bash
# docker exec -it my-mpi bash
