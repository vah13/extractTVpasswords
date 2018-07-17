# Extract password from TeamViewer memory using Frida (CVE-2018-14333)

&nbsp;&nbsp;Hi there, in this article we want to tell about our little research about password security in TeamViewer. The method can help during the pentest time for post exploitation to get access to another machine using TeamViewer. 

## TeamViewer automatically authentication
&nbsp;&nbsp;A few days ago I worked on my windows cloud VPS with TeamViewer (where I set a custom password). After work I disconnected, at the next time when I wanted to connect, I saw that TeamViewer had auto-filled the password.  

![tw_interface](https://github.com/vah13/extractTVpasswords/blob/master/img/tw_interface.png?raw=true)

I think “Interesting, how can i get access to the password? How is the password stored in my computer?”


## Password location
I dumped the memory of the TeamViewer and grepped password. 
![dump](https://github.com/vah13/extractTVpasswords/blob/master/img/dump.png?raw=true)

Ooo yeees, 😊 password in the memory is stored in Unicode format. It turns out that if you finish work with TeamViewer and don’t kill the process (or exit from TeamViewer

![menu](https://github.com/vah13/extractTVpasswords/blob/master/img/menu.png?raw=true)

the password will be stored in memory) 

&nbsp;&nbsp;After analyzing we understood that the first red area is a start magic data, in the second one – end magic data (from time to time, end magic data has this value = 00 00 00 20 00 00).

## Script for getting password 
To extract passwords from memory we wrote two mini programs, in Python and C++ language.

Thx Frida team for a wonderful tool! Our python script attaches to the TeamViewer.exe process, gets the base address and memory size of each library in this process. After that, it dumps one by one memory area, searches parts with [00 88] bytes at the start and [00 00 00] bytes in the end and copies them in the array. The next and the last step is choosing end decoding raws according to the regexp and password policy.

![python_work_example](https://github.com/vah13/extractTVpasswords/blob/master/img/python_work_example2.png?raw=true)

After executing the C++ code, you will get this view “asdQWE123” is the password

![cpp_poc](https://github.com/vah13/extractTVpasswords/blob/master/img/cpp_poc3.png?raw=true)
 
### For the future
1.	The programs can extract well remote ID and passwords, but he also gets some false positive dates. If we will have free time, we will try to reduce false positive rates. 
2.	Optimize C++ code from https://github.com/vah13/extractTVpasswords

#### examples
c++ example

![cppex](https://github.com/vah13/extractTVpasswords/blob/67359b4d91d9e1ff817ecf476e448e3e8e07099e/img/cpp_work_example.png?raw=true)


python example

![pyex](https://github.com/vah13/extractTVpasswords/blob/master/img/python_work_example2.png?raw=true)


[@NewFranny](https://twitter.com/NewFranny)

[@vah_13](https://twitter.com/vah_13)
