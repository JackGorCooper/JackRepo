#!/usr/bin/env python3

import sys
import fileinput
import pathlib
import re

def main():

    flag_tup = examine_args()
    if(not flag_tup[3]):
        return
    else:
        input_file = flag_tup[0]
        sort_flag = flag_tup[1]
        print_flag = flag_tup[2]

    pair_of_dicts=process_file(input_file)
    print_results(pair_of_dicts,sort_flag,print_flag)
    return

#Examine our args, if there is a problem, print a message and return tuple with [-1] = False
def examine_args():
    #list returned as tuple of the form
    # (filename,sort_flag,print_flag,valid_file?)
    flag_list = ["", False, False, False]

    if len(sys.argv) < 3:
        print("{}: missing '--infile <filename> [--sort] [--print-words]'".format(sys.argv[0]))
        return tuple(flag_list)
    
    for i in range(1,len(sys.argv)):
        if sys.argv[i] == "--sort":
            flag_list[1] = True
        elif sys.argv[i] =="--print-words":
            flag_list[2] = True
        elif sys.argv[i] =="--infile":
            flag_list[0] = sys.argv[i+1]
            break
        
    
    path = pathlib.Path(flag_list[0])
    if path.is_file() == False:
        print("unable to open '{}' for reading".format(input_file))
        return tuple(flag_list)
    flag_list[-1] = True
    return tuple(flag_list)

#Process the file and return a tuple of two dicts, tuple[0] contains word lengths as keys, and counts as values
#tuple[1] contains word lengths as keys, and sets of words of that length (to deal with duplicates)
def process_file(input_file):
    return_tup = ({},{0:{}})
    for line in fileinput.input(input_file):
        line = line.strip()
        words = re.split('\s|[.,;()]', line)
        for word in words:
            if len(word) in return_tup[0].keys():
                return_tup[0][len(word)] += 1
                return_tup[1][len(word)].add(word)
            else:
                return_tup[0][len(word)] = 1
                return_tup[1][len(word)] = {word}
    if 0 in return_tup[0].keys():
        del return_tup[0][0]
    if 0 in return_tup[1].keys():
        del return_tup[1][0]
    return return_tup
    
#Sort our tuple_list first by right value descending, then by left value ascending    
def sort_tuple_list(li, sort_flag):
    if (sort_flag == False): 
        li.sort()
        return
    else:
        for i in range(len(li)):
            for j in range(i,len(li)):
                if li[i][1] < li[j][1]:
                    temp = li[j]
                    li[j] = li[i]
                    li[i] = temp
                elif (li[i][1] == li[j][1]) and (li[i][0] > li[j][0]):
                    temp = li[j]
                    li[j] = li[i]
                    li[i] = temp
                    
        return

#Returns the median value of int list
def median_of_list(li):
    if len(li) == 0:
        return 0
    elif len(li) == 1:
        return li[0]
    li.sort()
    if len(li) %2 == 1:
        return li[(len(li)-1)//2]
    else: 
        return (li[len(li)//2] + li[len(li)//2-1])/2 
    
def print_results(pair_of_dicts,sort_flag,print_flag):
    li = list(pair_of_dicts[0].items())
    sort_tuple_list(li, sort_flag)
    for tu in li:
        print('Count[{}]={};'.format(str(tu[0]).zfill(2), str(tu[1]).zfill(2)),end="")
        if not print_flag:
            print()
        else:
            print(" (words:",end="")
            set_of_words = list(pair_of_dicts[1][tu[0]])
            set_of_words.sort()
            for word in set_of_words:
                if len(set_of_words) < 2:
                    print(' "{}")'.format((word)))
                    break
                print(' "{}"'.format((word)),end="")
                if word == set_of_words[-2]:
                    print(" and",end="")
                elif word == set_of_words[-1]:
                    print(")")
                else:
                    print(",",end="")

    if sort_flag and not print_flag:
        keys_list = [key for (key,value) in li]
        print('Median word length: {:1.1f}'.format(median_of_list(keys_list)))


    return
#Do not change this
if __name__ == "__main__":
    main()

    
