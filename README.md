# SDLE Assignment

SDLE Assignment of group T07G14.

Group members:

1. Bruno Machado (up201907715@up.pt)
2. João Padrão (up202108766@up.pt)
3. José Francisco Veiga (up202108753@up.pt)

## Introduction

- This project, entitled **Shopping Lists on the Cloud**, centres on the implementation of a distributed system that uses conflict-free replicated data types ([CRDTs](https://en.wikipedia.org/wiki/Conflict-free_replicated_data_type)) to guarantee consistency between replicas in a shopping list management application. The application allows users to create, share and manage shopping lists collaboratively, even in offline-first scenarios.

## Main Features

- **CRDT implementation:** Use of CCounters to manage item quantities in a conflict-free way.

- **Distributed architecture:** Based on ZeroMQ for robust communication between clients and servers, employing the [Paranoid Pirate Pattern](https://zguide.zeromq.org/docs/chapter4/) to increase fault tolerance. The server is composed of a broker and multiple workers. The broker is responsible for load balancing the clients' requests among the workers, which are responsible for processing them.

- **Persistence and replication:** MongoDB is used to persist data on the server side, guaranteeing durability and allowing recovery after failures.

- **Objective:** To demonstrate the viability of cloud-supported, conflict-free collaborative applications with minimal friction for the user.

## Execution

Below are the instructions to build and run the project. This is mainly focused on Ubuntu, but the links below also provide instructions for other operating systems.

### Prerequisites

To build and run the project, ensure the following software is installed:

- **CMake** (version 3.30 or higher): you can download and install CMake from the [official website](https://cmake.org/download/).

- **MongoDB**: Follow the instructions on the official website, [here](https://www.mongodb.com/docs/manual/tutorial/install-mongodb-on-ubuntu/) is a guide to install it on Ubuntu.

- **MongoDB C++ Driver** (mongocxx and bsoncxx): [here](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/linux/) is a guide to install it on Ubuntu. Make sure to install it with the Boost polyfill option.

### Build Instructions

- Clone the repository and navigate to the project directory:

   ```bash
   git clone <repository_url>
   cd g14
   ```

- Init the submodules:

   ```bash
   git submodule update --init --recursive
   ```

- Build the project using CMake:

   ```bash
   cmake -S . -B build
   cmake --build build
   ```

### Run Instructions

- Start the MongoDB server:

   ```bash
   sudo systemctl start mongod
   ```

- Run the broker:

   ```bash
   cd build
   ./main_broker
   ```

- Run the workers:

   ```bash
   ./main_worker
   ```

- Run the client:

   ```bash
   ./client
   ```
