////////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
// 
// Copyright (c) 2021 Tarek Sherif
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////////

#include "events.h"
#include "../shared/debug.h"

#define MIN_EVENT_TIME 50

// Initialize sequence to "running"
void events_start(EventsSequence* sequence) {
    if (sequence->running) {
        return;
    }

    sequence->running = true;
    sequence->time = 0;
    sequence->triggeredEvent = EVENTS_NONE;
    sequence->complete = false;
    sequence->alpha = 0.0f;
}

// Stop and reset a sequence
void events_stop(EventsSequence* sequence) {
    sequence->running = false;
    sequence->time = 0;
    sequence->triggeredEvent = EVENTS_NONE;
    sequence->complete = false;
    sequence->alpha = 0.0f;
}

// Move a sequence forward by dt milliseconds.
// (Should be called at beginning of frame).
void events_beforeFrame(EventsSequence* sequence, float dt) {
    if (!sequence->running) {
        return;
    }

    sequence->triggeredEvent = EVENTS_NONE;
    sequence->alpha = 0.0f;
    float lastTime = sequence->time;
    sequence->time = lastTime + dt;

    float start = 0.0f;
    float end = 0.0f;
    for (int32_t i = 0; i < sequence->count; ++i) {
        EventsEvent* event = sequence->events + i;
        DEBUG_ASSERT(event->delay > MIN_EVENT_TIME || event->duration > MIN_EVENT_TIME, "events_beforeFrame: Events must have a duration or delay of at least 50ms.");
        start = end + event->delay;
        end = start + event->duration;
        if (start <= sequence->time && end >= lastTime) {
            if (event->duration > 0.0f) {
                sequence->alpha = (sequence->time - start) / event->duration;
                if (sequence->alpha > 1.0f) {
                    sequence->alpha = 1.0f;
                } 
            }
            sequence->triggeredEvent = event->id;
            break;
        }
    }

    if (sequence->triggeredEvent == EVENTS_NONE && lastTime > end) {
        if (sequence->loop) {
            float loopDt = lastTime - end + dt;
            sequence->time = 0.0f;
            events_beforeFrame(sequence, loopDt);
        } else {
            sequence->running = false;
            sequence->complete = true;
        }
    }

}

// Check the currently active event
bool events_on(EventsSequence* sequence, int32_t id) {
    if (!sequence->running || sequence->triggeredEvent == EVENTS_NONE) {
        return false;
    }

    return sequence->triggeredEvent == id;
}

//////////////////////////////////////////////////////////
// Events to sequence display of main title and subtitle
//////////////////////////////////////////////////////////

static EventsEvent titleControlEvents[] = {
    { 
        .delay = 1600.0f,
        .id =  EVENTS_TITLE
    },
    { 
        .delay = 1600.0f,
        .id =  EVENTS_SUBTITLE
    }
};

EventsSequence events_titleControlSequence = {
    .events = titleControlEvents,
    .count = sizeof(titleControlEvents) / sizeof(titleControlEvents[0])
};

//////////////////////////////////////////////////////////
// Title display and fadeout events
//////////////////////////////////////////////////////////

static EventsEvent titleEvents[] = {
    { 
        .duration = 2000.0f,
        .id =  EVENTS_DISPLAY
    },
    {
        .duration = 2500.0f,
        .id = EVENTS_FADE
    }
};

EventsSequence events_titleSequence = {
    .events = titleEvents,
    .count = sizeof(titleEvents) / sizeof(titleEvents[0])
};

EventsSequence events_subtitleSequence = {
    .events = titleEvents,
    .count = sizeof(titleEvents) / sizeof(titleEvents[0])
};

//////////////////////////////////////////////////////////
// Display of control instructions after opening title
//////////////////////////////////////////////////////////

static EventsEvent instructionEvents[] = {
    { 
        .delay = 500.0f,
        .duration = 3000.0f,
        .id =  EVENTS_DISPLAY
    },
    { 
        .duration = 500.0f
    }
};

EventsSequence events_instructionSequence = {
    .events = instructionEvents,
    .count = sizeof(instructionEvents) / sizeof(instructionEvents[0])
};

//////////////////////////////////
// Display game over screen
//////////////////////////////////

static EventsEvent gameOverEvents[] = {
    { 
        .delay = 1000.0f,
        .id =  EVENTS_RESTART
    }
};

EventsSequence events_gameOverSequence = {
    .events = gameOverEvents,
    .count = sizeof(gameOverEvents) / sizeof(gameOverEvents[0])
};

static EventsEvent gameOverRestartEvents[] = {
    { 
        .duration = 1000.0f,
        .id =  EVENTS_DISPLAY
    },
    { 
        .duration = 1000.0f
    }
};

EventsSequence events_gameOverRestartSequence = {
    .events = gameOverRestartEvents,
    .count = sizeof(gameOverRestartEvents) / sizeof(gameOverRestartEvents[0]),
    .loop = true
};

//////////////////////////////////
// Level transition text display
//////////////////////////////////

static EventsEvent levelTransitionEvents[] = {
    { 
        .duration = 3000.0f,
        .id =  EVENTS_DISPLAY
    }
};

EventsSequence events_levelTransitionSequence = {
    .events = levelTransitionEvents,
    .count = sizeof(levelTransitionEvents) / sizeof(levelTransitionEvents[0])
};