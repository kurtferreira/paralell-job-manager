# Parallel Job Manager

Simple (unoptimised) example of a parallel job manager hard at work (inspired by the GDC Naughty Dog presentation). The philosophy behind the job manager is that each job manager runs in its own thread with a stack of jobs that it executes one at a time, allowing each job to yield (for one or all other jobs) before proceeding. The reason for this particular example existing was to illustrate a dependency free implementation of the concept.

In order to utilise, functions should be relatively minor in their execution context (think functional programming). Each function should have the data it requires to process passed to it.
	
## Defining a job
~~~~
// JOB DEFINITION
// all jobs have a pointer to their manager passed (to allow yielding or adding more jobs)
void MyJob(rdJobManager *js, const char *myparam) {
    printf("Parameter passed: %s\n", myparam);
}
~~~~

## Initialising a Job Manager
~~~~
// ideally one per hardware thread
rdJobManager manager; 

// Initialise the job manager
manager.Init(); 

// Simple parameter to pass to function
const char *myParam = "Hello World";

manager.AddJob( (rdFunction_t) MyJob, (rdParam_t) myParam);

// Execute the job list
manager.Execute(); 

// Clear out the jobs
manager.ClearList(); 
~~~~

## Yielding
In the event a job may need to wait or spin for a bit, it can allow the job manager to continue executing other jobs by calling the Yield function. If no parameters are passed, it will execute all remaining jobs in the queue. If a parameter is passed, it will yield for _n_ jobs.
~~~~
// Yield for one job
void MyJob(rdJobManager *js, const char *myparam) {
    for (int i = 0; i < 10; i++) {
        printf("Iterating index: %i\n", i);   
        // Execute 1 job and then return
        js->Yield( 1 );
    }
}
~~~~

## Multi-threaded
The purpose of parallel jobs isn't to remove multi-threading, just take advantage of not having to worry about data-sharing and 'race-cases' that plague multi-threaded programs. Ideally you want to use both so that a thread isn't sitting idle when it could be put to better use.

In the multi-threaded case, you would use the manager something along the following (pseudo-code):
~~~~
#define WORLD_THREAD 0
#define RENDER_THREAD 1

rdJobManager managers[MAX_THREADS];

void World_Updates() {
    while (GAME_RUNNING) {
        managers[WORLD_THREAD].ClearList();
    
        // ...
        // perform all your world logic that adds jobs like animating entities
        // ...
    
        managers[WORLD_THREAD].Execute();
    }
}
void Render_Updates() {
    while (GAME_RUNNING) {
        managers[WORLD_THREAD].ClearList();

        // ...
        // perform all render jobs like calculating shadows, casting rays etc.
        // ...
    
        managers[WORLD_THREAD].Execute();
    }
}

int main() {
    // Create threads
    std::thread *renderThread = new std::thread (Render_Updates);
    std::thread *worldThread = new std::thread (World_Updates);
    
    // Start threads
    renderThread->join();
    worldThread->join();
    
    // clean up
    delete renderThread;
    delete worldThread;
    
    return 0;
}
~~~~

## Conclusion
This is just a simple, unoptimised example of a parallel job manager after viewing the excellent Naughty Dog GDC presentation. There are c++11/14 "extensions" and a Boost library that will allow you to implement co-routines or Fibers much easier and with all the sugar you need, but this is just intended to be a simple exercise on a raw implementation without any dependencies.

The program was compiled on a Mac using GCC and the command:

`g++ -o jobmanager -g main.cpp jobmanager/jobmanager.cpp`
