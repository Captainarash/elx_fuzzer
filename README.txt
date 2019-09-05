ELX Fuzzer Driver Doc

This driver is written with some goals:

1. Fuzz the higher exception code easily. This means that the values
   must be controlled from userspace without further modification of
   the driver and recompilation. 

2. Be flexible, meaning that letting you use any garbage value for
   testing and at the same time, protect kernel from panicing. 
   Any crash (hopefully) must be from higher exception levels.

3. Provide logging and tracking so user can understand what happened
   and provide feedback in order to mutate the inputs.




How to implement the driver?

The driver is a simple character device which accepts open, read, 
write and ioctl on its file located in /dev/elx_fuzzer

The idea is to receive input from userspace through write or ioctl
operations and give the output through read operation or ioctl result.

This means that we either need to make a parser in the driver or
position the parser in userspace and send the final register values
to the driver to forward it to the higher exception levels.

 
