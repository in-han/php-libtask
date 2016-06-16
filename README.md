Introduction
---------------
```
this project is based on https://github.com/liexusong/ptask

the origin project(liexusong/ptask) is amazing, but there is some bugs.

especially it doesn't deal with zend vm global variables, this is a key problem. 


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


Other examples?
-----------
```
examples/example_net_http.php
