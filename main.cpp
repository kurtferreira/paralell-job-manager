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

#include <stdio.h>
#include <assert.h>
#include "jobmanager/jobmanager.h"

// Each thread has its own job stack and scheduler, but for simplicity's sake
// we'll just use a single thread
#define RD_MAX_THREADS     2       
rdJobManager managers[RD_MAX_THREADS];

// A sub-sub-routine!
void Voicemail(rdJobManager *js) {
    printf("Leaving a voicemail!\n");
}

// This function will execute 1 item, then yield for another (if any)
void Phone(rdJobManager *js, const char *name) {
    printf("Dialing %s, 1 ring at a time:\n", name);
    
    for (int p = 0; p < 10; p++) {
        printf("Ring #%i\n", p);

        // This will run only 1 other task, then return
        // js->Yield( 1 );
        
        // This will allow all other jobs on stack to continue before running again
        js->Yield();
    }

    printf("... no answer for %s!\n", name);

    // Assume the person hasn't answered, leave a voicemail
    // This is a job adding a job to the stack
    js->AddJob((rdFunction_t) Voicemail, NULL);
}

// simple message structure
struct Message_t {
    const char * from;
    const char * message;
};

void SMS(rdJobManager *js, Message_t *sms) {
    printf("Sending sms from %s\nMessage: %s\n", sms->from, sms->message);
}



int main () {
    printf("Starting...\n");

    // Just using the manager on thread 0 so we don't overcomplicate with
    // platform-specific threads

    managers[0].Init();
    
    /*
        In this scenario, we attempt to phone Kurt, and while ringing
        send an SMS to him from multiple people, and finally, when he doesn't answer
        leave a voicemail.
    */
    Message_t message1 = { "Fiora", "Good morning!"};
    Message_t message2 = { "Angela", "Hey there!"};
    Message_t message3 = { "Martin", "Howdy!"};

    managers[0].AddJob((rdFunction_t) Phone, (rdParam_t) "Kurt");
    managers[0].AddJob((rdFunction_t) SMS, (rdParam_t) &message1); // passing more complicated parameters through
    managers[0].AddJob((rdFunction_t) SMS, (rdParam_t) &message2);
    managers[0].AddJob((rdFunction_t) SMS, (rdParam_t) &message3);
 

    // Start executing the list
    managers[0].Execute();

    // Clear out the jobs
    managers[0].ClearList();

    printf("...ending.\n");

    return 0;
}