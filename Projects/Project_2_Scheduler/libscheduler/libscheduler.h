/** @file libscheduler.h
 */

#ifndef LIBSCHEDULER_H_
#define LIBSCHEDULER_H_
#include "../libpriqueue/libpriqueue.h"

//	------------------------------------------------------------------------------------------
//	Stores information making up a job to be scheduled including any statistics.
//	You may need to define some global variables or a struct to store your job queue elements. 
//	------------------------------------------------------------------------------------------
typedef struct _job_t_
{
	int arr_t;			//	Job arrival time
	int run_t;			//	Job run time
	int rem_t;			//	Job time remaining
	
	int pri;			//	Job Priority 
	int jid;			//	Job ID

	int init_core_t;	//	Initial Core time
	int updt_core_t;	//	Last updated cores time
} job_t;

//	------------------------------------------------------------------------------------------
//	Useful global cores structure
//	------------------------------------------------------------------------------------------
typedef struct _core_t_
{
	job_t** jobs;
	int cnt;
} core_t;

//
//	Constants which represent the different scheduling algorithms
//
typedef enum {FCFS = 0, SJF, PSJF, PRI, PPRI, RR} scheme_t;

void	scheduler_start_up					(int cores_n, scheme_t scheme);
int		scheduler_new_job					(int job_number, int time, int running_time, int priority);
int		scheduler_job_finished				(int core_id, int job_number, int time);
int		scheduler_quantum_expired			(int core_id, int time);
float	scheduler_average_turnaround_time	();
float	scheduler_average_waiting_time		();
float	scheduler_average_response_time		();
void	scheduler_clean_up					();
void	scheduler_show_queue				();

//	------------------------------------------------------------------------------------------
//	Custom Helper Functions
//	------------------------------------------------------------------------------------------
void	inc_wait	(int t);
void	inc_resp	(int t);
void	inc_turn	(int t);
void	create_core	(core_t *p, int x);
void	free_core	(core_t *p);
void	free_job	(job_t* p);
void	inc_time	(int t);
int		get_core	();
int		is_prempt	();
int		preempt		(job_t* job);
int		sch_time	(const void * p1, const void * p2);
job_t*	create_job	(int j, int a, int r, int pr);
job_t*	insert_job	(int x, job_t* job);
job_t*	delete_job	(int core, int id);


//	------------------------------------------------------------------------------------------
//	Global Var/Struct Initializations
//	------------------------------------------------------------------------------------------
priqueue_t* jobs;
scheme_t sch_type;
core_t cores;
int cur_t;

#endif /* LIBSCHEDULER_H_ */
