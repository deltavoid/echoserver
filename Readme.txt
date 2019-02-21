

echoserver using standard C network libraries and pthread with one thread per connection model




synchronized editon

    sender and recver on a conneciton is synchronized.

    two varients buf_size and cons control the speed of send data.


asynchronized edition

    sender and recver are asynchronized.

    it may just try the maxnium bandwith of the NIC.