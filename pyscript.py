#!/usr/bin/env/python
import os
import re
import matplotlib.pyplot as plt
import numpy as np

#runs rrsim with given parameters and returns a stream to the output

def run_sim(task_num, seed, quantum, dispatch):
 
    simgen_str = ('./simgen ' + str(task_num) + " " + str(seed))
    rrsim_str = ('./rrsim ' + '--quantum ' + str(quantum) + ' --dispatch ' + str(dispatch))
    pipe_str = " | "
    grep_str = "grep EXIT"
    return os.popen(simgen_str + pipe_str + rrsim_str + pipe_str + grep_str)
    
def find_data(stream):
    w_times = []
    ta_times = []
    for line in stream.readlines():
        x = line.split(" ")
        x = x[3:5]
        x[0] = x[0][2:]
        x[1] = x[1][3:-1]
        w_times.append(float(x[0]))
        ta_times.append(float(x[1]))

    return(w_times, ta_times)

 
def main():

    task_num = 1000
    seeds = (986320, 123689, 206349, 865892, 331613, 514599, 461141, 793071, 572561, 490209,
            634323, 21811, 163387, 705034, 907773, 932423, 248612, 640785, 890277, 684726)
    quantum_lens = (50, 100, 250, 500) 
    dispatch_costs = (0,5,10,15,20,25)

    w_data = {}
    ta_data = {}
    for q in quantum_lens:
        for d in dispatch_costs:
            w_data[(q,d)] = []
            ta_data[(q,d)] = []
            for i in range(0,20):
                stream = run_sim(task_num,seeds[0], q, d)
                temp_data = find_data(stream)

                w_data[(q,d)] += temp_data[0]
                ta_data[(q,d)] += temp_data[1]

    for q in quantum_lens:
        for d in dispatch_costs:
            w_data[(q,d)] = sum(list(w_data[(q,d)]))//len(list(w_data[(q,d)]))
            ta_data[(q,d)] = sum(ta_data[(q,d)])//len(ta_data[(q,d)])

    w = plt.figure()
    w_avgs = w_data.items()
    x = np.array(dispatch_costs)
    for q in quantum_lens:
        y = np.array([float(tup[1]) for tup in w_avgs if tup[0][0] == q])
        plt.plot(x,y,marker="o", label='q='+str(q))

    plt.xlabel("Dispatch overhead")
    plt.ylabel('Average waiting time')
    plt.title("Round Robin scheduler -- # tasks 1000; seed value: 986320")
    plt.legend()
    w.savefig("graph_waiting.pdf")


    t = plt.figure()
    ta_avgs = ta_data.items()
    x = np.array(dispatch_costs)
    for q in quantum_lens:
        y = np.array([float(tup[1]) for tup in ta_avgs if tup[0][0] == q])
        plt.plot(x,y,marker="o", label='q='+str(q))

    plt.xlabel("Dispatch overhead")
    plt.ylabel('Average turnaround time')
    plt.title("Round Robin scheduler -- # tasks 1000; seed value: 986320")
    plt.legend()
    t.savefig("graph_turnaround.pdf")


    
    
"""     
    
    for i in range(0,len(w_avgs)):

        print(w_avgs[i])
        print(ta_avgs[i])
    q1 = [pack for pack in w_avgs if w_avgs[0] == 50] 
    p = plt.plot(q1[0],q1[1])
    p.xlabel("Dispatch Overhead")
    p.ylabel("Wait time")
    p.savefig('plot1.pdf')
  """  


if __name__ == "__main__":
    main()
