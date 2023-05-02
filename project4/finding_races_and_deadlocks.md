# Finding Races and Deadlocks
## Samson DeVol, OS444, Project 4

### Part 1: Race Conditions

```
0 if x == 12:
1 	x++;
```

**how the race occurs** 
in this case the race could occur between lines 0 and 1, in which case the if statement verifies x is equal to 12.
But, after verifying that x is 12 the value of x changes and is incremented giving a different result than the desired x == 13. 

**how to fix it**
to fix this race condition, you could use a mutex wrapped around both lines, making the if statement and incrementation an atomic action. 

```
0 if x == 12:
1 	lock(m1)
2 	x++;
3 	unlock(m1)
```
**how the race occurs**
in this case the, quite similar to the scenario above, could have line 0 run where x does equal 12.
But, berfore the mutex m1 is aquired to increment the value of x another thread operation could alter the value.

**how to fix it**
to fix this race condition, you just have to swap lines 0 and 1 so that the lock(m1) operation creates an atomic action for the if and incrementation.
```

0 if y not in hash:
1     hash[y] = 12
2 else
3     hash[y]++
```

**how the race occurs**
in this case a race condition could occur in and cause a couple probelms:
- line 0 confirms y isn't in the hash, but then y is added before line 1, and hash y is then overwritten to equal 12
- line 0 confirms y is in the hash, but then y is removed from the hash before lines 2 and 3, causing a program crashing error

**how to fix it**
add a mutex! Putting a mutex around all the lines would ensure that the hash checks and operations are atomic

```
0 x += 12;
```

**how the race occurs**
in this case the race could occur where the incrementing by 12 and setting the variable x equal to the variable are not performed atomically. 
So, it would be possible to get the value of x, add 12 to it, but before setting x equal to that value x is changed or deleted somewhere else in the code and the program is not concurrent.

**how to fix it**
yet again, add a mutex around the line of code

```
0 x = 0
1 semaphore_init(value):
2   x = value
3 
4 semaphore_signal():
5     x++;
6 
7 semaphore_wait():
8     while x == 0:
9         do nothing  # spinlock
10 
11    x--
```

**how the race occurs**
in this case there are a couple cases that could occur:
- init and signal are called simultaneously and getting the intial value of x = 0, the init value could be overwritten with a simple 1 int increment making the semaphore operate completely wrong. 
- multiple wait functions are called and execute at the same time that x is equal to, say, 1. Then both decrmement the count and x has a negative value and the wait call would no longer block since x != 0, which is not the intended functionality.
- multiple signal calls could occur, where each tries to increment on the same value of x and one or more increments are then no longer added to the count. This could result in the wait function blocking indefinitley. 

**how to fix it**
the first step is to make sure that each of the semaphore functions operate atomically, so line 2, line 5, and lines 8-11 should all be wrapped in the same mutex. Furthermore the operation init needs to be called in such a way that it is ensured to run first, because if signal was to run before init than it's value could be overwritten and the program would run incorrectly. This is a bit outside the scope of the question but I felt was a good thing to mention. 

### Part 2: Deadlocks

```
0    function1():
1        lock(m1)
2        lock(m2)
3
4        unlock(m2)
5        unlock(m1)
6
7    function2():
8        lock(m2)
9        lock(m1)
10
11       unlock(m1)
12       unlock(m2)
```

**how the deadlock occurs**
in this case, function1 could obtain lock m1, but before obtaining lock m2 function2 obtains it and vice versea. Then both threads are stuck in deadlock waitng for the locks to be relinquished, which they never will. 

**how to fix it**
there are a handful of ways to prevent this but most commonly and straight forware you could write the code in a way that the locks are obtained in a consistant manner across the functions. For example lock m1 is always obtained before lock m2. 

```
0    function1():
1        lock(m1)
2        lock(m2)
3
4        unlock(m2)
5        unlock(m1)
6
7    function2():
8        lock(m1)
9        lock(m2)
10       
11       unlock(m1)
12       lock(m1)
13
14       unlock(m2)
15       unlock(m1)
```

**how the deadlock occurs**
in this case the lock m1 can be obtained by function 2, then release on line 11. But before it can reclaim lock m1 on line 12 function 1 grabs it and a deadlock occurs

**how to fix it**
i question the necessity of unlocking and locking m1 on line 11 and 12, and would recommend looking at the code an potentially making some refactorings. 
But, you could also usa a hold-and-wait solution, where there is a preventative lock used to wrap lines 1-2, lines 8-9, and lines 11-12 making sure that all combinations of locking is done atomically including the unlocking and locking of m1 on lines 11-12 

```
0    function1(m1, m2):  # Mutexes are passed in by caller
1        lock(m1)
2        lock(m2)
3
4        unlock(m2)
5        unlock(m1)
```

**how the deadlock occurs**
in this case i'm guessing that the mutexes are passed in with the value they hold at the time of caller function being called, which means that the locks are useless since they don't represent global state. if the calling function passes a locked variable, the function1 will be blocked.

**how to fix it**
locks should be declared globally and not passed as parameter by the caller. 

