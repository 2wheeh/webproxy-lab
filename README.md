### adder
- 환경변수로 arg1, arg2 전달
### part 1
- valid 검사 구현 따로 못 함 -> _curl_ ,  _telnet_ 으로 들어오는 request style 대응 가능
### part 2
- thread 이용 : thread arg로 connfd와 cache_list 두 가지를 전달하기위해 myarg 구조체 활용
### part 3
- 전역변수가 아닌 thread 매개변수로 cache_list 전달하여 사용
- doubly linked list로 cache list구현
- cache 탐색 시 선형 탐색
- LRU 적용
- 세부사항 cache.c, cache.h 주석 참고 
```
####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text
```
