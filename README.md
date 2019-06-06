# Bulk Synchronous Parallel pattern

*BSP + problem	==>		[17:13, 21:45, 36:50]*

The **Bulk Synchronous Parallel (BSP)** parallel programming can be described as follows:

+ Parallel computations are performed by a sequence of super-steps;
+ Within each super-step, a set of independent concurrent activities is computed. Each independent activity schedules communications to other concurrent activities, which will be performed at the end of the super-step;
+ When all the super-step concurrent activities are terminated, all the communications are performed and finally the next super-step is started, with each concurrent activity being able to consume the data carried by the communications directed to that concurrent activity.

The goal in this case is to provide a pattern implementing a BSP computation such that the user may provide as parameters, for each one of the super-steps:
1. the code of the independent concurrent activities in each one of the super-steps;
2. the items to be communicated during the communication phase at the end of each super-step (data to be sent, destination (index of the destination) of the receiving concurrent activity);
3. in case of iterative algorithms, a condition on the global state returning true in case termination has been reached is also to be provided. In case the condition does not hold true, the number of the super-step to be executed again must be provided;

For the sake of simplicity, we assume all items exchanged in communications must have the same type T.

The pattern must be tested executing the Tsikin BSP sortingalgorithm described as follows:
+ the *n* input items to be sorted are distributed among *p* concurrent activities (*n/p* each, *n* must be multiple of *p*);
+ **first super-step**: each concurrent activity sorts its data portion and then selects p+1 samples uniformly distributed in the ordered sequence and including the first and the last item in the ordered sequence. At the end of the super-step, each concurrent activity sends to the all the other concurrent activities the p+1 samples;
+ **second super-step**: each concurrent activity sorts the list of all samples received, including the ones computed on its own. Then it picks up *p+1* separator items equally distributed in the sequence. Subsequently sends all the items in between separator *p* and *p+1* to concurrent activity *p*;
+ **third super-step**: each concurrent activity sorts the received items;

#### TODO list
+ [ ] add supersteps tester, to test possibility to define different numbers of activities for each superstep and possibility to specify the next superstep to be exeuted
+ [ ] fix and include bsp testers inside cmake and delivery script
+ [x] implement the two different activities composition
+ [ ] write the report