# bp-checker
Support tool to validate which address ranges can be read/write using 
`ptrace` interface. This is mainly used for debugging. The process will try
to read/write contents into the memory region specified by the -begin/-end
parameters, from the beginning to end in increments specified using the 
-step parameter.

## Usage

### Figuring out valid regions to read/write

```
bp-checker -begin <start_address> -end <end_address> [-step <size>] -- COMMAND
```

- `-begin`: begin address to start checking
- `-end`: end address to finish checking
- `-step`: increment 

A report will be generated showing the regions that can be read/written.
