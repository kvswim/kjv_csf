Jeremy Bauchwitz and Kyle Verdeyen

Emails: jbauchw1@jhu.edu and kverdey1@jhu.edu

Assignment 3 

Problem 1

    
See diagram.pdf for a drawn diagram of our corrected SCRAM assembler.

First, in Dewdney’s SCRAM model the ALU only received a single input
via the 2:1 MUX; however, the ALU needs to have two inputs so that
it can perform ADD and SUB, which both require two inputs. So, we 
eliminated this MUX and directly wired two lines from the AC
and the MBR (which stores the input to be added/subtracted
to the input stored in the AC), respectively. This change seemed 
minimal to us because it really required removing an 
existing component (the MUX). 

Second, we added a register (labeled RS) for storing the output of 
the ALU, which indirectly connects back to the ALU by way of the AC.
This was added to prevent the ALU operations from theoretically producing 
an infinite loop (despite the fact that we’re using edge-triggering), 
since before the ALU would constantly add/subtract its inputs and 
immediately feed the output into the MUX that then feeds back into 
the ALU by way of the AC. This will also ensure that the correct value
is always inputted into the AC and therefore the ALU. RS receives its 
load signal from a line (x8) we added from the CLU. The inclusion
of RS seemed like a minimal addition since it’s a single register, and 
only required making one change to the CLU (the new load line). 

Third, as discussed in class Dewdney’s implementation of JMP and JMZ
were seriously flawed since the IR was only connected to the PC by way
of the AC, and so JMP/JMZ would require trashing the value stored in 
AC each time they were executed. To fix this, we needed a direct input
line from IR(0) to the PC, but this could result in the PC’s value being
changed when it shouldn’t be and the PC still needed to have an input
from AC; so, to solve this we added a 2:1 MUX that outputting to the PC
that takes its inputs from IR(0) and AC, respectively. This MUX has a
selector line (x12) that we added coming from the CLU which chooses
whether the value from IR(0) or AC updates the PC. Also, since the IR
shouldn’t overwrite AC during JMP/JMZ, the line connecting IR(0) to 
the MUX outputting to the AC is unnecessary and therefore we removed
it. We feel that these changes are minimal since we only had to add
one 2:1 MUX connecting to the PC and an extra input wire from IR(0), 
while also being able to eliminate another connection (again, the
IR(0)-to-MUX/AC wire). Note that we explain at the bottom how our 
SCRAM does the check of whether or not AC is 0, rather than in this
section.


Below we have written out our solutions for the FETCH and SCRAM
instruction microprograms, along with explanations for each:

FETCH
t0: MAR <-- PC
t1: MBR <-- M, PC <-- PC + 1
t2: IR <-- MBR 
   
Explanation: no change from what was given.

    
LDA
q1t3: MAR <-- IR(0)
q1t4: MBR <-- M
q1t5: AC <-- MBR
q1t6: <reset the counter> 
    
Explanation: no change from what was given.


LDI
q2t3: MAR <-- IR(0)
q2t4: MBR <-- M
q2t5: MAR <-- MBR
q2t6: MBR <-- M
q2t7: AC <-- MBR
q2t8: <reset the counter> 

Explanation: no change from what was given.

    
STA
q3t3: MAR <-- IR(0)
q3t4: MBR <-- AC
q3t5: M <-- MBR
q3t6: <reset the counter> 
    
Explanation: as the description of STA states, the contents of
AC are loaded into the MBR, which is then transferred into the
xth cell of memory. 

    
STI
q4t3: MAR <-- IR(0)
q4t4: MBR <-- M
q4t5: MAR <-- MBR
q4t5: MBR <-- AC
q4t7: <reset the counter> 

Explanation: similar to STA, but the value in the MBR isn’t first
loaded into the AC; rather, it is loaded into the MAR (since it
indicates the memory address of the next cell to look up). That 
cell is then overwritten by the value in AC.

    
ADD
q5t3: MAR <-- IR(0)
q5t4: MBR <-- M
q5t5: AD <-- MBR
q5t6: ALU <-- AD + AC
q5t7: RS <-- ALU
q5t8: AC <-- RS
q5t9: <reset the counter>
 
Explanation: this is no different than the given microprogram
for ADD, other than the addition of the lines indicating that
the ALU outputs to RS, which in turn provides an input to AC.

    
SUB
q6t3: MAR <-- IR(0)
q6t4: MBR <-- M
q6t5: AD <-- MBR
q6t6: ALU <-- AC - AD
q6t7: RS <-- ALU
q6t8: AC <-- RS
q6t9: <reset the counter>    

Explanation: this is the same as ADD, except we have shown that 
AD is subtracted from AC rather than being added to it (though
this is taken care of internally by the ALU).     


JMP
q7t3: PC <-- IR(0)
q7t4: <reset the counter>

Explanation: as previously explained, Dewdney’s JMP microprogram
erroneously overwrite’s the value in AC each time, when all it 
needs to do is send a signal from the IR to the PC. 

    
JMZ
q8t3: PC <-- IR(0)
q8t4: <reset the counter>

Explanation: this is the same idea as JMP, but this is only executed
when AC is 0. This check is done in the CLU by the following mechanism
(which hasn’t been drawn since it’s being handled within the CLU):
    
    First, there is a NOT gate within the CLU that inverts the signal
    from q8 (so, when JMZ is being executed q8 = 1 and !q8 = 0).
    
    Second, each bit of the input to the CLU from the AC is NOR’d with
    the output of the NOT gate and then AND’d together. Thus when both
    the AC and !q8 are 0 the the output of this operation is 1, and 
    this signal is what is routed to the selector of the MUX.


Problem 2 Notes

1. After catching each error, we print an error message to stderr and 
    then decided to have the program immediately exit (with code 0) so
    that no operations could potentially be performed on erroneous inputs.

2. We have designed our program to be as modular as possible, without
    overdoing it or allowing the modularity to become burdensome. So for 
    example, we chose not to extract our two structs to their own files 
    because they are both so small and simple that doing so seemed 
    unnecessary. 