The Repository consists of multiple smaller projects, that each have their own way of running and installation requirements.
Refer to the README.md files in the project folders for further instructions.

The available projects are:
- SHE_Server: The modified hardware implementation from Roy et al. that runs on the FPGA board and prints timings when recieving ciphertexts and relin keys.
- SHE_Client: The client used for transmission timings. If you want to take measurements on the server, do it with this client implementation.
- SHE_SEAL: A little SEAL demo used to take timings in order to compare them with the FPGA.
- SHE_OpenFHE: A little OpenFHE demo used to take timings in order to compare them with the FPGA.
- SHE_SEAL_Client: A complete client implementation using SEAL and an interactive terminal. Does not take timings and returns the wrongs results, since SEAL modifications break the calculations at some point.

SHE_Client and SHE_SEAL_Client require a connection to the server. All other client applications can be executed standalone.