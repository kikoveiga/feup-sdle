# SDLE Assignment

SDLE Assignment of group T07G14.

Group members:

1. Bruno Machado (up201907715@up.pt)
2. João Padrão (up202108766@up.pt)
3. José Francisco Veiga (up202108753@up.pt)

## Introduction

- This project, entitled **Shopping Lists on the Cloud**, centres on the implementation of a distributed system that uses conflict-free replicated data types (CRDTs) to guarantee consistency between replicas in a shopping list management application. The application allows users to create, share and manage shopping lists collaboratively, even in offline-first scenarios.

**Main aspects of the project:**

- **CRDT implementation:** Use of CCounters to manage item quantities in a conflict-free way.

- **Distributed architecture:** Based on ZeroMQ for robust communication between clients and servers, employing the **Paranoid Pirate Pattern** to increase fault tolerance.

- **Persistence and replication:** MongoDB is used to persist data, guaranteeing durability and allowing recovery after failures.

- **Objective:** To demonstrate the viability of cloud-supported, conflict-free collaborative applications with minimal friction for the user.

## Execution

### Prerequisites

To build and run the project, ensure the following software is installed:

1. **CMake** (version 3.30 or higher)  
   You can download and install CMake from the official website:  
   [CMake Downloads](https://cmake.org/download/)

2. **MongoDB C++ Driver** (mongocxx and bsoncxx)  
   Follow the instructions to install the MongoDB C++ Driver:  
   [MongoDB C++ Driver Installation Guide](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started/download-and-install/)

3. **ZeroMQ** and **zmqpp** library  
   - To install **ZeroMQ**, follow the instructions from the official website:  
     [ZeroMQ Installation Guide](https://zeromq.org/get-started/)
   - For **zmqpp**, you can follow these installation steps:  
     [zmqpp Installation Guide](https://github.com/zeromq/zmqpp#installation)

### Build Instructions

1. Clone the repository and navigate to the project directory:

   ```bash
   git clone <repository_url>
   cd g14