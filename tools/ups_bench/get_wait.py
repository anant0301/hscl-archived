wait_times = {}
most_wait = {}
for i in range(10):
    most_wait[i] = 0
most_wait[20] = 0
with open('stat.txt', 'r') as file:
    for line in file:
        parts = line.split()
        if len(parts) == 4 and parts[0] == 'Thread':
            thread_id = parts[1]
            wait_time = int(parts[3])
            if wait_time > most_wait.get(thread_id, 0):
                most_wait[thread_id] = wait_time
            wait_times[thread_id] = wait_times.get(thread_id, 0) + wait_time
total_wait = 0
for thread_id, total_wait_time in wait_times.items():
    total_wait += total_wait_time
    print("Total wait time for {}: {} {}".format(thread_id, total_wait_time/2200/1000000, most_wait[thread_id]))

print(total_wait/2200/1000000)
