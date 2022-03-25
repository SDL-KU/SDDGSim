# SDDGSim
Spatial Data Dependence Graph for CNN Accelerators (SDDGSim): \
\
Spatial data dependence graph based pre-RTL simulator for convolutional neural network dataflows, Jooho Wang, Sungkyung Park, and Chester Sungchung Park, IEEE Access, Vol. 10, 2022, pp. 11382 - 11403\
\

Abstract:
=============
This is a pre-RTL accelerator simulator that models a specific CNN dataflow under given memory constraints. Specifically, the simulator uses a spatial data dependence graph (SDDG) to exploit spatial information, such as which ALU/register each of the instructions involves (e.g., which register location the incoming pixel is loaded into). The simulator also makes it possible to evaluate the impact of the memory constraints imposed by memory block constraints in the memory hierarchy. In order to maximize performance as well as operational correctness, the pre-RTL simulator assumes a latency- and bandwidth-insensitive controller for each PE. It was validated against the Eyeriss and DSIP implementations, showing that the estimation error is less than 7% and 6%, respectively.

Requirements:
=============
We *highly* recommend that users use the `Visual Studio 2017 (x64)`,
as it has all the required dependencies installed and the environment prepared.

Build:
======
After setting the new project:
Build => Solution Build (Ctrl + Shift + B)

Run:
======
After you build SDDGSim, you can use the provided programs in the tiled third convolutional layer (M64-C1.txt) of AlexNet to test SDDGSim.
Debug => Start without debugging (Ctrl + F5) *or* Debug => Start debugging (F5)

SDDGSim will generate a text file during its execution. One file you might be interested is `DSE_OUTPUT_L[Li,Lw,Lo]_B[Bi,Bw,Bo].txt` which profiles the performance, power and area of the hardware accelerator.

Jooho Wang and Prof. Chester Sungchung Park.\
If you have any questions, please send an email to joohowang@konkuk.ac.kr\
Original release: Konkuk University, 2022\
