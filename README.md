Implementation of a somewhat homomorphic encryption system using an FPGA hardware accelerator.
This project was part of my bachelor's thesis at KIT and developed in four months.

The Repository consists of multiple smaller projects, that each have their own way of running and installation requirements.
Refer to the README.md files in the project folders for further instructions.

The available projects are:
- SHE_Server: A modified implementation of [Roy et al.](https://github.com/KULeuven-COSIC/HEAT/tree/master) that runs on the ZCU102 FPGA board and prints timings when recieving ciphertexts and relin keys. All rights for the pre-existing code lay by the original publishers.
- SHE_Client: The client used for transmission timings. If you want to take measurements on the server, do it with this client implementation.
- SHE_SEAL: A [SEAL](https://github.com/microsoft/SEAL) demo used to take timings in order to compare them with the FPGA.
- SHE_OpenFHE: An [OpenFHE](https://github.com/openfheorg/openfhe-development) demo used to take timings in order to compare them with the FPGA.
- SHE_SEAL_Client: A complete client implementation using SEAL and an interactive terminal. Does not take timings and returns the wrongs results, since SEAL modifications break the calculations at some point.

SHE_Client and SHE_SEAL_Client require a connection to the server. All other client applications can be executed standalone.
