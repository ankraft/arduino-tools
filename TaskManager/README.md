# TaskManager

This class implements a simple task manager to handle the execution of multiple tasks for task-oriented programs. Tasks can be started, stopped, executed and scheduled to be executed after a certain time. 

Using the class tremendously simplifies the handling of regular taks, such as polling input, updating output pins, or similar processes.

## Installation

- Just copy the files from this directory to your project.
- Also copy the .h and .ino files from the [LinkedList](../LinkedList/README.md) sub-project to your project. 

## Usage

### Task Manager
Instances of the class *TaskManager* are handling tasks and their execution. There could be more than one instance of this class, but in general this would not provide advantages in regards to management overhead.

### Tasks
A task has the following characteristics:

- It has exactly one *execution handler*. The execution handler is called periodically to execute a recurring task.
- It may have an *initialization handler* and an *de-initialization handler* to initialize and finalize a task.
- A task is called *periodically*, after a specified number of milliseconds.
- A task can run forever or a specified number of times.
- A task's first execution can be delayed for some time.
- A task may only run for a certain period.
- A task can be *stopped* and *restarted*.

**Please note**: Tasks are not executed in parallel. No two tasks can really execute their handlers at the same time. 

A task should return as fast a possible in order to give other tasks a chance to execute their job. If a task takes longer to execute its job in the execution handler than another task's specified interval, the other task can only be executed after that longer running task finishes its execution handler.

**Please note**: An execution handler should **never** call ```delay()```, endless loops, or other blocking functions that deliberately delay code execution artificially.

### Execution handler
An execution handler is a single function that is called periodically. It has the following signature:

```cpp
typedef bool (*TaskHandler)();
```

for example:
```cpp
bool aTask() {
	// do something
	return true;
}
```

An execution handler must return *true* if the execution was successful and the task should continue. If it returns *false* then the task is stopped.

### Initialization Handler
An *initialization handler* is an optional function to perform any form of initialization for a task. It is called when a task is started and before the first run of the *execution handler* function.

The signature for an *initialization handler* is the same as for an *execution handler*. It returns *true* if the initialization was successful and the task execution should start normally. Otherwise, *false* is returned. In this case neither the task execution is started nor the *de-initialization handler* is called.

### De-Initialization Handler
An *de-initialization handler* is another optional function to perform any form of finalization for a task. It is called when a task is stopped and after the last run of the *execution handler* function, e.g. when the number of task iterations is limited.

The signature for an *de-initialization handler* is the same as for an *execution handler*. It returns *true* if the finalization was successful and the task execution should stop normally. Otherwise, *false* is returned. In this case the task execution is not stopped.

### Creating and Running Tasks

#### Creating the TaskManager
The *TaskManager* is created by simply instantiating it, perhaps as a global variable.
```cpp
# include "TaskManager.h"

TaskManager manager;
```

#### Adding and Starting Tasks

The following adds a task that is executed every second (1000 milliseconds). 
*doSomething* is the name of a function that is called to execute the task's code. The task is activated immediately.

```cpp
manager.addTask(doSomething, 1000);
...

bool doSomething() {
	// task code
	return true;
}
```

If one doesn't want to activate a task immediately the *add()* function returns
the task identifier that can be used later to activate the task.

```cpp
int taskId = manager.addTask(doSomething, 1000, false);
manager.startTask(taskId);
```

The *startTask()* method can also be used to prolong the execution or to limit 
the number of executions of a task to a specific time or number of executions.

The following task would start after 5 seconds, run every every second, and be executed 10 times.

```cpp
int taskId = manager.addTask(doSomething, 1000, false);
manager.startTask(taskId, 5000, 0, 10);
```

The following task would start immediately, run it every second, and stop after 20 seconds.

```cpp
int taskId = manager.addTask(doSomething, 1000, false);
manager.startTask(taskId, 0, 20000, 0);
```

The following code is similar to the previous example. The difference is that 
the last parameter specifies whether a task's interval should be measured when
starting the regular execution of the task (true), ie. including the task's execution
time, or whether it should be measured when the task finished the execution 
(false), ie. excluding the task's execution time.  
The default for that parameter in other versions where it is not specified 
is *true*, ie. include the task's execution time.

```cpp
int taskId = manager.addTask(doSomething, 1000, false);
manager.startTask(taskId, 0, 20000, 0, false);	// exclude tasks execution time from interval time
```

To add initialization and de-initialization handler to a task, the following
version of the *addTask()* method must be used. This would start the task, call
the *initialization handler* function, run the task every second 10 times, and
then call the *de-initialization handler* function.

```cpp
int taskId = manager.addTask(doSomething, initSomething, deinitSomething, 1000, false);
manager.startTask(taskId, 0, 0, 10);
...
bool initSomething() {
	// task code
	return true;
}
bool doSomething() {
	// initialization code
	return true;
}
bool deinitSomething() {
	// finalization code
	return true;
}
```

#### Running Tasks

Task initialization (as described above) could be done in the the *setup()*
function of an Arduino sketch, but task execution must be triggered regularly by
calling the *TaskManager*'s *runTasks()* function. Ideally, this is done in
the *loop()* function of a sketch. In task-oriented programs that might be the
only statement in that function.

```cpp
void loop() {
  manager.runTasks();
}
```

#### Stopping Tasks

To stop a running task one calls the *TaskManager*'s *stop()* method. If set,
a *de-initialization handler* is called.

```cpp
manager.stopTask(taskId);
```

#### Removing Tasks

To remove a task one calls the *removeTask()* method. The *stopTask()* method
is called for the task, but the return value of the *de-initialization handler*
is ignored, though. 

```cpp
manager.removeTask(taskId);
```

#### Miscellaneous

The *isRunning()* method can be used to check whether a task is currently running.

```cpp
if (manager.isRunning(taskId)) {
	...
}
```

The *reset()* method can be used to stop and remove all tasks of a *TaskManager* and to reset it.

```cpp
manager.reset();
```

## Class Documentation
The *TaskManager* has the following public methods.

### Class Constructor
- **TaskManager()**  
This class only has a simple constructor.

### Class Methods
- **long addTask(TaskHandler taskHandler, unsigned long interval)**  
Add a new task.  
*taskHandler* is a pointer to a function that is called for executing the task.  
*interval* is the time in milliseconds between task executions.  
The method returns a *taskID*, or -1 in case of an error.
- **long addTask(TaskHandler taskHandler, unsigned long interval, bool autoStart)**. 
Add a new task.  
*taskHandler* is a pointer to a function that is called for executing the task.  
*interval* is the time in milliseconds between task executions.  
*autoStart* indicates whether the task execution should start implicitly, or must be started via one of the *startTask()* methods.  
The method returns a *taskID*, or -1 in case of an error.
- **long addTask(TaskHandler taskHandler, TaskHandler initTaskHandler, TaskHandler deinitTaskHandler, unsigned long interval, bool autoStart)**  
Add a new task.  
*taskHandler* is a pointer to a function that is called for executing the task.  
*initTaskHandler* is a pointer to a function that is called once when the task is started. Might be *NULL*.  
*deinitTaskHandler* is a pointer to a function that is called once when the task is stopped. Might be *NULL*.  
*interval* is the time in milliseconds between task executions.  
*autoStart* indicates whether the task execution should start implicitly, or must be started via one of the *startTask()* methods.  
The method returns a *taskID*, or -1 in case of an error.
- **void startTask(long taskId)**  
Start a task.  
*taskId* is the ID of the task to be started.
- **void startTask(long taskId, unsigned long startAfter, unsigned long runFor, unsigned long iterations)**  
Start a task.  
*taskId* is the ID of the task to be started.  
*startAfter* is the number of milliseconds after which the task is executed for the first time. 0 means immediately.  
*runFor* is the number of time in milliseconds for the task to run. 0 means forever.  
*iterations* is the number of times for the task to run. 0 means forever.
- **void startTask(long taskId, unsigned long startAfter, unsigned long runFor, unsigned long iterations, bool runOnTime)**  
Start a task.  
*taskId* is the ID of the task to be started.  
*startAfter* is the number of milliseconds after which the task is executed for the first time. 0 means immediately.  
*runFor* is the number of time in milliseconds for the task to run. 0 means forever.  
*iterations* is the number of times for the task to run. 0 means forever.  
*runOnTime* if true then the TasManager tries to run the task exactly on the time slice (ie. after the interval time, including the time the task needs to execute), otherwise after *interval* ms after the task handler returned.  The default for other *startTask()* methods for this parameter is *true*.
- **void stopTask(long taskId)**  
Stop a task.  
*taskId* is the ID of the task to be stopped.
- **void removeTask(long taskId)**  
Remove a task from the task manager.  
*taskId* is the ID of the task to be removed.
- **bool isTaskRunning(long taskId)**  
Check whether a task is currently running.
- **void reset()**  
Stop and remove all tasks and reset the task manager.
- **void runTasks()**  
Check for runnable tasks and execute them.  
This method must be called very often and regularly, ideally in the *loop()* function of a sketch.


## License
Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.
