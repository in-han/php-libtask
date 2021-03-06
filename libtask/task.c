/* Copyright (c) 2005 Russ Cox, MIT; see COPYRIGHT */

#include "taskimpl.h"
#include <fcntl.h>
#include <stdio.h>

int	taskdebuglevel;
int	taskcount;
int	tasknswitch;
int	taskexitval;
Task	*taskrunning;
Task	main_task;      //fdtask or scheduler share this 
Task	*main_task_ptr= & main_task;      

Context	taskschedcontext;
Tasklist	taskrunqueue;

void ** zend_argument_stack_addr;
void * zend_argument_stack_new;

void ** EG_active_symbol_table_addr;
void ** EG_current_execute_data_addr;
void ** EG_scope_addr;
void ** EG_called_scope_addr;
void ** EG_active_op_array_addr;
void ** EG_start_op_addr;
void ** EG_return_value_ptr_ptr_addr;


Task	**alltask;
int		nalltask;

static char *argv0;
static	void		contextswitch(Context *from, Context *to);
static void taskinfo(int s);

static void
taskdebug(char *fmt, ...)
{
	va_list arg;
	char buf[128];
	Task *t;
	char *p;
	static int fd = -1;

return;
	va_start(arg, fmt);
	vfprint(1, fmt, arg);
	va_end(arg);
return;

	if(fd < 0){
		p = strrchr(argv0, '/');
		if(p)
			p++;
		else
			p = argv0;
		snprint(buf, sizeof buf, "/tmp/%s.tlog", p);
		if((fd = open(buf, O_CREAT|O_WRONLY, 0666)) < 0)
			fd = open("/dev/null", O_WRONLY);
	}

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	t = taskrunning;
	if(t)
		fprint(fd, "%d.%d: %s\n", getpid(), t->id, buf);
	else
		fprint(fd, "%d._: %s\n", getpid(), buf);
}

static void
taskstart(uint y, uint x)
{
	Task *t;
	ulong z;

	z = x<<16;	/* hide undefined 32-bit shift from 32-bit compilers */
	z <<= 16;
	z |= y;
	t = (Task*)z;

//print("taskstart %p\n", t);
	t->startfn(t->startarg);
    //print("taskexits %p\n", t);
	taskexit(0);
    print("not reacehd\n");
}

static int taskidgen;

static Task*
taskalloc(void (*fn)(void*), void *arg, uint stack)
{
	Task *t;
	sigset_t zero;
	uint x, y;
	ulong z;

	/* allocate the task and stack together */
	t = malloc(sizeof *t+stack);
	if(t == nil){
		fprint(2, "taskalloc malloc: %r\n");
		abort();
	}
	memset(t, 0, sizeof *t);
	t->stk = (uchar*)(t+1);
	t->stksize = stack;
	t->id = ++taskidgen;
	t->startfn = fn;
	t->startarg = arg;

	/* do a reasonable initialization */
	memset(&t->context.uc, 0, sizeof t->context.uc);
	sigemptyset(&zero);
	sigprocmask(SIG_BLOCK, &zero, &t->context.uc.uc_sigmask);

	/* must initialize with current context */
	if(getcontext(&t->context.uc) < 0){
		fprint(2, "getcontext: %r\n");
		abort();
	}

	/* call makecontext to do the real work. */
	/* leave a few words open on both ends */
	t->context.uc.uc_stack.ss_sp = t->stk+8;
	t->context.uc.uc_stack.ss_size = t->stksize-64;
#if defined(__sun__) && !defined(__MAKECONTEXT_V2_SOURCE)		/* sigh */
#warning "doing sun thing"
	/* can avoid this with __MAKECONTEXT_V2_SOURCE but only on SunOS 5.9 */
	t->context.uc.uc_stack.ss_sp = 
		(char*)t->context.uc.uc_stack.ss_sp
		+t->context.uc.uc_stack.ss_size;
#endif
	/*
	 * All this magic is because you have to pass makecontext a
	 * function that takes some number of word-sized variables,
	 * and on 64-bit machines pointers are bigger than words.
	 */
//print("make %p\n", t);
	z = (ulong)t;
	y = z;
	z >>= 16;	/* hide undefined 32-bit shift from 32-bit compilers */
	x = z>>16;
	makecontext(&t->context.uc, (void(*)())taskstart, 2, y, x);

	return t;
}

int
taskcreate(void (*fn)(void*), void *arg, uint stack)
{
	int id;
	Task *t;

	t = taskalloc(fn, arg, stack);
	taskcount++;
	id = t->id;
    
    //notmal 
    if( zend_argument_stack_new ){
        t->zend_argument_stack = zend_argument_stack_new; 
    //fdtask
    }else{
        t->zend_argument_stack = * zend_argument_stack_addr; 
    }

    t->EG_active_symbol_table = NULL;
    t->EG_current_execute_data = NULL;
    t->EG_scope = NULL;
    t->EG_called_scope = NULL;
    t->EG_active_op_array = NULL;
    t->EG_start_op = NULL;
    t->EG_return_value_ptr_ptr = NULL;

	if(nalltask%64 == 0){
		alltask = realloc(alltask, (nalltask+64)*sizeof(alltask[0]));
		if(alltask == nil){
			fprint(2, "out of memory\n");
			abort();
		}
	}
	t->alltaskslot = nalltask;
	alltask[nalltask++] = t;
	taskready(t);
	return id;
}

void
tasksystem(void)
{
	if(!taskrunning->system){
		taskrunning->system = 1;
		--taskcount;
	}
}

void
taskswitch(void)
{
	needstack(0);
    
    // save vm stack to taskrunning
    // not fdtask, not scheduler
    //if( taskrunning->zend_argument_stack != NULL ){
        // printf("taskswitch->switch running environment[save running, switch to sched]\n");
        /* save */
        taskrunning->zend_argument_stack = * zend_argument_stack_addr;   
        taskrunning->EG_current_execute_data = *EG_current_execute_data_addr;
        taskrunning->EG_active_symbol_table = *EG_active_symbol_table_addr;
        taskrunning->EG_scope = *EG_scope_addr;
        taskrunning->EG_called_scope = *EG_called_scope_addr;
        taskrunning->EG_active_op_array = *EG_active_op_array_addr;
        taskrunning->EG_start_op = *EG_start_op_addr;
        taskrunning->EG_return_value_ptr_ptr = *EG_return_value_ptr_ptr_addr;


        /*1*/* zend_argument_stack_addr = main_task_ptr->zend_argument_stack;
        /*2*/* EG_current_execute_data_addr = main_task_ptr->EG_current_execute_data;
        /*3*/* EG_active_symbol_table_addr = main_task_ptr->EG_active_symbol_table;
        /*4*/* EG_scope_addr = main_task_ptr->EG_scope;
        /*5*/* EG_called_scope_addr = main_task_ptr->EG_called_scope;
        /*6*/* EG_active_op_array_addr = main_task_ptr->EG_active_op_array;
        /*7*/* EG_start_op_addr = main_task_ptr->EG_start_op;
        /*8*/* EG_return_value_ptr_ptr_addr =  main_task_ptr->EG_return_value_ptr_ptr;

        /*switch main, no need ?*/

    //}else{
    //}
	contextswitch(&taskrunning->context, &taskschedcontext);

    //zend_argument_stack_main = * zend_argument_stack_addr;
    //* zend_argument_stack_addr = taskrunning->zend_argument_stack;
}

void
taskready(Task *t)
{
    // for a fdwait task, if task is exit directly, the fd will activate the task anyway.
	if( t->exiting == 1 ){
        return;
    }
	t->ready = 1;
	addtask(&taskrunqueue, t);
}

int
taskyield(void)
{
	int n;
	
	n = tasknswitch;
	taskready(taskrunning);
	taskstate("yield");
	taskswitch();
	return tasknswitch - n - 1;
}

int
anyready(void)
{
	return taskrunqueue.head != nil;
}

void
taskexitall(int val)
{
	exit(val);
}

void
taskexit(int val)
{
    void * t;
	taskexitval = val;
	taskrunning->exiting = 1;
    while( 1 ){
        break;
        // free argument_stack
    }
	taskswitch();
}

static void
contextswitch(Context *from, Context *to)
{
	if(swapcontext(&from->uc, &to->uc) < 0){
		fprint(2, "swapcontext failed: %r\n");
		assert(0);
	}
}

void
taskscheduler(void)
{
	int i;
	Task *t;

	taskdebug("scheduler enter");
	for(;;){
		if(taskcount == 0)
			exit(taskexitval);
		t = taskrunqueue.head;
		if(t == nil){
			fprint(2, "no runnable tasks! %d tasks stalled\n", taskcount);
			exit(1);
		}
		deltask(&taskrunqueue, t);
		t->ready = 0;
		taskrunning = t;
		tasknswitch++;
		taskdebug("run %d (%s)", t->id, t->name);

        //printf("taskswitch->switch running environment[save sched, switch to task]\n");
        // restore zend vm stack page
        // not fdtask 
        //if( taskrunning->zend_argument_stack != NULL ){
            /* save, no need ? */
            /*1*/main_task_ptr->zend_argument_stack = *zend_argument_stack_addr;
            /*1*/main_task_ptr->EG_current_execute_data = *EG_current_execute_data_addr;
            /*3*/main_task_ptr->EG_active_symbol_table = * EG_active_symbol_table_addr;
            /*4*/main_task_ptr->EG_scope = * EG_scope_addr;
            /*5*/main_task_ptr->EG_called_scope = * EG_called_scope_addr;
            /*6*/main_task_ptr->EG_active_op_array = * EG_active_op_array_addr;
            /*7*/main_task_ptr->EG_start_op = * EG_start_op_addr;
            /*8*/main_task_ptr->EG_return_value_ptr_ptr = * EG_return_value_ptr_ptr_addr;

            /* switch */
            /*1*/* zend_argument_stack_addr = taskrunning->zend_argument_stack;
            /*2*/* EG_current_execute_data_addr = taskrunning->EG_current_execute_data;
            /*3*/* EG_active_symbol_table_addr = taskrunning->EG_active_symbol_table;
            /*4*/* EG_scope_addr = taskrunning->EG_scope;
            /*5*/* EG_called_scope_addr = taskrunning->EG_called_scope;
            /*6*/* EG_active_op_array_addr = taskrunning->EG_active_op_array;
            /*7*/* EG_start_op_addr = taskrunning->EG_start_op;
            /*8*/* EG_return_value_ptr_ptr_addr =  taskrunning->EG_return_value_ptr_ptr;
        //}
        // zend_argument_stack_main = * zend_argument_stack_addr;
        
        //printf("taskscheduler, have set running, run contextswitch\n");
       // taskinfo( 0 );
		contextswitch(&taskschedcontext, &t->context);

        //* zend_argument_stack_addr = zend_argument_stack_main;
        //taskrunning->zend_argument_stack = *zend_argument_stack_addr;

		taskrunning = nil;
		if(t->exiting){
			if(!t->system)
				taskcount--;
			i = t->alltaskslot;
			alltask[i] = alltask[--nalltask];
			alltask[i]->alltaskslot = i;
			free(t);
		}
	}
}

void**
taskdata(void)
{
	return &taskrunning->udata;
}

/*
 * debugging
 */
void
taskname(char *fmt, ...)
{
	va_list arg;
	Task *t;

	t = taskrunning;
	va_start(arg, fmt);
	vsnprint(t->name, sizeof t->name, fmt, arg);
	va_end(arg);
}

char*
taskgetname(void)
{
	return taskrunning->name;
}

void
taskstate(char *fmt, ...)
{
	va_list arg;
	Task *t;

	t = taskrunning;
	va_start(arg, fmt);
	vsnprint(t->state, sizeof t->name, fmt, arg);
	va_end(arg);
}

char*
taskgetstate(void)
{
	return taskrunning->state;
}

void
needstack(int n)
{
	Task *t;

	t = taskrunning;

	if((char*)&t <= (char*)t->stk
	|| (char*)&t - (char*)t->stk < 256+n){
		fprint(2, "task stack overflow: &t=%p tstk=%p n=%d\n", &t, t->stk, 256+n);
		abort();
	}
}

static void
taskinfo(int s)
{
	int i;
	Task *t;
	char *extra;
    long top=0;

	fprint(2, "task list:\n");
	for(i=0; i<nalltask; i++){
		t = alltask[i];
		if(t == taskrunning)
			extra = " (running)";
		else if(t->ready)
			extra = " (ready)";
		else
			extra = "";
        if( t->zend_argument_stack ){
            top =  * (long*)( t->zend_argument_stack );
        }
		fprint(2, "%6d%c %-20s %s%s %x %x\n", 
			t->id, t->system ? 's' : ' ', 
			t->name, t->state, extra, (long)(t->zend_argument_stack), top );
	}
}

/*
 * startup
 */

/*
static int taskargc;
static char **taskargv;
int mainstacksize;

static void
taskmainstart(void *v)
{
	taskname("taskmain");
	taskmain(taskargc, taskargv);
}

int
main(int argc, char **argv)
{
	struct sigaction sa, osa;

	memset(&sa, 0, sizeof sa);
	sa.sa_handler = taskinfo;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGQUIT, &sa, &osa);

#ifdef SIGINFO
	sigaction(SIGINFO, &sa, &osa);
#endif

	argv0 = argv[0];
	taskargc = argc;
	taskargv = argv;

	if(mainstacksize == 0)
		mainstacksize = 256*1024;
	taskcreate(taskmainstart, nil, mainstacksize);
	taskscheduler();
	fprint(2, "taskscheduler returned in main!\n");
	abort();
	return 0;
}
*/

/*
 * hooray for linked lists
 */
void
addtask(Tasklist *l, Task *t)
{
	if(l->tail){
		l->tail->next = t;
		t->prev = l->tail;
	}else{
		l->head = t;
		t->prev = nil;
	}
	l->tail = t;
	t->next = nil;
}

void
deltask(Tasklist *l, Task *t)
{
	if(t->prev)
		t->prev->next = t->next;
	else
		l->head = t->next;
	if(t->next)
		t->next->prev = t->prev;
	else
		l->tail = t->prev;
}

unsigned int
taskid(void)
{
	return taskrunning->id;
}

