To run this example, download the repository from https://github.com/KULeuven-COSIC/HEAT/tree/master/ZCU102 and follow the building instructions.
Once the FPGA is programmed, navigate into firmware/project_sw of the project and delete all the files in there.
Copy all files from this repository's project_sw folder in there and start the Xilinx SDK as usual.
After starting the programm, the FPGA should output some connection information that can be used for debugging.

If the SDK is complaining about parts of lwIP missing (it seems to appear randomely), go into the bsp setting of the network core and disable lwIP in the mss settings.
After this, build the project. This step will fail but is required.
Next re-enable lwIP and build the project. This will reload all the lwIP files.
Since the default settings are suboptimal, we still need to replace the system.mss file in the network core with the provided system.mss in this repository and build the project a third time.