// Sleeping locks

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "sleeplock.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_sleeplock(struct sleeplock *lk, char *name)
{
	init_channel(&(lk->chan), "sleep lock channel");
	init_spinlock(&(lk->lk), "lock of sleep lock");
	strcpy(lk->name, name);
	lk->locked = 0;
	lk->pid = 0;
}

int holding_sleeplock(struct sleeplock *lk)
{
	int r;
	acquire_spinlock(&(lk->lk));
	r = lk->locked && (lk->pid == get_cpu_proc()->env_id);
	release_spinlock(&(lk->lk));
	return r;
}

//==========================================================================

void acquire_sleeplock(struct sleeplock *lk) //TAMAM//
{
	//TODO: [PROJECT'24.MS1 - #13] [4] LOCKS - acquire_sleeplock
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	if(holding_sleeplock(lk)) {
//		panic("acquire sleep lock is already held by the same CPU.", lk->name);
//	}

	//cprintf("aquire_sleeplock called with lock: %s\n", lk->name);

	struct Env_Queue *q = &(lk->chan.queue);
	struct Env *env = get_cpu_proc();
	struct spinlock *guard = &(lk->lk);
	acquire_spinlock(guard);

	while(lk->locked) {
		//cprintf("enqueue : id = %d \n", env->env_id);
		sleep(&(lk->chan), guard);
	}


	lk->locked = 1;
	release_spinlock(guard);
	//panic("acquire_sleeplock is not implemented yet");
	//Your Code is Here...

}

void release_sleeplock(struct sleeplock *lk)
{
	//cprintf("release_sleeplock called with lock: %s\n", lk->name);
	struct Env_Queue *q = &(lk->chan.queue);
	acquire_spinlock(&(lk->lk));

	if(queue_size(q) > 0) { // if exists any sleeping processes wakup them all.
		wakeup_all(&(lk->chan));

	}
	lk->locked = 0;

	release_spinlock(&(lk->lk));
	//panic("release_sleeplock is not implemented yet");
	//Your Code is Here...

}





