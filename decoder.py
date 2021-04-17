"""
Provided for you is the encryption alphabet used to encrypt the provided files.
This inclues: a-z, A-Z, 0-9, punctuation like .(); (not the same as in a1-a3), space and newline
"""

import string
alphabet = string.ascii_lowercase + string.ascii_uppercase + string.digits + string.punctuation + " \n"
import sys
import re
from os import path
sys.path.append(".")
from cipher import FileDecoder
from cipher import DecryptException
   
def main():
#INPUT
    
    #Enter a filename, while the file is not valid, ask again. If user enters 'q' return

    filename = input("Please enter a filename: ")
    while(not path.isfile(filename)):
        #Allow the user to quit
        if(filename == 'q'):
            return

        filename = input("Unable to open '{}' for reading. \nPlease enter a different file: ".format(filename))
    
    pass_is_invalid = True
    while(pass_is_invalid):
        password = input("Please enter the key for the file: ")
        #check if user is attempting to exit
        if(password == 'q'):
            return
        
        #check if password is valid
        #check password length
        check = re.match("^.{6,8}", password)
        if (not check):
            print("Password must be 6-8 characters in length, please try again.")
            continue
        #check for at least two digits
        check = re.match("^(.*\d){2,}", password)
        if (not check):
            print("Password must contain at least two digits, please try again.")
            continue
        #check for at least one uppercase
        check = re.match("^.*[A-Z]", password)
        if (not check):
            print("Password must contain at least one uppercase letter, please try again.")
            continue
        #check for EXACTLY TWO special characters
        check = re.match("^([^!@#\$&\*\-_\.]*[!@#\$&\*\-_\.]){2}[^!@#\$&\*\-_\.]*$", password)
        if (not check):
            print("Password must contain exactly two special characters, please try again.")
            continue
        
        #passed all four requirements
        pass_is_invalid = False
        #check if password successfully decrypts file
        
        try:
            file_decoder = FileDecoder(password, filename, alphabet)
        except DecryptException:
            print("'{}' failed to decrypt the '{}'.".format(password, filename))
            pass_is_invalid = True

#OUTPUT
    print("\nRESULTS")
    
    print("FileDecoder:",file_decoder)
    print("Entries:",len(file_decoder)) 
    month_tup = ('no-month','Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug','Sep','Oct','Nov','Dec')
    #Skip the first element of our list, the header row
    iterable = iter(file_decoder)
    next(iterable, None)
    curr_month = 0
    prev_month = 0
    mon_sails = 0
    mon_delay = 0
    for row in iterable:
        curr_month = int(row[4])
        if(prev_month != 0 and curr_month != prev_month):
            print("    Average delay for {}: {:.3}".format(month_tup[prev_month],mon_delay/mon_sails))
            mon_sails = 0
            mon_delay = 0
        
        expected_dept = int(row[6])*60 + int(row[7])    #in minutes since midnight
        actual_dept = int(row[11])*60 + int(row[12])    
        mon_delay += (actual_dept - expected_dept)
        prev_month = curr_month
        mon_sails += 1
        
    print("    Average delay for {}: {:.3}".format(month_tup[prev_month],mon_delay/mon_sails))   
    print("END")
if __name__ == "__main__":
    main()
