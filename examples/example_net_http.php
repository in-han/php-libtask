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

$connect_count = 0;

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
        if( ! is_long($conn)  ){
            continue;
            echo "Warning, accept return wrong\n"; 
            continue;
        }
        echo "get a connection:$conn\n";

        // alloc new stack for new task
        ptask_create( "connection", array( 'fd'=> $conn ) );

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
    global $connect_count;
    static $i = 0;
    $i ++;
    //echo "send:$send\n";
    echo "i:$i\n";


    while(true){
        //echo "will run ptask_net_recv ...\n";
        //$read = ptask_net_recv( $conn_fd, 256 );
        $read = ptask_net_recv( $conn_fd, 250 );
        if( $read === -2 ){
            echo "fdread timeout !!\n";
            $ret = ptask_net_close( $conn_fd );
            return;
        }else if( $read === 0 ){
            echo "fdconn[$conn_fd] is close\n";
            ptask_yield();
            $ret = ptask_net_close( $conn_fd );
            return;
        }
        //echo "fdread:$conn_fd, ".  $read . "\n";
        
        $resp_header = array(
                'HTTP/1.1 200',
                'Content-Type: text/html',
                'Connection: close',
        );
        $resp_body = 'abc';
        $resp_header[] = sprintf("Content-Length: %d", strlen( $resp_body ) );
        $resp_header_txt = implode("\r\n", $resp_header );
        $resp_txt = $resp_header_txt . "\r\n\r\n" . $resp_body;

        //while( true ) ptask_yield();
        $send = ptask_net_send( $conn_fd, $resp_txt );
        
        //close direct
        $ret = ptask_net_close( $conn_fd );
        return;
    }
    return;
    /*
    $read = ptask_net_recv( $conn_fd, 50 );
    echo "fdread:$conn_fd, ".  $read . "\n";
    
    $send = ptask_net_send( $conn_fd, $read );
    echo "send:$send\n";
    $ret = ptask_net_close( $conn_fd );
    var_dump( $ret );
     */


    return;
}


ptask_create("listen", "handler1");

ptask_run();



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
?>
