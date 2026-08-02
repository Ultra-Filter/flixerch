
## Guidelines

### Project structure

- src
    - app.c
    - backend
        - recommendation_system
        - search_engine
        - database
        - web
    - core
        - job_system
        - video_decoder
        - audio_decoder
    - frontend
        - renderer
        - ui_system



## TODO's

### Week 1
- Basic renderer fubctionality
- Basic job system functionality 
- Basic web functionality 

**GOAL** *By the end of the week we want an executable that launches a window and display an image from the web loaded using curl on different thread.* 

## Assignments

### Sahar 
* Make sure this can run on your machine (check build.cmd and change path needed)
* Jira and other related stuff.
    * NO *kubernetes* and NO *kafka*
* Make sure everything is OK and if you have notes let us know.

### Eli
* Make sure this can run on your machine (check build.cmd and change path needed)
* Start reading about Threads in windows (using winapi)
* Implement basic thread function creation (Async)
* Get some movie image from TMDB using curl

### Dor
* Implement basic renderer (launch window and openGL context)
* Implement basic ui system (buttons and mvie card)
* Combine what you created with the things Eli implemented to presernt the image on the window.