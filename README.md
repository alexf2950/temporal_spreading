# temporal_spreading

Agent-based simulation of disease spreading on temporal networks (SI,SIR,SIS) with rewiring options.
The simulation type, one of {SI,SIS,SIR,SIS_rewire,SIR_rewire}, has to be specified in the settings file.

## Input

The input file has to be a text file with a separated list of edges, where one edge is in the form (source, target, timestamp).
A binary version of the input file will be created on the first run to increase the speed of subsequent runs.

## Rewiring

If the rewiring option is chosen, infected nodes will become detected after the detection period passed. Contacts to
detected individuals will be avoided by rewiring them to healthy or undetected individuals.

## Groups

The program offers the possibility to read a group file, which specifies the belonging of an individual to a group.
If such a file is provided, the prevalence levels will be observed in each of the groups individually.

## More Information

will follow...
