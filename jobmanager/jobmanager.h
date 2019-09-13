/*
====================================================================================

    RD Job Manager Source Code
    Copyright (C) 2018 Reload
    
    This file is part of the RD Job Manager GPL Source Code ("RD Job Manager Source Code").

    RD Job Manager Source Code is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RD Job Manager Source Code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RD Job Manager Source Code.  If not, see <https://www.gnu.org/licenses/>.

====================================================================================
*/

#ifndef __RD_JOB_MANAGER_H__
#define __RD_JOB_MANAGER_H__

#define RD_NULL         0    // null for pointers etc.
#define RD_MAX_JOBS     1024        // maximum jobs per thread

// job function declaration
typedef void (*rdFunction_t)( class rdJobManager *js, void * ); 
// helper for casting
typedef void (*rdParam_t);                                  

// Job structure
struct rdJob_t {
    void * data;            // any data associated with this job
    rdFunction_t fn;        // job function
    bool executed;  // has the job fully executed
};

class rdJobManager {
private:
    volatile int counter;   // number of jobs on stack
    volatile int current;   // currently executing job on stack
    volatile int limit;   // number of jobs to execute after yield, usually current + 1 unless asked otherwise
    rdJob_t      jobs[RD_MAX_JOBS];// array of jobs
public:
    rdJobManager();
    ~rdJobManager();

    // Initialise this scheduler
    void Init();

    // Clear out the job queue (should be done after calling Execute)
    void ClearList();

    // Add a job to the queue
    void AddJob(rdFunction_t job, void * data);

    // yield current job for 'numberOfJobs' (default = 0, finish executing remainder of stack)
    void Yield(int numberOfJobs = 0);

    // Process the current job queue
    void Execute();
};

#endif // __RD_JOB_MANAGER_H__