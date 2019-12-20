# VirtualMemory

Welcome to the VirtualMemory source code!

virtual_memory_t - implements simantics pointer continuous linear memory. Serves for the implementation of continuous linear memory from pieces of local memory. It is used to parse binary, nested protocols. The class object is initialized with pieces of local memory (pointer and length), and provides the ability to access as linearly continuous memory. This behavior makes it possible not to rewrite the protocol parsing algorithms on linearly continuous local memory; it is enough to change the function signature, where to transfer the virtual memory class instead of the pointer to local memory.
