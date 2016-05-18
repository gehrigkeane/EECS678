/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"

//	------------------------------------------------------------------------------------------
//	Scheduler Functions
//	------------------------------------------------------------------------------------------
/**
	Initalizes the scheduler.
 
	Assumptions:
		- You may assume this will be the first scheduler function called.
		- You may assume this function will be called only once.
		- You may assume that cores is a positive, non-zero number.
		- You may assume that scheme is a valid scheduling scheme.
	@param num_cores	the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
	@param scheme	the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int num_cores, scheme_t scheme)
{
	jobs = (priqueue_t*)malloc(sizeof(priqueue_t));
	sch_type = scheme;
	priqueue_init(jobs,&sch_time);
	inc_time(0);

	create_core(&cores,num_cores);
}//scheduler_start_up


/**
	Called when a new job arrives.
 
	If multiple cores are idle, the job should be assigned to the core with the
	lowest id.
	If the job arriving should be scheduled to run during the next
	time cycle, return the zero-based index of the core the job should be
	scheduled on. If another job is already running on the core specified,
	this will preempt the currently running job.
	Assumptions:
		- You may assume that every job wil have a unique arrival time.
	@param job_number a globally unique identification number of the job arriving.
	@param time the current time of the simulator.
	@param running_time the total number of time units this job will run before it will be finished.
	@param priority the priority of the job. (The lower the value, the higher the priority.)
	@return index of core job should be scheduled on
	@return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	int i;
	inc_time(time);
	job_t* job = create_job(job_number, time, running_time, priority);

	if ( (i = get_core()) != -1 )
	{
		insert_job(i,job);
		return i;
	}//if
	else if ( is_prempt() )
	{
		i = preempt(job);
		if ( i == -1 )
			priqueue_offer(jobs,job);
		return i;
	}//else if
	priqueue_offer(jobs,job);
	return -1;
}//scheduler_new_job


/**
	Called when a job has completed execution.
 
	The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
	If any job should be scheduled to run on the core free'd up by the
	finished job, return the job_number of the job that should be scheduled to
	run on core core_id.
 
	@param core_id the zero-based index of the core where the job was located.
	@param job_number a globally unique identification number of the job.
	@param time the current time of the simulator.
	@return job_number of the job that should be scheduled to run on core core_id
	@return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	// Increment Time
	inc_time(time);

	// Process job termination
	job_t* p = delete_job(core_id,job_number);
	priqueue_remove(jobs,p);
	inc_wait(cur_t - p->arr_t - p->run_t);
	inc_turn(cur_t - p->arr_t);
	free_job(p);
	
	// Schedule new job
	p = priqueue_poll(jobs);
	if ( p != NULL )
	{
		insert_job(core_id,p);
		return p->jid;
	}//if
	return -1;
}//scheduler_job_finished


/**
	When the scheme is set to RR, called when the quantum timer has expired
	on a core.
 
	If any job should be scheduled to run on the core free'd up by
	the quantum expiration, return the job_number of the job that should be
	scheduled to run on core core_id.
	@param core_id the zero-based index of the core where the quantum has expired.
	@param time the current time of the simulator. 
	@return job_number of the job that should be scheduled on core cord_id
	@return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	// Increment Time
	inc_time(time);

	// Process quantumn rollover
	delete_job(core_id,cores.jobs[core_id]->jid);
	
	// Schedule new job
	job_t* p = priqueue_poll(jobs);
	if ( p != NULL )
	{
		insert_job(core_id,p);
		return p->jid;
	}//if
	return -1;
}//scheduler_quantum_expired

//	------------------------------------------------------------------------------------------
//	Timing Calculations
//	------------------------------------------------------------------------------------------

float WAIT_T = 0;
float RESP_T = 0;
float TURN_T = 0;

int WAIT_N = 0;
int RESP_N = 0;
int TURN_N = 0;

void inc_wait(int t)	{ WAIT_N ++; WAIT_T += t; }
void inc_resp(int t)	{ RESP_N ++; RESP_T += t; }
void inc_turn(int t)	{ TURN_N ++; TURN_T += t; }

/**
	Returns the average waiting time of all jobs scheduled by your scheduler.
	Assumptions:
		- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
	@return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return (WAIT_N == 0) ? 0.0 : (float)WAIT_T/WAIT_N;
}//scheduler_average_waiting_time


/**
	Returns the average turnaround time of all jobs scheduled by your scheduler.
	Assumptions:
		- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
	@return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return (TURN_N==0) ? 0.0 : (float)TURN_T/TURN_N;
}//scheduler_average_turnaround_time


/**
	Returns the average response time of all jobs scheduled by your scheduler.
	Assumptions:
		- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
	@return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return (RESP_N == 0) ? 0.0 : (float)RESP_T/RESP_N;
}//scheduler_average_response_time


/**
	Free any memory associated with your scheduler.
 
	Assumptions:
	- This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	void* p = NULL;
	
	while( (p = priqueue_poll(jobs)) != NULL )
		free_job((job_t*)p);
	
	priqueue_destroy(jobs);
	free(jobs);
	free_core(&cores);
}//scheduler_clean_up


/**
	This function may print out any debugging information you choose. This
	function will be called by the simulator after every call the simulator
	makes to your scheduler.
	In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:
		2(-1) 4(0) 1(-1)	

	This function is not required and will not be graded. You may leave it
	blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}

//	------------------------------------------------------------------------------------------
//	Custom Helper Functions
//	------------------------------------------------------------------------------------------
void	free_job	(job_t* p)			{ free(p); }
void	free_core	(core_t *p)			{ free(p->jobs); }
int		is_prempt	()					{ if ( sch_type == PPRI || sch_type == PSJF ) { return 1; } return 0; }
int		get_core	()					{	int i;
											for(i=0;i<cores.cnt;i++) { if ( cores.jobs[i] == NULL ) { return i; } }
											return -1;
										}
void	create_core	(core_t *p, int x)	{	int i;
											p->cnt = x;
											p->jobs = ( job_t** )malloc(sizeof(job_t*)*x);
											for( i=0; i < x; i++) p->jobs[i] = NULL;
										}
job_t*	create_job	(int j, int a, int r, int pr)
{
	job_t* p = ( job_t* )malloc(sizeof(job_t)); 
	p->arr_t = a;
	p->run_t = r;
	p->rem_t = r;
	p->pri = pr;
	p->jid = j;
	p->init_core_t = -1;
	p->updt_core_t = -1;
	return p;
}

job_t* insert_job(int x, job_t* job)
{
	if ( cores.jobs[x] != NULL )
	{
		printf("Job insertion failed on core: %d", x);
		exit(1);
	}//if - core isn't empty

	cores.jobs[x] = job;
	job->updt_core_t = cur_t;
	return job;
}//insert_job

job_t* delete_job(int core, int id)
{
	if ( cores.jobs[core]->jid != id )
	{
		printf("Delete job failed, invaild job ID");
		exit(1);
	}//if - attempt to remove invalid job from core

	job_t* job = cores.jobs[core];
	job->updt_core_t = -1;
	cores.jobs[core] = NULL;
	priqueue_offer(jobs,job);
	return job;
}//delete_job

int preempt(job_t* job)
{
	int cmp_n, cmp = 0;
	int i, x = -1;
	for ( i=0; i < cores.cnt; i++ )
	{
		cmp_n = sch_time(job, cores.jobs[i]);
		if ( cmp_n < cmp )
		{
			cmp = cmp_n;
			x = i;
		}//if
		else if ( cmp_n == cmp )
		{
			if ( x != -1 && cores.jobs[x]->arr_t < cores.jobs[i]->arr_t )
				x = i;
		}//else if
	}//for

	// insert job into core list.
	if ( x >= 0 )
	{
		delete_job(x,cores.jobs[x]->jid);
		insert_job(x,job);
	}//if

	return x;
}//preempt

int sch_time(const void * p1v, const void * p2v)
{
	job_t* p1 = (job_t*)p1v;
	job_t* p2 = (job_t*)p2v;
	int job_arr_t = (int)(p1->arr_t - p2->arr_t);
	if ( sch_type == FCFS )
		return job_arr_t;
	else if ( sch_type == SJF )
		return ((int)(p1->run_t - p2->run_t) == 0) ? job_arr_t : (int)(p1->run_t - p2->run_t);
	else if ( sch_type == PSJF )
		return ((int)(p1->rem_t - p2->rem_t) == 0) ? job_arr_t : (int)(p1->rem_t - p2->rem_t);
	else if ( sch_type == PRI )
		return ((int)(p1->pri - p2->pri) == 0) ? job_arr_t : (int)(p1->pri - p2->pri);
	else if ( sch_type == PPRI )
		return ((int)(p1->pri - p2->pri) == 0) ? job_arr_t : (int)(p1->pri - p2->pri);
	return 0;
}

void inc_time(int t)
{
	int i;
	cur_t = t;
	for ( i=0 ;i < cores.cnt; i++ )
	{
		// Catch temp job
		job_t* job = cores.jobs[i];
		if ( job )	
		{
			// if jobs is idle and not recently updated, get it scheduled and update response time
			if ( (job->init_core_t == -1) && (job->updt_core_t != cur_t) )
			{
				job->init_core_t = job->updt_core_t;
				inc_resp(job->init_core_t - job->arr_t);
			}//if

			// Update remaining times for running jobs
			job->rem_t -= cur_t - job->updt_core_t;
			job->updt_core_t = cur_t;
		}//if
	}//for
}//inc_time
