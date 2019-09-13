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

#include "jobmanager.h"

rdJobManager::rdJobManager() {}

rdJobManager::~rdJobManager() { 
    this->ClearList();
}

void rdJobManager::Init() {
    this->ClearList();
}

void rdJobManager::ClearList() {
    for (int i = 0; i < this->counter; i++) {
        this->jobs[i].executed  = false;
        this->jobs[i].fn        = RD_NULL;
        this->jobs[i].data      = RD_NULL;
    }

    this->current = 0;
    this->counter = 0;
    this->limit = this->current + 1;
}

void rdJobManager::AddJob(rdFunction_t job, void * data) {   
    // ideally assert or throw exception
    if(this->counter >= RD_MAX_JOBS) 
        return;

    this->jobs[this->counter].fn = job;
    this->jobs[this->counter].data = data;
    this->jobs[this->counter].executed = false;

    this->counter++;
}

// yield current job for 'numberOfJobs' (default = 0, finish executing remainder of stack)
void rdJobManager::Yield(int numberOfJobs) {
    int oldCurrent = this->current;

    this->current++;          // Increment the index

    if (numberOfJobs != 0 && numberOfJobs > 0) {
        this->limit = this->current + numberOfJobs;
    } else {
        this->limit = (this->current + this->counter) - 1;
    }

    this->Execute(); 

    // We're done yielding, carry on with normal execution
    this->current = oldCurrent + 1;
    this->limit = this->current + 1;
}

void rdJobManager::Execute() {
    for (int i = this->current; i < this->limit && i < this->counter; i++) {
        if (this->jobs[i].executed)
            continue;

        // run the job and pass a pointer to this scheduler 
        this->jobs[i].fn(this, this->jobs[i].data);
        this->jobs[i].executed = true;
        
        this->current++; 
    }
}