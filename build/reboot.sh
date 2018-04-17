#!/bin/bash
DOMAINS="gitserver ca webserver ldapserver logserver"		# list of domains

# Start domains
for dom in ${DOMAINS}
do
	./build/virtman --status ${dom}				# Query status
	STATUS=${?}						# Store status
	if (( ${STATUS} == 1 )); then				# if running
		./build/virtman --stop ${dom}			# stop domain
	fi
	while (( ${STATUS} == 1))				# while running
	do
		./build/virtman --status ${dom}			# query status
		STATUS=${?}					# store status
	done
	if (( ${STATUS} == 0 )); then				# if stopped
		echo "Stopped domain ${dom}"			# say stopped.
	fi
done

sleep 30							# sleep for 30 seconds
DOMAINS="logserver ldapserver webserver ca gitserver"		# list of domains

# Stop domains
for dom in ${DOMAINS}
do
	./build/virtman --status ${dom}				# query status
	STATUS=${?}						# store status
	if (( ${STATUS} == 1 )); then				# if running
		echo "${dom} is already running!"		# scream
	else
		./build/virtman --start ${dom}			# start domain
		while (( ${STATUS} != 1 ))			# while not running
		do
			./build/virtman --status ${dom}		# query status
			STATUS=${?}				# store status
		done
		while (( ${STATUS} != 0 ))			# while not responding
		do
			./build/notifier --cli ${dom} 22 test	# query ssh port
			STATUS=${?}				# store status
		done
		echo "${dom} started"				# say started.
	fi
done
