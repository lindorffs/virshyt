#!/bin/bash

DOMAINS="logserver ldapserver webserver ca gitserver"           # list of domains

# Stop domains
for dom in ${DOMAINS}
do
        ../build/virshyt --status ${dom}                               # query status
        STATUS=${?}                                             # store status
        if (( ${STATUS} == 1 )); then                           # if running
                echo "${dom} is already running!"               # scream
        else
                ../build/virshyt --start ${dom}                        # start domain
                while (( ${STATUS} != 1 ))                      # while not running
                do
                        ../build/virshyt --status ${dom}               # query status
                        STATUS=${?}                             # store status
                done
                while (( ${STATUS} != 0 ))                      # while not responding
                do
                        ../build//notifier --cli ${dom} 22              # query ssh port
                        STATUS=${?}                             # store status
                done
                echo "${dom} started"                           # say started.
        fi
done

