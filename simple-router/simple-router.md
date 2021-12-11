**Group members:** Caleb Dahlke and Gaurish Korpal 
                
**Resources used:**

(0) Class slides and description document
(1) [Sami Niiranen](https://github.com/saminiir/level-ip)
(2) [Stanford, CS144, Fall 2009](http://www.scs.stanford.edu/09au-cs144/lab/router.html)
(3) [Princeton, COS461, Spring 2010](https://www.cs.princeton.edu/courses/archive/spr10/cos461/assignments-router.html)
(4) [UCSD, CSE123, Fall 2013](https://cseweb.ucsd.edu/classes/fa13/cse123-a/project.html)
(5) [UCLA, CS118, Spring 2017](http://web.cs.ucla.edu/classes/spring17/cs118/project-3.html)
(6) [Bryant and O'Hallaron, Computer Systems: A Programmer's Perspective](http://csapp.cs.cmu.edu/3e/home.html)
(7) Various StackOverflow posts
(8) More specific sources cited within the code.

**Description:** Implement  ARP (reply, request, cache), IP (checksum, forwarding, cache), and ICMP (checksum, ping).

**Changes made to the [stub code](https://github.com/gkorpal/simple-router):**

*  `sr_protocol.h`   :   corrected typo in line 90 of the header file.

* `sr_router.c/h`   :   added the required header files; 
                        initialized ARP cache and packet cache;
                        added methods and data structures needed for handling ARP, IP, and ICMP packets;
                        implemented checksum for both IP and ICMP packets.

**Functionalities available:**

1. Download files from web servers using wget
2. Ping the web servers and router interfaces with 0% packet loss
3. TTL decrement during  IP forward
4. Checksum of IP and ICMP packets
5. ARP cache to prevent router from unnecessary ARP reply/request