Introduction
---------------
```
This project is based on https://github.com/liexusong/ptask.

The origin project(liexusong/ptask) is amazing, but there is some bugs.

Especially it doesn't deal with zend vm global variables, and this is a key problem. 
```


How to install?
---------------
```
$ git clone https://github.com/in-han/php-libtask
$ cd ptask/libtask
$ make
$ sudo make install
$ cd ../ext
$ phpize
$ ./configure --with-php-config=path-to-php-config
$ make
$ sudo make install
```
modified php.ini add configure entry: extension=ptask.so


How to use?
-----------
```php
<?php

function handler($arg)
{
	for ($i = 0; $i < 1000; $i++) {
		echo $arg, ": ", $i, "\n";
		ptask_yield();
	}
}


ptask_create("handler", "handler1");
ptask_create("handler", "handler2");

ptask_run();
```


Other examples?
-----------
```
term1:
$ php  examples/example_net_http.php

term2:
$ ab -n 10000 -c 10 http://localhost:8880/

This is ApacheBench, Version 2.0.41-dev <$Revision: 1.141 $> apache-2.0
Copyright (c) 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Copyright (c) 1998-2002 The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Finished 10000 requests


Server Software:
Server Hostname:        localhost
Server Port:            8880

Document Path:          /
Document Length:        0 bytes

Concurrency Level:      10
Time taken for tests:   0.900477 seconds
Complete requests:      10000
Failed requests:        0
Write errors:           0
Non-2xx responses:      10000
Total transferred:      390000 bytes
HTML transferred:       0 bytes
Requests per second:    11105.23 [#/sec] (mean)
Time per request:       0.900 [ms] (mean)
Time per request:       0.090 [ms] (mean, across all concurrent requests)
Transfer rate:          422.00 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       0
Processing:     0    0   0.1      0       3
Waiting:        0    0   0.1      0       3
Total:          0    0   0.1      0       3

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      0
  75%      0
  80%      0
  90%      0
  95%      0
  98%      0
  99%      0
 100%      3 (longest request)
