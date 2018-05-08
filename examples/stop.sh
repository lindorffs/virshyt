#!/bin/bash

DOMAINS="gitserver ca webserver ldapserver logserver"           # list of domains

# Stop domains
for dom in ${DOMAINS}
do
        ../build/virshyt --status ${dom}                        # Query status
        STATUS=${?}                                             # Store status
        if (( ${STATUS} == 1 )); then                           # if running
                ../build/virshyt --stop ${dom}                  # stop domain
        fi
        while (( ${STATUS} == 1))                               # while running
        do
                ../build//virshyt --status ${dom}               # query status
                STATUS=${?}                                     # store status
        done
        if (( ${STATUS} == 0 )); then                           # if stopped
                echo "Stopped domain ${dom}"                    # say stopped.
        fi
done

