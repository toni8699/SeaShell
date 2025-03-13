# Operating Systems Project

## Overview
This project focuses on building a system interpreter in C. The project includes components for process control, scheduling, shell interaction, and memory management.

## Key Features
- **Interpreter**: 
  _ Type "help" for the list of commands
  - Handles errors with specific messages for unknown commands and incorrect usage.

- **Process Control Block (PCB)**:
  - Manages process states and context switching.
  - Implements a ready queue for process scheduling.
  - Supports operations like enqueue, dequeue, and priority-based insertion.

- **Scheduler**:
  - Implements various scheduling algorithms including First-Come-First-Served (FCFS), Shortest Job First (SJF), Round Robin (RR), and Aging.
  - Manages process execution using a ready queue.
  - Supports non-preemptive and preemptive scheduling policies.

- **Shell**:
  - Provides a command-line interface for user interaction.
  - Parses user input and executes commands through the interpreter.
  - Supports both interactive and batch modes.

- **Shell Memory**:
  - Manages memory allocation and deallocation for shell processes.
  - Supports setting and retrieving key-value pairs in shell memory.
  - Handles script memory for executing batch scripts.

## Project Structure
- **src/**: Contains the source code for the project.
  - **interpreter.c**: Main interpreter implementation.
  - **pcb.c**: Process Control Block management.
  - **scheduler.c**: Scheduling algorithms.
  - **shell.c**: Command-line interface.
  - **shellmemory.c**: Memory management for shell processes.
  - **Makefile**: Build configuration.
  - **Scripts**: Various test scripts for demonstrating functionality.
## Getting Started
### Prerequisites
- GCC or another C compiler
- Make (for building the project)
### Compile and run the project
```bash
make mysh
./mysh
```







