## Basic Shell implementation in C

* Executes arbitrary commands like `ls`, `ls -a -l`, `pwd`, etc
* Allows switching bettwen directories using `cd` and adding/deleting files
* Implements background jobs: the max number of bg jobs running at the same time is 5
    * The command `bglist` will display a listing of all the commands currently executing in the background, similar to:
  
         0: /home/user/a/foo
         1: /home/user/a/boo
         Total Background jobs: 2

    * The command `bgkill 1` will send the TERM signal to job 1 to terminate that job 

    * `start 1` and the `stop 1` command take a single parameter which is the job number to start or stop respectively

**Running**

Use `make` command to run the Makefile
