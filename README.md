# CSP-UDL

Welcome to the GitHub repository for our research project! 
This repository contains the procedures and test instances related to our paper titled "Mathematical models and heuristics for double-Load crane scheduling in slab yards".

The repository is organized into four folders and two zip archives.
The `include` folder includes all header files, while the `src` folder contains the source files for the developed heuristics (the T-ALNS algorithm and the matheuristic).
To facilitate compilation, a `MakeFile` is provided in the `build` folder, enabling users to compile our codes independently.
For more convenience, we have precompiled all executable procedures (the developed MILP models and heuristics) and placed them in the `bin` folder, allowing users to start quickly without additional steps.

## Steps to Compile and Run

Below, we provide a step-by-step guide for compiling and executing the procedures (for Linux users).

### Step 1  
[Prepare] Before compiling/executing the procedure, please ensure that Gurobi 10.0 is installed on your platform and that the necessary environment variables are configured correctly.

### Step 2  
[Prepare] Please unzip the `instances.zip` archive into a directory of your choice.
In addition, please unzip the `Preresults.zip` archive as a folder named `Preresults`, ensuring that this folder is placed parallel to the `build` folder.
This step is essential because the computation of the $\rm Gap$ for heuristics relies on the MIP bounds, which are retrieved from the precomputed result files stored in the `Preresults` folder. 

### Step 3  
[Prepare] Before compiling the procedure, users may customize the algorithmic parameters, which are collectively defined in the `include/configure.h` file.
Furthermore, it is necessary to configure specific compilation directives in the `build/MakeFile`, such as the path to the Gurobi installation (`GRB_HOME`) and the name of the executable to be generated (`OBJS`).  

### Step 4  
[Compile] Please navigate to the `build` directory and type in the `make` command to compile our codes and generate the executable procedure.

### Step 5  
[Run] You can now execute the procedure via the command line.
Please begin by typing the procedure name in command line, followed by \textbf{six} key arguments.
The first two arguments specify the proportion of medium-span and small-span tasks in the instance. Note that the proportion of large-span tasks is defaulted to 1 and does not require user input.
The 3rd and 4th arguments represent the crane speed factors.  
The 5th argument is the number of tasks.  
The 6th argument specifies the storage path of the `instances` folder.  

For example,  if you want to utilize the T-ALNS algorithm to solve an instance with the setting of $N=30, (\psi_L, \psi_M, \psi_S) = (1, 1, 2), \tau_{\alpha\beta}' = 1.4 \tau_{\alpha\beta}$, and $\tau_{\alpha\beta}'' = 2.0 \tau_{\alpha\beta}$, you should enter the following command in the command line and press 'Enter' to strat:

```bash
./ALNS_Medium.a 1 2 1.4 2.0 30 ../instances
```

### Step 6  
[Outcome] Upon completion of the procedure, two folders, named as `Results` and `ALNSLogFile`, will be automatically created in the main directory.
The `Results` folder contains concise outcomes of the algorithm, while the `ALNSLogFile` folder is used to store the detailed solution logs for further analysis.

## Quick Start: Running Precompiled Procedures

Users can also navigate to the ‘~/bin’ directory to directly execute our precompiled procedures. 
The command for running these procedures is the same as described above.
For more comprehensive guidance, please refer to the document ‘instruction.pdf’, where we provide a detailed instruction on how to quickly get started with our procedures.
Additionally, it includes some further information on the file formats for instances, results, and logs, offering a clearer explanation of our program.



