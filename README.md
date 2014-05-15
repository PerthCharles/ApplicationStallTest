A Application Stall Test Suite
==============================
This is a simple client-server application suite to test the occurrence  
of 'Application Stall'.  

In this context, 'Application stall' means that the data from  
Application Layer cannot deliver to TCP layer in time. Because  
the Application Layer is slower than TCP layer, TCP Layer cannot  
transmit enough data to the other side even if allowed by cwnd  
and awnd.  

The phenomenon of 'Application Stall' was also mentioned in the paper  
of [Proportional Rate Reduction](http://research.google.com/pubs/pub37486.html) by Google in SIGCOMM'2011.  

# Four Test Modes  
* Using send() to tranfer the whole buffer with Blocking IO Socket.  

* Using send() to tranfer the whole buffer with Non-Blocking IO Socket  

* Using send() to tranfer 4KB with Blocking IO Socket, until the whole buffer is tranfered.   

* Using send() to tranfer 4KB with Non-Blocking IO Socket, until the whole buffer is tranfered.   
