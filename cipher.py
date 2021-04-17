"""
In this file, you need to add your FileDecoder class
See a4 PDF for details

WE WILL EVALUATE YOUR CLASS INDIVIDUAL, SO MAKE SURE YOU READ
THE SPECIFICATIONS CAREFULLY.
"""

class FileDecoder:
    
    def __init__(self, key, filename, alphabet):
        self.key = key
        self.filename = filename
        self.alphabet = alphabet
        self.rows = list()
        self.read_file()

    def __iter__(self):
        for row in self.rows:
            yield row
    
    def __str__(self):
        return "FileDecoder(key='{}', file='{}')".format(self.key, self.filename)

    def __len__(self):
        return len(self.rows)
    
    def encrypt(self, text):
        encrypted = ""
        i = 0
        for char in text:
            char_val = (self.alphabet.index(char)) 
            key_val = (self.alphabet.index(self.key[i%len(self.key)]))
            new_val = (char_val + key_val) % len(self.alphabet)
            encrypted +=self.alphabet[new_val]
            i += 1
        return encrypted

    def decrypt(self, text):
        decrypted = ""
        i = 0
        row_length = 0;
        for char in text:
            char_val = (self.alphabet.index(char))
            key_val = (self.alphabet.index(self.key[i%len(self.key)]))
            new_val = (char_val - key_val) % len(self.alphabet)
            decrypted += self.alphabet[new_val]
            if(self.alphabet[new_val] == '\n'):
                temp_list = decrypted.strip().split(",")
                self.rows.append(temp_list)
                #Detect the length of the first row in the CSV file
                if (len(self.rows) == 1):
                    row_length = len(temp_list)
                #Check each row after is the same length, if not, raise an exception
                #since there is an invalid CSV file.
                elif (row_length != len(temp_list)):
                    raise DecryptException
                    
                decrypted = ""
            i += 1

    def read_file(self):
        temp_file = open(self.filename, mode='r')
        self.decrypt(temp_file.read())
        temp_file.close()

class DecryptException(Exception):
    pass                
