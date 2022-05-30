# SIC/XE Machine
CSE4100 System Programming Project / Sogang Univ.
## Project 1
Preliminary steps into implement SIC/XE Machine <br>
Shells that will run assemblers, linking loaders <br>
Memory Space that will load and run object code compiled <br>
Opcode table that convert mnemonic (ADD, COMP, FLOAT, etc ...) into opcode values and related commands 
* Shell (`sicsim> `)
* Commands related to Shell (`help`, `dir`, `quit`, `history`)
* Memory Space (allocated 1MB)
* Commands related to memory space (`dump`, `edit`, `fill`, `reset`)
* Opcode table (must be made into HashTable)
* Commands related to opcode (`opcode`, `opcodelist`)

## Project 2
Assemble SIC/XE assembly code file (`.asm`) <br>
Make object file(`.obj`) and listing file(`.lst`)
* Commands related to Shell (`help`, `type`)
* Assembler (`assemble`)
* Commands related to assemble (`symbol`)

## Project 3
Link several object files and load the program on memory.
* Addressing Command (`progaddr`)
* Linking Loader (`loader`)
* Run the program (`run`) $\rightarrow$ need fix
* Debug Command (`bp`)
------
* compile: `make`
* run: `a.out`
* delete: `make clean`
