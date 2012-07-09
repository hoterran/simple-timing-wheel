#simple-timing-wheel

##data str
wheels is a array, wheels_size is its' size.
wheel is a double list array.
each tick, index increment, execute the task in current wheel, granularity is each tick consume time, unit is milliseconds, 
first is the task 's first execute relative time, interval is the task 's execute interval, they are all milliseconds.
rotation_count, if task's first or interval is larger than max duration wheels, rotation_count will increment.
so before execute task, need check if global rotation_count is equal task rotation_count

##task type 

- Repeat: this task will repeat execute, after current executing, calc next index and add it into list.
- Once: this task just execute once.




