#virshyt: for when virsh is just to much

lets face it. restarting all the things is just hard.
this makes it kind of easier?

there are two executables, virshyt and notifier.

notifier is a simple linux socket application which
can either run as a simple accept server or a simple
connect client. no actual data is sent or received,
but tcp connections are established. this is enough
to determine the libvirt domains network status. see
examples/start.sh for an example usage.

virshyt is a linux libvirt/ncurses/socket application
which can start, stop, or check the status of a
domain on the libvirt host, as well as stopping,
starting or restarting a list of domains.

# the options
## virtman
usage: ./build/virtman --start <domain>
       ./build/virtman --stop <domain>
       ./build/virtman --status <domain>
       ./build/virtman --restart-app [with-wait] # with-wait requires root
       ./build/virtman --start-app [with-wait]   # "                     "
       ./build/virtman --stop-app

## notifier
usage: ./build/notifier --serv <port>
       ./build/notifier --cli <host> <port> <data> # the data is not actually sent.

