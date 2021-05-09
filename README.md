# watcher

File watcher that sends file changes to a slack channel. Written in C, for
linux. Depends on libcurl. I wrote this out of necessary and boredom. You
probably shouldn't run this code without taking a look at it.

It's in 109 lines, so it won't take too long.

## Getting started

1. Install libcurl
apt install libcurl4-openssl-dev

2. Modify THE_CHANNEL_ID/THE_SLACK_TOKEN in watcher.c
(I know, not great, but I am currently too lazy to handle that properly)

3. Compile
make

4. Watch a file
watcher my-log-file
