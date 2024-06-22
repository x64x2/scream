# scream

Inspired from [this toot](https://social.coop/@foolishowl/110867475052765533).

This repo holds a Linux Kernel Module that implements `/dev/scream`.
It reads an constant stream of `a` with at random intervals `A`.

## How to use?

Install `linux-headers` through your distro.

```console
$ make
# insmod scream.ko
$ cat /dev/scream
# rmmod scream.ko
```

