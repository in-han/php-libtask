<?php
/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @brief 
 *  
 **/

ini_set("error_reporting", E_ALL);
ini_set("error_log", "/tmp/error.log");
ini_set("display_errors","On");
ini_set("memory_limit","1000M");


function listen($arg)
{

    global $socket;
    if( ! isset($socket) ){
        $socket = ptask_net_listen(PTASK_NET_TCP, "127.0.0.1", 8880 );
        if( $socket == false ){
            die("create listen failed !!!\n");
        }
    }

    while( true ){
        $conn = ptask_net_accept( $socket );

        // alloc new stack for new task
        ptask_create_begin();
        ptask_create( "connection", array( 'fd'=> $conn ) );
        ptask_create_end();

        continue;
    }

    return;
}


function connection($arg){
    if( ! isset($arg['fd']) ){
        die("arg is wrong!!!\n");
    } else {
        $conn_fd = $arg['fd'];
    }

    $read = ptask_net_recv( $conn_fd, 50 );
    echo "fdread:$conn_fd, ".  $read . "\n";
    $read = ptask_net_recv( $conn_fd, 50 );
    echo "fdread:$conn_fd, ".  $read . "\n";
    
    $send = ptask_net_send( $conn_fd, $read );
    echo "send:$send\n";
    $ret = ptask_net_close( $conn_fd );
    var_dump( $ret );

    return;
}


ptask_create_begin();
ptask_create("listen", "handler1");
ptask_create_end();

ptask_run();



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
?>
