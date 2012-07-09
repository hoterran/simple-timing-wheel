#simple-timing-wheel

a simple timing-wheel timer, use double list.

##data str
- wheels is a array, wheels_size is its' size.
- wheel is a double list array.
- each tick, index increment, execute the task in current wheel, granularity is each tick consume time, unit is milliseconds, 
- first is the task 's first execute relative time, interval is the task 's execute interval, they are all milliseconds.
- rotation_count, if task's first or interval is larger than max duration wheels, rotation_count will increment.
- so before execute task, need check if global rotation_count is equal task rotation_count

##task type 

- Repeat: this task will repeat execute, after current executing, calc next index and add it into list.
- Once: this task just execute once.

##use
- create a wheels, wheel_size is 60 slot, granularity is 200ms.
- add three time event into wheels.
- start timer

>Tw *tw = init(60, 200);
add(tw, 1000, 2000, a1, (void*)1, Repeat); 
add(tw, 100000, 200000, a1, (void*)2, Once); 
add(tw, 0, 10000, p1, tw, Repeat); 
start(tw);

